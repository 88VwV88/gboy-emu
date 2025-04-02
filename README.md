# gboy-emu

A Game Boy Emulator using C/C++, created for education purposes, with cycle accurate emulation of the CPU.

# requirements

### build depenedencies

```bash
sudo apt install gcc-14 libsdl2-dev libsdl2-doc
```

### meson build system

```bash
sudo apt install meson
```

## Build Steps

```bash
CXX=g++-14 meson setup -Db_sanitize=address builddir && cd builddir
meson compile
```
