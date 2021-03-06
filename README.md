# Lindenmayer System Renderer

[Lindenmayer systems](https://en.wikipedia.org/wiki/L-system) are a method of modelling plants and generating fractals.

This software is meant to facilitate rendering and capturing L-systems. It uses SDL2 & OpenGL to draw the L-system and can capture the resulting image in PNG format.

Included is an example.ini configuration file. An ini must be provided that defines the window parameters, draw settings, and L-system definitions that the software will use. You can also provide an output filepath to which the PNG will be saved. 

### Synopsis

`./lsystem.exe [-c inipath] [-o [pngpath]]`

### Description

**-c,--config *inipath***

Use *inipath* config file instead of example.ini (default)

**-o,--output [*pngpath*]**

Save generated image to either *pngpath* or to lsystem.png (default)

**-a,--animation [*pngdir*]**

Save all frames of animation to the *pngdir* or ./animation/ (default). The PNG files will be numbered sequentially.

![Sample image of a 14th generation dragon curve](sample.png)
