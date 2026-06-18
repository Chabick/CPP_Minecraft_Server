# Minecraft Server written in C/C++

This is a small minecraft server for version 1.21.11 written in C/C++, fokusing on better performance and lower ram usage than 
the normal java architecture, by utilising multithreading and better datastructures.
This project is currently still in the first stages, so no actual gameplay is yet possible, but I am trying to implement
as many game mechanics as soon as possible, as well as included anti-cheat (anti-fly/-xray/-speed/...). The entire project
is written without the help of artifical intelligence and is expected to be kept this way in the future.

If you wan't to build this project for your self, you need to use the meson build system and gcc/cc compiler in combination with
ninja:
1. install meson, ninja, gcc, cc on your system (currently only windows is supported as the project uses the winsock2 headers, thus mingw64 is recommended)
2. clone the repository and open the command prompt in the project root
3. configure meson to use the gcc/cc compiler with ninja
4. run "meson setup builddir && cd builddir && meson compile"
5. move all dlls generated in /builddir/subprojects/... and all system-dlls required into a single folder and execute the programm

Be aware that I take no responsibility for any damages this program might cause on your system. You are allowed to use & modify my code, as long as I am
credited and the project resulting from my code is not marketed, sold, or made money of in any way.
