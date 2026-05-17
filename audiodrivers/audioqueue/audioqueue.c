// AudioQueue audio driver for ft2play on macOS

#include <AudioToolbox/AudioQueue.h>
#include <os/lock.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../../pmp_mix.h"

static AudioQueueRef aq;
static os_unfair_lock lock = OS_UNFAIR_LOCK_INIT;

static void audioCallback(void *userdata, AudioQueueRef queue, AudioQueueBufferRef buffer)
{
	os_unfair_lock_lock(&lock);
	mix_UpdateBuffer((int16_t *)buffer->mAudioData, buffer->mAudioDataBytesCapacity / 4);
	os_unfair_lock_unlock(&lock);

	buffer->mAudioDataByteSize = buffer->mAudioDataBytesCapacity;
	AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
	(void)userdata;
}

void lockMixer(void)
{
	if (aq != 0)
		os_unfair_lock_lock(&lock);
}

void unlockMixer(void)
{
	if (aq != 0)
		os_unfair_lock_unlock(&lock);
}

bool openMixer(int32_t mixingFrequency, int32_t mixingBufferSize)
{
	AudioStreamBasicDescription desc;

	if (aq != 0)
		return true;

	memset(&desc, 0, sizeof (desc));
	desc.mFormatID = kAudioFormatLinearPCM;
	desc.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
	desc.mSampleRate = mixingFrequency;
	desc.mBitsPerChannel = 16;
	desc.mBytesPerFrame = 4;
	desc.mChannelsPerFrame = 2;
	desc.mBytesPerPacket = 4;
	desc.mFramesPerPacket = 1;

	if (AudioQueueNewOutput(&desc, audioCallback, NULL, NULL, NULL, 0, &aq) != noErr)
		return false;

	int32_t bufferByteSize = 4 * mixingBufferSize;
	for (int i = 0; i < 3; i++)
	{
		AudioQueueBufferRef buf;
		if (AudioQueueAllocateBuffer(aq, bufferByteSize, &buf) != noErr)
			goto openError;
		memset(buf->mAudioData, 0, bufferByteSize);
		buf->mAudioDataByteSize = bufferByteSize;
		AudioQueueEnqueueBuffer(aq, buf, 0, NULL);
	}

	AudioQueueStart(aq, NULL);
	return true;
openError:
	AudioQueueDispose(aq, true);
	aq = 0;
	return false;
}

void closeMixer(void) {
	if (aq != 0)
	{
		AudioQueueStop(aq, true);
		AudioQueueDispose(aq, true);
		aq = 0;
	}
}
