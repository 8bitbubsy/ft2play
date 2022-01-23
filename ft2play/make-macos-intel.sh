#!/bin/bash

echo Compiling 64-bit Intel binary, please wait...

rm release/other/ft2play &> /dev/null

clang -mmacosx-version-min=10.7 -arch x86_64 -mmmx -mfpmath=sse -msse2 -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -g0 -DNDEBUG -DAUDIODRIVER_SDL ../audiodrivers/sdl/*.c ../*.c src/*.c -O3 -lm -Winit-self -Wno-deprecated -Wextra -Wunused -mno-ms-bitfields -Wno-missing-field-initializers -Wswitch-default -framework SDL2 -framework Cocoa -lm -o release/other/ft2play
strip release/other/ft2play
install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 @executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2 release/other/ft2play

rm ../*.o src/*.o &> /dev/null
echo Done. The executable can be found in \'release/other\' if everything went well.
