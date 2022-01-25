#!/bin/bash

rm release/other/ft2play &> /dev/null
echo Compiling, please wait...

gcc -DNDEBUG -DAUDIODRIVER_SDL ../audiodrivers/sdl/*.c ../*.c src/*.c -g0 -lSDL2 -lm -lpthread -Wshadow -Winit-self -Wall -Wno-uninitialized -Wno-missing-field-initializers -Wno-unused-result -Wno-strict-aliasing -Wextra -Wunused -Wunreachable-code -Wswitch-default -O3 -o release/other/ft2play

rm ../*.o src/*.o &> /dev/null

echo Done. The executable can be found in \'release/other\' if everything went well.
