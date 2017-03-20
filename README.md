# Low level CAN signaling binder

Low level CAN bus binder, based upon OpenXC vi-firmware project.

Full document can be found under `docs` directory.

# Generate documentation

## Prerequisites

You can use gitbook to serve documentation. To do so, please install it using *npm* as well as *calibre* package from your package distribution manager:

```bash
sudo npm install -g gitbook-cli
sudo apt-get install calibre
```

To generate Doxygen documentation, you'll need doxygen and graphviz:

```bash
sudo apt-get doxygen graphviz
```

Prepare your build environment:

```bash
mkdir build && cd build
cmake ..
```

## Generation

Generate a PDF version :

```bash
make pdf
```

Serve an HTML version, this will run a web server that will serve you locally documentation :

```bash
make serve
```

Generate doxygen documentation:

```bash
make doxygen
```