# Installation

## From package

You can run the same command on a target runing a redpesk OS or in the [SDK container]({% chapter_link sdk-container-doc.overview %}) (development mode).

```bash
dnf install canbus-binding
```

## From sources

When developing inside the SDK container, to install the build dependencies, run the following command:

```bash
dnf builddep canbus-binding
```

Then clone and build from sources.

```bash
git clone https://github.com/redpesk-common/canbus-binding.git
cd canbus-binding
mkdir build
cd build
cmake ..
make -j
```

> Note: To rebuild all (including application framework) from sources, please refer to this [chapter]({% chapter_link host-build-doc.build-framework-on-your-computer %}).
