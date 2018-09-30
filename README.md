# Project Status

![Build status](https://img.shields.io/badge/build-success-brightgreen.svg)
![Version](https://img.shields.io/badge/version-alpha--2-red.svg)

The project is still under moderate development, so far the exact direction of it isn't know. It can quickly change aspect as it can stay as it is for months.

# Dependencies

The project has been developed using SDL2, SDL2_Image and SDL2_TTF, for more information about what it is you can [check it here](https://www.libsdl.org/download-2.0.php).

# Description

**This project is just a poor attempt of applying a couple of basic electrostatics principles, that have a n-body-simulation-alike feeling. Nothing is accurately coded or represented.**

In this simulation you have 3 distinct types of elementary particles, negatively charged ones *(red dots)*, positiviely charged ones *(green dots)* and neutrally charged particles *(white dots)*.

The colors of the lines represent the attraction between two bodies. *Green* line signifies that the bodies are attracted to eachother, *red* line means that the bodies are repulsing eachother. Once two bodies collide they cancel eachother becoming a netural particle. Since in this simulation a neutral particle have no sense of charge, they're entirely stationary.

# Installation & Compilation

This project has been developed on Linux so far, so here's a quick shortcut on what packages you should install:

```bash
sudo apt-get install sdl2lib-dev sdl2lib-image-dev sdl2lib-ttf-dev
```

Once you've got the libraries installed just do `make` in the project root directory and you should be able to run it flawlessly.

# Preview

This is how a simulation with initially 10 particles launched looks like.

![Preview](repo/media/ss_nbody_sim_1.png)

# Known bugs & caveats

- The text rendering is very choppy.
- Computation algorithm can potentially be improved.
