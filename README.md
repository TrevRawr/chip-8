# Chip-8 Emulator

## About
In case there weren't enough Chip-8 emulators already, I decided to make one more! My goals for this project were not to make the most feature-complete emulator, but rather to learn. My goals were to 
- Learn how emulation works
- Get better at C++
- Get better at testing code
- Learn how to develop cross-platform applications

I believe I met these goals for the most part. The emulator is functional, it has a full test suite for almost every opcode, and it runs on multiple platforms.

## What's Working
Every standard Chip-8 opcode is implemented, except for the opcodes that make sound. It's worth noting that I have set the clock speed slightly faster than the chip-8 spec specifies, because the default spec felt too slow to me. 
Other than running too fast, and being soundless, the few games I've tested with (Pong, Space Invaders, etc...) seem to work fine. 
Though there are test cases for just about every opcode, there are probably some small bugs somewhere that may surface with some ROMs.

## Usage
Supported platforms are macOS, and other Unix/Unix-like Operating Systems (Linux). I have tested building and running on Ubuntu 16.04 and macOS 10.11 El Capitan, but it should work on other platforms as well. It may even work on windows if Cygwin is installed (though this is untested). 

To run the emulator, first clone the project. Inside the project directory folder, run the following commands:

1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make`
5. `./chip_8 <path_to_your_ROM_here>`

You shouldn't have to install any dependencies in order to get the project working. The only real dependency is SDL2, and it should be downloaded and built automatically when you run the Cmake build file. 

## Future Goals
I have already achieved most of what I set out to learn with this project, but I would like to continue porting it to more platforms. In particular, I would like to try to port it to iOS and Android. I don't have any timeline in mind for when I plan to do this (maybe never!) but it would be a fun way to continue this project. 

## Thanks
When I started this project, I knew nothing about emulation, and didn't really know where to begin. Thankfully, I found a [fantastic guide by Laurence Muller](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/) that helped me get started. 