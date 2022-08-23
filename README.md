# Hyperscape

## Checking out the project

- Run `git clone --recurse-submodules https://github.com/edgerunnerdev/Hyperscape.git`

## Building on Windows

- Install Visual Studio 2022 (you can use the [Community edition](https://visualstudio.microsoft.com/vs/community/)).
- From the command line, run:
	`cd vcpkg`
	`bootstrap-vcpkg.bat`
	`vcpkg integrate install`
- Install [Git Large File System](https://git-lfs.github.com).

## Building on Linux

- From the command line, run:
	`cd vcpkg`
	`./bootstrap-vcpkg.sh`
	`./vcpkg integrate install`
	`./vcpkg install --triplet x64-linux`
	
You'll need to install the following packages:
sudo apt install clang cmake libsdl2-dev libsdl2-image-dev libbullet-dev libglew-dev libvpx-dev libsdl2-mixer-dev