# HEXTERMINATE

## Building on Windows

- Install Visual Studio 2019 (you can use the [Community edition](https://visualstudio.microsoft.com/vs/community/)).
- Install [Git Large File System](https://git-lfs.github.com).
- Clone Genesis (the engine) and HEXTERMINATE via HTTPS:
    - git clone https://gitlab.com/hostile.node/Genesis.git
    - git clone https://gitlab.com/hostile.node/Hexterminate.git
- In your clone of HEXTERMINATE, run **generate_vs_sln.bat**
- You'll now have a **Hexterminate.sln** which you can use to build the game.

## Building on Linux

Linux
You'll need to install the following packages:
sudo apt install clang cmake libsdl2-dev libsdl2-image-dev libbullet-dev libglew-dev libvpx-dev libsdl2-mixer-dev