#include "SDLib.h"
#include "SD.h"
#include <cstdio>

File myFile;

bool initialized = false;

void sd_initialize(void) {
  printf("Initializing SD card...\r\n");
  if (!SD.begin()) {
    printf("initialization failed!\r\n");
    while (1)
      ;
  }
  printf("initialization done.\r\n");
}

size_t sdlib_write_to_file(const char *filename, const uint8_t *buf,
                           size_t size) {
  if (!initialized) {
    sd_initialize();
  }
  myFile = SD.open(filename, FILE_WRITE);
  size_t r;
  if (myFile) {
    printf("Writing..\r\n");
    r = myFile.write(buf, size);
    // close the file:
    myFile.close();
    printf("done.\r\n");
  } else {
    // if the file didn't open, print an error:
    printf("error opening test.txt\r\n");
    r = 0;
  }
  return r;
}

extern size_t sdlib_read_from_file(const char *filename, uint8_t *buf,
                                   size_t size) {
  if (!initialized) {
    sd_initialize();
  }
  myFile = SD.open(filename);
  size_t r;
  if (myFile) {
    printf("%s", filename);

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
        r++;
      printf("%c", myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    printf("error opening test.txt\r\n");
    r=0;
  }
  printf("\r\nDone!\r\n");
  return r;
}
