Short instructions to build CAN-binder project
=================================

Prerequisites
----------------

Here are the prerequisite to build the project:

- CMake >= 3.3
- g++ >= 4.9
- AGL Application framework binder
- NodeJS >= 6, to build html5 demo UI.

Build instructions
----------------------

You can use the helper script `build.mk` to build the project :

```bash
# Cleaning existing build dir
./build.mk mrproper
# Only build the binding
./build.mk build
# Package the binding and the demo UI. Will need NodeJS >= v6
./build.mk package
```

Or using the CMake way:

```bash
# Get some help to build stuff
git submodule init
git submodule update
# And go to build it
mkdir build
cd build
cmake ..
# Only build
make
# Build and package tree filled
make populate
# Build and generate a Widget file
make widget
```

The project is built in `build` directory, `package` directory will holds the _wgt_ file generated, this is used by Yocto buildsystem to include an AGL application cleanly in an AGL image. `conf.d/packaging` will contains widget configuration file.