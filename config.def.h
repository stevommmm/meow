#ifndef CONFIG_H
#define CONFIG_H

#include "meow.h"

#define WIN_BORDER_WIDTH 2

const char* menu[] = {"dmenu_run", 0};
const char* term[] = {"gnome-terminal", 0};
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

const char* compton[] = {"compton", "-f", 0};

command startup_cmds[] = {
	{ compton },
};

#endif
