#ifndef DISK_H
#define DISK_H

#include "fs/file.h"

typedef unsigned int LOGOS_DISK_TYPE;
#define LOGOS_DISK_TYPE_REAL 0

struct disk {
  // Metadata
  LOGOS_DISK_TYPE type;
  int sector_size;

  // Disk ID
  int id;

  // Actual filesystem struct
  struct filesystem* filesystem;

  // Private data of the filesystem
  void* fs_private;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif
