/* Example program for interfacing with ft2play.
**
** Please excuse my disgusting platform-independant code here...
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../pmplay.h"
#include "posix.h"

// defaults when not overriden by argument switches
#define DEFAULT_MIX_FREQ 44100
#define DEFAULT_MIX_BUFSIZE 1024
#define DEFAULT_MIX_AMP 10
#define DEFAULT_MASTERVOL 256
#define DEFAULT_INTRP_FLAG true
#define DEFAULT_VRAMP_FLAG true

// set to true if you want ft2play to always render to WAV
#define DEFAULT_WAVRENDER_MODE_FLAG false

// default settings
static bool renderToWavFlag = DEFAULT_WAVRENDER_MODE_FLAG;
static bool interpolation = DEFAULT_INTRP_FLAG;
static bool volumeRamping = DEFAULT_VRAMP_FLAG;
static int32_t mixingAmp = DEFAULT_MIX_AMP;
static int32_t masterVolume = DEFAULT_MASTERVOL;
static int32_t mixingFrequency = DEFAULT_MIX_FREQ;
static int32_t mixingBufferSize = DEFAULT_MIX_BUFSIZE;

// ----------------------------------------------------------

static volatile bool programRunning;
static char *filename, *WAVRenderFilename;

static void showUsage(void);
static void handleArguments(int argc, char *argv[]);
static void readKeyboard(void);
static int32_t renderToWav(void);

// yuck!
#ifdef _WIN32
void wavRecordingThread(void *arg)
#else
void *wavRecordingThread(void *arg)
#endif
{
	(void)arg;
	WAVDump_Record(WAVRenderFilename);
#ifndef _WIN32
	return NULL;
#endif
}

#ifndef _WIN32
static void sigtermFunc(int32_t signum)
{
	programRunning = false; // unstuck main loop
	WAVDump_Flag = false; // unstuck WAV render loop
	(void)signum;
}
#endif

int main(int argc, char *argv[])
{
	if (argc < 2 || (argc == 2 && (!strcmp(argv[1], "/?") || !strcmp(argv[1], "-h"))))
	{
		showUsage();
		return 1;
	}

	handleArguments(argc, argv);

	// TODO: Return proper error codes to tell the user what went wrong...
	if (!initMusic(mixingFrequency, mixingBufferSize, interpolation, volumeRamping))
	{
		printf("Error: Out of memory while setting up replayer!\n");
		return 1;
	}

	// TODO: Return proper error codes to tell the user what went wrong...
	if (!loadMusic(filename))
	{
		printf("Error: Couldn't load song!\n");
		return 1;
	}

	// you only need to make a call to these if amp != 4 and/or mastervol != 256, which are the FT2 defaults
	if (mixingAmp != 4) setAmp(mixingAmp);
	if (masterVolume != 256) setMasterVol(masterVolume);

	// trap sigterm on Linux/macOS (since we need to properly revert the terminal)
#ifndef _WIN32
	struct sigaction action;
	memset(&action, 0, sizeof (struct sigaction));
	action.sa_handler = sigtermFunc;
	sigaction(SIGTERM, &action, NULL);
#endif

	if (renderToWavFlag)
		return renderToWav();

	if (!startMusic())
	{
		printf("Error: Couldn't start the audio system!\n");
		freeMusic();
		return 1;
	}

	startPlaying();

	printf("Playing, press ESC to stop...\n");
	printf("\n");
	printf("Controls:\n");
	printf("Esc=Quit   Space=Toggle Pause   Plus = inc. song pos   Minus = dec. song pos\n");
	printf("\n");
	printf("Mixing frequency: %dHz\n", realReplayRate);
	printf("Linear interpolation: %s\n", interpolation ? "On" : "Off");
	printf("Volume ramping: %s\n", volumeRamping ? "On" : "Off");
	printf("Mixing amp: %d\n", boostLevel);
	printf("Mixing volume: %d\n", masterVol);
	printf("\n");
	printf("Name: %s\n", song.name);
	printf("Channels: %d\n", song.antChn);
	printf("Instruments: %d\n", song.antInstrs);
	printf("Song length: %d (restart pos: %d)\n", song.len, song.repS);
	printf("\n");

	printf("- STATUS -\n");

#ifndef _WIN32
	modifyTerminal();
#endif

	programRunning = true;
	while (programRunning)
	{
		readKeyboard();

		printf(" Pos: %03d/%03d - Pattern: %03d - Row: %03d/%03d - Active voices: %02d/%02d %s\r",
			song.songPos, song.len, song.pattNr, song.pattPos, song.pattLen,
			getNumActiveVoices(), song.antChn, musicPaused ? "(PAUSED)" : "        ");
		fflush(stdout);

		Sleep(100);
	}

#ifndef _WIN32
	revertTerminal();
#endif

	printf("\n");

	stopMusic();
	freeMusic();

	printf("Playback stopped.\n");
	return 0;
}

static void showUsage(void)
{
	printf("Usage:\n");
	printf("  ft2play input_module [-f hz] [-b buffersize] [-a amp] [-m mastervol]\n");
	printf("  ft2play input_module [--no-intrp] [--no-vramp] [--render-to-wav]\n");
	printf("\n");
	printf("  Options:\n");
	printf("    input_module     Specifies the module file to load (.XM/.MOD/.FT supported)\n");
	printf("    -f hz            Specifies the mixing frequency (8000..96000)\n");
	printf("    -b buffersize    Specifies the mixing buffer size (256..8192)\n");
	printf("    -a amp           Specifies the mixing amplitude (1..32)\n");
	printf("    -m mastervol     Specifies the mixing master volume (0..256). This setting\n");
	printf("                     is ignored when rendering to WAV (always set to 256).\n");
	printf("    --no-intrp       Disables linear interpolation\n");
	printf("    --no-vramp       Disables volume ramping\n");
	printf("    --render-to-wav  Renders song to WAV instead of playing it. The output\n");
	printf("                     filename will be the input filename with .WAV added to the\n");
	printf("                     end.\n");
	printf("\n");
	printf("Default settings:\n");
	printf("  - Mixing frequency:         %d\n", DEFAULT_MIX_FREQ);
	printf("  - Mixing buffer size:       %d\n", DEFAULT_MIX_BUFSIZE);
	printf("  - Amp:                      %d\n", DEFAULT_MIX_AMP);
	printf("  - Master volume:            %d\n", DEFAULT_MASTERVOL);
	printf("  - Linear interpolation:     %s\n", DEFAULT_INTRP_FLAG ? "On" : "Off");
	printf("  - Volume ramping:           %s\n", DEFAULT_VRAMP_FLAG ? "On" : "Off");
	printf("  - WAV render mode:          %s\n", DEFAULT_WAVRENDER_MODE_FLAG ? "On" : "Off");
	printf("\n");
}

static void handleArguments(int argc, char *argv[])
{
	filename = argv[1];
	if (argc > 2) // parse arguments
	{
		for (int32_t i = 1; i < argc; i++)
		{
			if (!_stricmp(argv[i], "-f") && i+1 < argc)
			{
				const int32_t num = atoi(argv[i+1]);
				mixingFrequency = CLAMP(num, 8000, 96000);
			}
			else if (!_stricmp(argv[i], "-b") && i+1 < argc)
			{
				const int32_t num = atoi(argv[i+1]);
				mixingBufferSize = CLAMP(num, 256, 8192);
			}
			else if (!_stricmp(argv[i], "-a") && i+1 < argc)
			{
				const int32_t num = atoi(argv[i+1]);
				mixingAmp = CLAMP(num, 1, 32);
			}
			else if (!_stricmp(argv[i], "-m") && i+1 < argc)
			{
				const int32_t num = atoi(argv[i+1]);
				masterVolume = CLAMP(num, 0, 256);
			}
			else if (!_stricmp(argv[i], "--no-intrp"))
			{
				interpolation = false;
			}
			else if (!_stricmp(argv[i], "--no-vramp"))
			{
				volumeRamping = false;
			}
			else if (!_stricmp(argv[i], "--render-to-wav"))
			{
				renderToWavFlag = true;
			}
		}
	}
}

static void readKeyboard(void)
{
	if (_kbhit())
	{
		const int32_t key = _getch();
		switch (key)
		{
			case 0x1B: // esc
				programRunning = false;
			break;

			case 0x20: // space
				toggleMusic();
			break;

			case 0x2B: // numpad +
				song.globVol = 64;
				stopVoices(); // prevent stuck notes
				setPos(song.songPos + 1, 0);
			break;

			case 0x2D: // numpad -
				song.globVol = 64;
				stopVoices(); // prevent stuck notes
				setPos(song.songPos - 1, 0);
			break;
			
			default: break;
		}
	}
}

static int32_t renderToWav(void)
{
	const size_t filenameLen = strlen(filename);
	WAVRenderFilename = (char *)malloc(filenameLen+5);

	if (WAVRenderFilename == NULL)
	{
		printf("Error: Out of memory!\n");
		freeMusic();
		return 1;
	}

	strcpy(WAVRenderFilename, filename);
	strcat(WAVRenderFilename, ".wav");

	/* The WAV render loop also sets/listens/clears "WAVDump_Flag", but let's set it now
	** since we're doing the render in a separate thread (to be able to force-abort it if
	** the user is pressing a key).
	**
	** If you don't want to create a thread for the render, you don't have to
	** set this flag, and you just call WAVDump_Record("output.wav") directly.
	** Though, some songs will render forever (if they Bxx-jump to a previous order),
	** thus having this in a thread is recommended so that you can force-abort it, if stuck.
	*/
	WAVDump_Flag = true;
	if (!createSingleThread(wavRecordingThread))
	{
		printf("Error: Couldn't create WAV rendering thread!\n");
		free(WAVRenderFilename);
		freeMusic();
		return 1;
	}

	printf("Rendering to WAV. If stuck forever, press any key to stop rendering...\n");

#ifndef _WIN32
	modifyTerminal();
#endif
	while (WAVDump_Flag)
	{
		Sleep(200);
		if ( _kbhit())
			WAVDump_Flag = false;
	}
#ifndef _WIN32
	revertTerminal();
#endif

	closeSingleThread();

	free(WAVRenderFilename);
	freeMusic();

	return 0;
}

