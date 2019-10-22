#include "SD.h"
#include <cstdio>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

extern "C" {
    void sd_demo(void);
}

void sd_demo() {
  if (!card.init()) {
    printf("initialization failed.\r\n");
  } else {
    printf("Wiring is correct and a card is present.");
  }
}