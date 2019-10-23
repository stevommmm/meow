meow is an expansion of [TinyWM](https://github.com/mackstann/tinywm) that adds workspaces and keybinds


Usage:
  - Mouse hover window to raise and set focus
  - `Mod` + `Enter`: New terminal
  - `Mod` + `d`: dmenu_run
  - `Mod` + `Button1`, drag: interactive window move
  - `Mod` + `Button3`, drag: interactive window resize
  - `Mod` + `1`: Switch to workspace 1
  - `Mod` + `2`: Switch to workspace 2
  - `Mod` + `Shift` + `1`: Assign window to workspace 1
  - `Mod` + `Shift` + `2`: Assign window to workspace 2
  - `Mod` + `Shift` + `0`: Assign window to all workspaces


Development:

    Xephyr :2 &
    p=$!
    make
    DISPLAY=:2 ./meow
    kill $p
