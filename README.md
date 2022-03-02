# Lindenmayer System Renderer

[Lindenmayer systems](https://en.wikipedia.org/wiki/L-system) are a method of modelling plants and generating fractals.

This software is meant to facilitate rendering and capturing L-systems. It uses SDL2 & OpenGL to draw the L-system and can capture the resulting image in BMP format.

Included is an example.ini configuration file. An ini must be provided that defines the window parameters, draw settings, and L-system definitions that the software will use. You can also provide an output filepath to which the BMP will be saved. 

### Synopsis

`./lsystem.exe [-c inipath] [-o [bmppath]]`

### Description

**-c,--config *inipath***

Use *inipath* config file instead of example.ini (default)

**-o,--output [*bmppath*]**

Save generated image to either *bmppath* or to lsystem.bmp (default)


