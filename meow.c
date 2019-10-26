/* Meow, an expansion of TinyWM */
#include <X11/Xlib.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "config.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NumLockMask Mod2Mask

Atom wm_index;
Atom wm_name;
Atom wm_pid;


void gen_component_colors(Display *d, Colormap cm, XColor *border, XColor *bg) {
    srand(time(NULL));
    border->red = rand() % 65536;
    border->green = rand() % 65536;
    border->blue = rand() % 65536;
    border->flags = DoRed|DoGreen|DoBlue;
    printf("%i,%i,%i\n", border->red, border->green, border->blue);
    XAllocColor(d, cm, border);

    bg->red = (border->red + 32768)  % 65536;
    bg->green = (border->green + 23768) % 65536;
    bg->blue = (border->blue + 23768) % 65536;
    bg->flags = DoRed|DoGreen|DoBlue;
    XAllocColor(d, cm, bg);
}


int get_wm_index(Display *d, Window window) {
    Atom            dump_atom;
    int             dump_int;
    unsigned long   dump_long;
    unsigned char * win_id;
    XGetWindowProperty(d, window, wm_index, 0, 1, False, XA_INTEGER,
                             &dump_atom, &dump_int, &dump_long, &dump_long, &win_id);
    return win_id[0] | 0;
}


unsigned long get_wm_pid(Display *d, Window window) {
    Atom            dump_atom;
    int             dump_int;
    unsigned long   dump_long;
    unsigned char * win_pid;
    XGetWindowProperty(d, window, wm_pid, 0, 1, False, XA_CARDINAL,
                             &dump_atom, &dump_int, &dump_long, &dump_long, &win_pid);
    return ((unsigned long) win_pid) | 0l;
}


void set_wm_index(Display *d, Window window, int window_index) {
    XChangeProperty(d, window, wm_index,
        XA_INTEGER, 8, PropModeReplace, (unsigned char*)&window_index, 1);
}


void display_workspace(Display *d, Window root, int workspace_index) {
    printf("Setting workspace to %d\n", workspace_index);

    Window dump; Window * w_tree; unsigned int w_count; unsigned int i;
    XQueryTree(d, root, &dump, &dump, &w_tree, &w_count);
    for (i = 0; i < w_count; i++) {
        int index = get_wm_index(d, w_tree[i]);
        if (index != workspace_index && index != 0) {
            XUnmapWindow(d, w_tree[i]);
        } else {
            XMapWindow(d, w_tree[i]);
        }
    }
    if (w_tree) {
        XFree(w_tree);
    }
}


void run_keybind(Display *d, const char **program) {
    if (fork()) return;
    if (d) close(ConnectionNumber(d));

    setsid(); // Don't take children with us when we die
    execvp((char*)program[0], (char**)program);
}


int matches_w_numlock(unsigned int pressed, unsigned int mods) {
    return mods == pressed || (mods|NumLockMask) == pressed;
}

