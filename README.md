# CAN signaling binder

CAN bus binder, based upon OpenXC vi-firmware project.

There are 2 separated projects here, one with the CAN-binder that is installed
on an AGL target and the CAN config generator used to generate a source file for
the low level binding.

Full document can be found under `docs` directory.

# Fast build procedure

Just use build.sh script:

```bash
./build.sh
```

This will build both projects under build directory for each of them.

# Generate documentation

## Prerequisites

You can use gitbook to serve documentation. To do so, please install it using *npm* as well as *calibre* package from your package distribution manager:

```bash
sudo npm install -g gitbook-cli
sudo apt-get install calibre
```

To generate Doxygen documentation, you'll need doxygen and graphviz:

```bash
sudo apt-get install doxygen graphviz
```

Prepare your build environment:

```bash
mkdir build && cd build
cmake ..
```

## Generation

Generate a PDF version :

```bash
gendocs.sh pdf
```

Serve an HTML version, this will run a web server that will serve you locally documentation :

```bash
gendocs.sh serve
```

Generate doxygen documentation:

```bash
gendocs.sh doxygen
```
or
```bash
cd build && make doxygen
```
