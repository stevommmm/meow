#ifndef CONFIG_H
#define CONFIG_H

struct keybind {
    unsigned int mod;
    KeySym keysym;
    const char** com;
};

const char* menu[] = {"dmenu_run", 0};
const char* term[] = {"st", 0};

struct keybind keybinds[] = {
    {Mod4Mask, XK_d, menu},
    {Mod4Mask, XK_Return, term},
};

#endif
