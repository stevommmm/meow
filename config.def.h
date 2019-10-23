#ifndef CONFIG_H
#define CONFIG_H

// Window border colors
#define WIN_BORDER_WIDTH 1
#define WIN_BORDER_COLOR "blue"

const char* menu[] = {"dmenu_run", 0};
const char* term[] = {"st", 0};

struct keybind keybinds[] = {
    {Mod4Mask, XK_d, menu},
    {Mod4Mask, XK_Return, term},
};

#endif