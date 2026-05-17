#!/bin/bash

echo Compiling arm64 binary, please wait...

rm release/other/ft2play &> /dev/null

clang -target arm64-apple-macos11 -mmacosx-version-min=11.0 -arch arm64 -march=armv8.3-a+sha3 -g0 -DNDEBUG -DAUDIODRIVER_AUDIOQUEUE ../audiodrivers/audioqueue/*.c ../*.c src/*.c -O3 -Winit-self -Wno-deprecated -Wextra -Wunused -mno-ms-bitfields -Wno-missing-field-initializers -Wswitch-default -framework AudioToolbox -framework Cocoa -lm -o release/other/ft2play
strip release/other/ft2play

rm ../*.o src/*.o &> /dev/null
echo Done. The executable can be found in \'release/other\' if everything went well.
