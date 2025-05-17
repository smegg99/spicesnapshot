# SPICE Snapshot

**SPICE Snapshot** is a command-line utility for capturing a single display frame from a SPICE server (commonly used with QEMU/KVM).  
It supports optional password authentication, configurable frame rate/count, and saves the frame(s) as PNG files.

Inspired by `vncsnapshot` for VNC, this tool helps automate auditing or monitoring of virtual machines using the SPICE protocol.  
Originally developed for use with [ThugHunter](https://github.com/smegg99/ThugHunter), which also utilizes `vncsnapshot`.

## Features

- SPICE protocol support  
- Optional password authentication  
- Configurable FPS & multi-frame capture  
- Saves output as `.png`  

## Installation

### Debian / Ubuntu

Install the build and runtime dependencies:

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    pkg-config \
    git \
    libspice-client-glib-2.0-dev \
    libspice-client-gtk-3.0-dev \
    libgtk-3-dev \
    libcairo2-dev \
    libgdk-pixbuf2.0-dev
```

### Endeavour / Arch Linux

On Arch-based systems (including Endeavour), you can install them via pacman:

```bash
sudo pacman -Syu --needed \
    base-devel \
    git \
    pkgconf \
    spice-gtk \
    gtk3 \
    cairo \
    gdk-pixbuf2
```

Then clone, build & install:

```bash
Copy
Edit
git clone https://github.com/smegg99/spicesnapshot.git
cd spicesnapshot
make
sudo make install
```

### Usage

```bash
spicesnapshot version 1.0
Usage: ./spicesnapshot [OPTIONS] <IP> <PORT> <OUTPUT.png>
Options:
  -p, --password-file <file>  read VNC password from file
  -f, --fps <FPS>             frames per second (default 1)
  -c, --count <COUNT>         number of snapshots (default 1)
  -v, --verbose               print detailed output
  -q, --quiet                 suppress all output except errors
  -h, --help                  this help message
```

Example:

Grab 5 snapshots at 2 fps, reading password from pass.txt

```bash
spicesnapshot -p pass.txt -f 2 -c 5 192.168.122.10 5900 snap.png
```
