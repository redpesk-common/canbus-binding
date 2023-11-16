# CAN signaling binder

CAN bus binder, based upon OpenXC vi-firmware project.

Full document can be found under `docs` directory.

# Fast build procedure

Just use run :

```bash
mkdir -p ./build
cd ./build
cmake ..
make
make install DESTDIR=${DEST}

```

This will build both projects under build directory for each of them with default configuration.
