#!/bin/bash

emcc \
    -s USE_WEBGL2=1 \
    -Os \
    --shell-file shell.html \
    -DSOKOL_GLES3 \
    -I../lib \
    ../plant_game.cpp \
    ../lib/imgui.cpp \
    ../lib/imgui_demo.cpp \
    ../lib/imgui_draw.cpp \
    ../lib/imgui_tables.cpp \
    ../lib/imgui_widgets.cpp \
    -o build/index.html