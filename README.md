## About

Save currently displayed data from the instrument into .csv file.

## Prerequisites

Install from NI-VISA official site: [https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html#521671](https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html#521671)

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

Show help:

```c
./main -h
```

Example command:

```
./main -r "USB0::0x1AB1::0x04CE::DS1ZA253502774::INSTR" -c 2,4 -f ff_9mhz
```