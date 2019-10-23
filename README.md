`meow` is an expansion of [TinyWM](https://github.com/mackstann/tinywm) that adds workspaces, keybinds and some misc ease of life functionality.


Usage:
  - Mouse hover window to raise and set focus
  - `Mod` + `Enter`: New terminal
  - `Mod` + `d`: dmenu_run
  - `Mod` + `q`: Kill application
  - `Mod` + `Button1`, drag: interactive window move
  - `Mod` + `Button3`, drag: interactive window resize
  - `Mod` + `Shift` + `0`: Assign window to all workspaces
  - `Mod` + `1`: Switch to workspace 1
  - `Mod` + `2`: Switch to workspace 2
  - `Mod` + `Shift` + `1`: Assign window to workspace 1
  - `Mod` + `Shift` + `2`: Assign window to workspace 2


Development:

    Xephyr :2 &
    p=$!
    make
    DISPLAY=:2 ./meow
    kill $p
