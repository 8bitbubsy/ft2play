# ft2play
Bit-accurate C port of Fasttracker 2.09's XM replayer (SB16/WAV render mode). \
The project contains example code in the ft2play folder on how to interface with the API. \
This is a direct port of the original asm/Pascal source codes.

# Notes
- To compile ft2play (the test program) on macOS/Linux, you need SDL2
- This is <i>not</i> the same replayer/mixer code used in the FT2 clone (the FT2 clone also uses a port, but it has some audio precision improvements)
- The accuracy has only been compared against a few songs
- The code may not be 100% safe to use as a replayer in other projects, and as such I recommend to use this only for reference
- The mixing amplification is set to 4x per default (same value as FT2), and this is a bit low. I recommend calling <code>setAmp(10)</code> after <code>initMusic()</code>.

# How to test accuracy
1) Open FT2.08 or FT2.09 (use a fresh program start for every render) and load an XM/MOD module. Make sure "16-bit mixing", "Stereo" and "Interpolation" are enabled in the config screen
2) Save as WAV (Freq. = 44100, Amp. = 4)
3) Render the same song to WAV using ft2play (f.ex. "ft2play mysong.xm --render-to-wav")
4) Use a program capable of verifying the binary integrity between the two output files. If they differ, you found a problem and should create a new issue for this project on GitHub :)
