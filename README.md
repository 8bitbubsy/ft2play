# ft2play
Aims to be a bit-accurate C port of Fasttracker 2.09's XM replayer (SB16/WAV render mode). \
This is a direct port of the original asm/Pascal source codes. \
\
The project contains example code in the ft2play folder on how to interface with the API.

# Notes
- To compile ft2play (the test program) on macOS/Linux, you need SDL2
- When compiling, you need to pass the driver to use as a compiler pre-processor definition (f.ex. AUDIODRIVER_WINMM, check "pmplay.h")
- This is <i>not</i> the same replayer/mixer code used in the FT2 clone (the FT2 clone also uses a port, but it has some audio precision improvements)
- The accuracy has only been compared against a handful of songs
- The code may not be 100% thread-safe (or safe in general), and as such I don't really recommend using this replayer in other projects. My primary goal was to create an accurate C port that people can use for reference.

# How to test accuracy
1) Open FT2.08 or FT2.09 (use a fresh program start for every render) and load an XM/MOD module. Make sure "Stereo" and "Interpolation" are enabled in the config screen
2) Save as WAV with the following settings: Frequency = 44100, Amplification = 10 (not 4!)
3) Render the same song to WAV using ft2play (f.ex. "ft2play mysong.xm --render-to-wav")
4) Use a program capable of verifying the binary integrity between the two output files. If they differ, you found a problem, please create a GitHub issue for it :)