void bind_button(Display *d, Window root, int button, unsigned int mods) {
    XGrabButton(d, button, mods, root, True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(d, button, mods|NumLockMask, root, True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
}


// Bind keyboard with mods, also with num lock
void bind_key(Display *d, Window root, KeySym keysym, unsigned int mods) {
    KeyCode c = XKeysymToKeycode(d, keysym);
    XGrabKey(d, c, mods, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(d, c, mods|NumLockMask, root, True, GrabModeAsync, GrabModeAsync);
}


void ws_add_keybind(Display *d, Window root, int index) {
    bind_key(d, root, XK_0 + index, Mod4Mask);
    bind_key(d, root, XK_0 + index, Mod4Mask|ShiftMask);
}


int xerror() { return 0; }


int main(void) {
    signal(SIGCHLD, SIG_IGN);

    Display          *dpy;
    Window            root;
    int               screen;
    XWindowAttributes attr;

    /* we use this to save the pointer's state at the beginning of the
     * move/resize.
     */
    XButtonEvent start;
    XEvent ev;

    /* return failure status if we can't connect */
    if(!(dpy = XOpenDisplay(NULL))) return 1;

    XSetErrorHandler(xerror);
    screen = DefaultScreen(dpy);
    root  = RootWindow(dpy, screen);

    // Attributes for managing workspaces
    int current_workspace = 1;
    int window_index;
    wm_index = XInternAtom(dpy, "WM_INDEX", False);
    wm_name = XInternAtom(dpy, "WM_NAME", False);
    wm_pid = XInternAtom(dpy, "_NET_WM_PID", True);

    // Window borders
    Colormap screen_colormap = DefaultColormap(dpy, screen);
    XColor border;
    XColor bg;
    gen_component_colors(dpy, screen_colormap, &border, &bg);
    // XAllocNamedColor(dpy, screen_colormap, WIN_BORDER_COLOR, &border, &borde
    XSetWindowBackground(dpy, root, bg.pixel);
    XClearWindow(dpy, root);

    // Hook requests for child windows of the root so we can set entry masks
    XSelectInput(dpy,  root, SubstructureRedirectMask);
    // show a handy little cursor
    XDefineCursor(dpy, root, XCreateFontCursor(dpy, 68));

    // Iterate our config.h keybinds and register for notifications
    for (unsigned int i=0; i < sizeof(keybinds)/sizeof(*keybinds); ++i) {
        bind_key(dpy, root, keybinds[i].keysym, keybinds[i].mod);
    }

    // Grab our workspace index keys, we use MOD+SHIFT for assigning workspace (WS 0 is all)
    bind_key(dpy, root, XK_0, Mod4Mask|ShiftMask);
    for (int i=1; i < 10; i++) {
        ws_add_keybind(dpy, root, i);
    }

    // Set up a key to close windows
    bind_key(dpy, root, XK_q, Mod4Mask);


    /* XGrabKey and XGrabButton are basically ways of saying "when this
     * combination of modifiers and key/button is pressed, send me the events."
     * so we can safely assume that we'll receive Alt+F1 events, Alt+Button1
     * events, and Alt+Button3 events, but no others.  You can either do
     * individual grabs like these for key/mouse combinations, or you can use
     * XSelectInput with KeyPressMask/ButtonPressMask/etc to catch all events
     * of those types and filter them as you receive them.
     */
    bind_button(dpy, root, 1, Mod4Mask);
    bind_button(dpy, root, 3, Mod4Mask);

    start.subwindow = None;

    printf("Init complete, beginning event loop\n");

    for(;;) {
        /* this is the most basic way of looping through X events; you can be
         * more flexible by using XPending(), or ConnectionNumber() along with
         * select() (or poll() or whatever floats your boat).
         */
        XNextEvent(dpy, &ev);

        /* this is our keybinding for raising windows.  as i saw someone
         * mention on the ratpoison wiki, it is pretty stupid; however, i
         * wanted to fit some sort of keyboard binding in here somewhere, and
         * this was the best fit for it.
         *
         * i was a little confused about .window vs. .subwindow for a while,
         * but a little RTFMing took care of that.  our passive grabs above
         * grabbed on the root window, so since we're only interested in events
         * for its child windows, we look at .subwindow.  when subwindow ==
         * None, that means that the window the event happened in was the same
         * window that was grabbed on -- in this case, the root window.
         */
        if(ev.type == ButtonPress && ev.xbutton.subwindow != None) {
            /* we "remember" the position of the pointer at the beginning of
             * our move/resize, and the size/position of the window.  that way,
             * when the pointer moves, we can compare it to our initial data
             * and move/resize accordingly.
             */
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        }
        /* we only get motion events when a button is being pressed,
         * but we still have to check that the drag started on a window */
        else if(ev.type == MotionNotify && start.subwindow != None) {
            // exhaust event queue of duplicates
            while(XCheckTypedEvent(dpy, MotionNotify, &ev));

            /* now we use the stuff we saved at the beginning of the
             * move/resize and compare it to the pointer's current position to
             * determine what the window's new size or position should be.
             *
             * if the initial button press was button 1, then we're moving.
             * otherwise it was 3 and we're resizing.
             *
             * we also make sure not to go negative with the window's
             * dimensions, resulting in "wrapping" which will make our window
             * something ridiculous like 65000 pixels wide (often accompanied
             * by lots of swapping and slowdown).
             *
             * even worse is if we get "lucky" and hit a width or height of
             * exactly zero, triggering an X error.  so we specify a minimum
             * width/height of 1 pixel.
             */
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(dpy, start.subwindow,
                attr.x + (start.button==1 ? xdiff : 0),
                attr.y + (start.button==1 ? ydiff : 0),
                MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
                MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
        }
        else if(ev.type == ButtonRelease) {
            start.subwindow = None;
        } /* FINISH HANDLING DRAG / RESIZE EVENTS */
        else if(ev.type == KeyPress) {
            KeySym keysym = XKeycodeToKeysym(dpy, ev.xkey.keycode, 0);

            // Check workspace key bindings
            switch(keysym) {
                case XK_0:
                    if (matches_w_numlock(ev.xkey.state, Mod4Mask|ShiftMask)) {
                        set_wm_index(dpy, ev.xkey.subwindow, atoi(XKeysymToString(keysym)));
                    }
                    break;
                case XK_1:
                case XK_2:
                case XK_3:
                case XK_4:
                case XK_5:
                case XK_6:
                case XK_7:
                case XK_8:
                case XK_9:
                    if (matches_w_numlock(ev.xkey.state, Mod4Mask|ShiftMask)) {
                        set_wm_index(dpy, ev.xkey.subwindow, atoi(XKeysymToString(keysym)));
                    } else {
                        current_workspace = atoi(XKeysymToString(keysym));
                        display_workspace(dpy, root, atoi(XKeysymToString(keysym)));
                    }
                    break;
                case XK_q:
                    if (ev.xkey.subwindow != None) {
                        // Be polite and try to tell programs to exit
                        int pid = get_wm_pid(dpy, ev.xkey.subwindow);
                        XDestroyWindow(dpy, ev.xkey.subwindow);
                        XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
                        XSync(dpy, False);
                        if (pid != 0) {
                            kill(pid, SIGTERM);
                        } else {
                            // If it's remote or something dumb... death
                            XKillClient(dpy, ev.xkey.subwindow);
                        }
                    }
                    break;

            }

            // Check our list of bind keys from config.h and exec stuff
            for (unsigned int i=0; i < sizeof(keybinds)/sizeof(*keybinds); ++i){
                if (keybinds[i].keysym == keysym &&
                    matches_w_numlock(ev.xkey.state, keybinds[i].mod)) {
                    run_keybind(dpy, keybinds[i].com);
                }
            }
        }
        else if(ev.type == EnterNotify) {
            // Exhaust enter events in case of mass updates
            while(XCheckTypedEvent(dpy, EnterNotify, &ev));

            // Focus windows and raise them when we mouseover
            XSetInputFocus(dpy, ev.xcrossing.window, RevertToParent, CurrentTime);
            XRaiseWindow(dpy, ev.xcrossing.window);
        }
        else if(ev.type == MapRequest) {
            // Set the current workspace as the WM_INDEX prop on the window
            window_index = current_workspace;
            XChangeProperty(dpy, ev.xmaprequest.window, wm_index,
                XA_INTEGER, 8, PropModeReplace, (unsigned char*)&window_index, 1);

            if (WIN_BORDER_WIDTH > 0) {
                XSetWindowBorderWidth(dpy, ev.xmaprequest.window, WIN_BORDER_WIDTH);
                XSetWindowBorder(dpy, ev.xmaprequest.window, border.pixel);
            }

            // Register ourselves for entry events for the window - allows us to surface it on mouseover
            XSelectInput(dpy, ev.xmaprequest.window, EnterWindowMask);
            XMapWindow(dpy, ev.xmaprequest.window);
        } else {
            printf("Unhandled event with type %d\n", ev.type);
        }
    }
}

