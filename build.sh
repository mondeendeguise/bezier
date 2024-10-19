#!/usr/bin/env bash

gcc -Wall -Wextra -pedantic -O2 -o bezier src/main.c src/vector.c -lglfw -lGLEW -lGL
