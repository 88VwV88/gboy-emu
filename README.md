# gboy-emu

A Game Boy Emulator using C/C++, created for education purposes, with cycle accurate emulation of the CPU.

# requirements

### gcc-14

```bash
sudo apt install gcc-14
```

### meson build system

```bash
sudo apt install meson
```

## Build Steps

```bash
CXX=g++-14 meson setup -Db_sanitize=address builddir && cd buildir
meson compile
```
