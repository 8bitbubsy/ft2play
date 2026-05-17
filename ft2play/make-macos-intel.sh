#!/bin/bash

echo Compiling 64-bit Intel binary, please wait...

rm release/other/ft2play &> /dev/null

clang -mmacosx-version-min=10.7 -arch x86_64 -mmmx -mfpmath=sse -msse2 -g0 -DNDEBUG -DAUDIODRIVER_AUDIOQUEUE ../audiodrivers/audioqueue/*.c ../*.c src/*.c -O3 -Winit-self -Wno-deprecated -Wextra -Wunused -mno-ms-bitfields -Wno-missing-field-initializers -Wswitch-default -framework AudioToolbox -framework Cocoa -lm -o release/other/ft2play
strip release/other/ft2play

rm ../*.o src/*.o &> /dev/null
echo Done. The executable can be found in \'release/other\' if everything went well.
