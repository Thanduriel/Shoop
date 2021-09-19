![Shoop](/content/texture/Background.png)
The long awaited C++ remake of [Shoop](https://github.com/Gnaarf/UnicycleSheep)!
This version includes all parts of the original and also comes with various improvements such as adjustable resolution, linux support and a revamped physics model.

## About
> There are sheep on remote controlled unicycles.

Originally developed during a gamejam in 2015 Shoop has become classic at Acagamics events.
This local multiplayer game tasks two players with leading there sheep to victory in a unicycle based tournament.
The goal is to have the opposing sheep hit their head on some surface bevor yours does.

## Build
Building requires a C++ 20 compiler (tested with g++-10.3 and msvc-19.29).
Dependencies are integrated as submodules so you just need to run
```sh
$ git clone --recursive https://github.com/Thanduriel/Shoop
$ cd Shoop
$ mkdir build
$ cd build
$ cmake ..
```
You can skip the SFML submodule if you have another version installed that cmake can find.