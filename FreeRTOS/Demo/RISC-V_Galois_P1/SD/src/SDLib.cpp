#include "SDLib.h"
#include "SD.h"
#include <cstdio>

File testfile;

int sdlib_initialize(void) {
  printf("Initializing SD card...\r\n");
  if (!SD.begin()) {
    printf("initialization failed!\r\n");
    return 0;
  }
  printf("initialization done.\r\n");
  return 1;
}

size_t sdlib_write_to_file(const char *filename, const uint8_t *buf,
                           size_t size) {
  size_t r = 0;
  // open the testfile. note that only one testfile can be open at a time,
  // so you have to close this one before opening another.
  testfile = SD.open(filename, FILE_WRITE);

  // if the testfile opened okay, write to it:
  if (testfile) {
    printf("Writing to test.txt...\r\n");
    r = testfile.write((uint8_t *)buf, size);
    // close the testfile:
    testfile.close();
    printf("done.\r\n");
  } else {
    // if the testfile didn't open, print an error:
    printf("error opening test.txt\r\n");
  }
  return r;
}

size_t sdlib_read_from_file(const char *filename, uint8_t *buf, size_t size) {
  size_t r = 0;
  testfile = SD.open(filename);
  if (testfile) {
    printf("%s", filename);
    // read from the file until there's nothing else in it:
    while (testfile.available() && r < size) {
      uint8_t c = testfile.read();
      // printf("%c",c);
      buf[(uint16_t)r] = c;
      r++;
    }
    // close the testfile:
    testfile.close();
  } else {
    // if the testfile didn't open, print an error:
    printf("error opening test.txt\r\n");
  }
  printf("\r\nDone!\r\n");
  return r;
}
