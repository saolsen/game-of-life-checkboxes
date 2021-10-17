#!/bin/bash

emcc \
    -Os \
    --shell-file shell.html \
    -DSOKOL_GLES2 \
    -I../lib \
    ../plant_game.cpp \
    ../lib/imgui.cpp \
    ../lib/imgui_demo.cpp \
    ../lib/imgui_draw.cpp \
    ../lib/imgui_tables.cpp \
    ../lib/imgui_widgets.cpp \
    -o build/index.html