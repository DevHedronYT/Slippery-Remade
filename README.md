# Slippery-Remade
Remaking My First Game Using My SDL2 Framework

## To Compile
- Install Ninja tool and make sure to have Mingw or GCC
- Latest version of game has not been tested on other platforms, only Windows 10
- Run the run.bat file


- [YouTube Devlog](https://youtu.be/u2CqcR7XSrg)
- [Original Game](https://devhedronyt.itch.io/slippery-the-game)


- [Original Engine](https://github.com/DevHedronYT/Game)

- *Note some compilation programs such as art.c cannot be compiled now as I haven't fixed their paths in Ninja*

## Engine Features
- Made using C, SDL2, SDL_gfx library which can be found online and 
inspired by DaFluffyPotato's games and my previous pygame games
- Using VSC for creating the game:
    - .vscode
    - Utilizing VSC task system to compile using script

- Build system is Ninja:
    - Highly efficient
    - Different platform support and target support
    - Using .bat script to compile
    - Fast & saves time
    - How it works:
        - Config file contains variables which determines how and what is to be compiled

- Build folder contains different platform folders and resources..etc..
- This project has different parts obviously and the abstraction is done in the following way:
    - SDL2 Library + SDL_gfx Library + Utils -> Engine -> TARGET
    - Utils (inspired by extd_cstd):
        - Features:
            - Smaller type names
            - File loading and writing
            - Hash table
            - Lists
            - Efficient logging system
            - Math functions (lerp, distance, 2d vectors, random numbers)
            - Small profiler
            - String conversion functions
            - Configuration file parser

    - Engine:
        - Features:
            - Init and uninitalization information struct
            - Props struct containing necessary program information
            - Debug functions:
                - Frame time
                - Memory used
            - Image & Animation functions
            - Music & SFX
            - UI functions
            - Physics functions
            - Camera
            - Circles
            - Particles
            - Drawing functions
            - "Serizalization" functions
            - Grid & Color functions
            - Deltatime/Framerate-Independence
            - Event functions
            - ECS
            - Timers


    - I have also taken the courtesy of making this project in such a way that it is completely offline and
    online things don't need to be used. I already downloaded the .html version of beepbox for creating music.

