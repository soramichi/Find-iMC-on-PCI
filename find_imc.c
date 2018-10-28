#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DRV_IS_PCI_VENDOR_ID_INTEL  0x8086
#define VENDOR_ID_MASK 0x0000FFFF
#define DEVICE_ID_MASK 0xFFFF0000
#define DEVICE_ID_BITSHIFT 16

// https://pci-ids.ucw.cz/read/PC/8086
// https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/742765
#if defined(SKYLAKE)
const unsigned int IMC_DID[] = {0x2042, 0x2046, 0x204A};
const int n_channels = 6;
const char* arch = "Skylake";

#elif defined(BROADWELL)
const unsigned int IMC_DID[] = {0x6fb0, 0x6fb1, 0x6fb4, 0x6fb5};
const int n_channels = 4;
const char* arch = "Broadwell";

#elif defined(HASWELL)
const unsigned int IMC_DID[] = {0x2fb0, 0x2fb1, 0x2fd0, 0x2fd1};
const int n_channels = 4;
const char* arch = "Haswell";

#elif defined(IVYBRIDGE)
const unsigned int IMC_DID[] = {0x0eb0, 0x0eb1, 0x0eb4, 0x0eb5};
const int n_channels = 4;
const char* arch = "Ivy Bridge";

#else // Error
const unsigned int IMC_DID[] = {};
const int n_channels = 0;
const char* arch = "";
__attribute__((constructor)) static void warn() {
  fprintf(stderr, "You should specify either -DSKYLAKE, -DBROADWELL, -DHASWELL, or -DIVYBRIDGE at the compile time.\n");
  exit(1);
}

#endif

const int n_nodes_max = 8;             // normal ones, do not assume special machines like SGI or Cray

void pci_read(int bus, int device, int function, off_t offset, void* data, size_t size){
  ssize_t ret;
  char device_file_name[64];
  int fd;

  sprintf(device_file_name, "/proc/bus/pci/%02x/%02x.%x", bus, device, function);
  fd = open(device_file_name, O_RDONLY);

  if (fd < 0){
    // even if the device cannot be found, it is not an error
    return;
  }

  ret = pread(fd, data, size, offset);
  if (ret != size) {
    perror("pread(fd, data, size, offset)");
    exit(-1);
  }

  close(fd);
}

unsigned long pci_read_ulong(int bus, int device, int function, int offset){
  unsigned long ret = 0;
  pci_read(bus, device, function, offset, &ret, sizeof(ret));
  return ret;
}

struct iMC{
  int bus_no;
  int device_no;
  int function_no;
};

int main() {
  int bus, device, function;
  int i, j;

  printf("This binary is build exclusively for %s.\n", arch);
  printf("For other micro architectures, specify -DARCH (e.g. -DSKYLAKE) at the compile time.\n\n");
  
  struct iMC** iMCs = malloc(sizeof(struct iMC*) * n_nodes_max);
  for(i=0; i<n_nodes_max; i++) {
    iMCs[i] = malloc(sizeof(struct iMC) * n_channels);
    memset(iMCs[i], 0, sizeof(struct iMC) * n_channels);
  }

  int node = 0, channel = 0;
  for (bus = 0; bus < 256; bus++) {
    for (device = 0; device < 32; device++) {
      for (function = 0; function < 8; function++) {
	unsigned long value = pci_read_ulong(bus, device, function, 0);
	unsigned long vendor_id = value & VENDOR_ID_MASK;
	unsigned long device_id = (value & DEVICE_ID_MASK) >> DEVICE_ID_BITSHIFT;

	if (vendor_id != DRV_IS_PCI_VENDOR_ID_INTEL) {
	  continue;
	}

	for (i=0; i<sizeof(IMC_DID); i++) {
	  if (device_id == IMC_DID[i]) {  // found an iMC
	    struct iMC imc = {.bus_no = bus, .device_no = device, .function_no = function};
	    iMCs[node][channel] = imc;

	    channel++;
	    // We assume that every channel of a node is populated.
	    // Otherwide, how do we know which node a channel belongs??
	    if (channel == n_channels) {
	      node++;
	      channel = 0;
	    }
	  }
	}
      }
    }
  }

  for(i = 0; i<node; i++) {
    printf("node %d\n", i);
    for(j = 0; j<n_channels; j++) {
      printf("channel %d: /proc/bus/pci/%x/%02x.%x\n", j+1, iMCs[i][j].bus_no, iMCs[i][j].device_no, iMCs[i][j].function_no);
    }
  }
}
