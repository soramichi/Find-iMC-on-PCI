# Find iMCs (integrated memory controllers) on PCI busses
Usage:
```
$ make skylake
$ ./find_imc
This binary is build exclusively for Skylake.
For other micro architectures, specify -DARCH (e.g. -DSKYLAKE) at the compile time.

node 0
channel 1: /proc/bus/pci/20/0a.2
channel 2: /proc/bus/pci/20/0a.6
channel 3: /proc/bus/pci/20/0b.2
channel 4: /proc/bus/pci/20/0c.2
channel 5: /proc/bus/pci/20/0c.6
channel 6: /proc/bus/pci/20/0d.2
node 1
channel 1: /proc/bus/pci/98/0a.2
channel 2: /proc/bus/pci/98/0a.6
channel 3: /proc/bus/pci/98/0b.2
channel 4: /proc/bus/pci/98/0c.2
channel 5: /proc/bus/pci/98/0c.6
channel 6: /proc/bus/pci/98/0d.2
```

You can then read from/write to the lited files to configure memory controller performance monitoring.

The code is based on a sample provided in Section 1.9 of [Intel Xeon Processor Scalable Memory Family Uncore Performance Monitoring Reference Manual](https://software.intel.com/en-us/download/intel-xeon-processor-scalable-memory-family-uncore-performance-monitoring-reference-manual).
