#ifndef MEOW_H
#define MEOW_H

#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>

typedef struct keybind {
    unsigned int mod;
    KeySym keysym;
    const char** com;
} keybind;

typedef struct command {
    const char** com;
} command;

#endif
