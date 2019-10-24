#ifndef CONFIG_H
#define CONFIG_H

#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>

typedef struct keybind {
    unsigned int mod;
    KeySym keysym;
    const char** com;
} keybind;

// Window border colors
#define WIN_BORDER_WIDTH 1
#define WIN_BORDER_COLOR "blue"

const char* menu[] = {"dmenu_run", 0};
const char* term[] = {"st", 0};
const char* voldown[] = {"amixer", "sset", "Master", "5%-",         0};
const char* volup[]   = {"amixer", "sset", "Master", "5%+",         0};
const char* volmute[] = {"amixer", "sset", "Master", "toggle",      0};

keybind keybinds[] = {
    {Mod4Mask, XK_d, menu},
    {Mod4Mask, XK_Return, term},
    {0, XF86XK_AudioLowerVolume, voldown},
    {0, XF86XK_AudioRaiseVolume, volup},
    {0, XF86XK_AudioMute,        volmute},
};

#endif
