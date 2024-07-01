## About
Save data from the instrument using VISA library

## Prerequisites

### VISA library

#### MacOS

Install from NI-VISA official site: [https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html#521671](https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html#521671)

#### Ubuntu/Debian

Install from apt:

```
sudo apt install libvisa-dev
```

## Build

```bash
mkdir build
cd build
cmake ..
make
```
