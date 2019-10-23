#ifdef __cplusplus
extern "C" {
#endif
#ifndef __SD_LIB_H__
#define __SD_LIB_H__

#include <stddef.h>
#include <stdint.h>

//dateTimeCallback(
//    void (*dateTime)(uint16_t* date, uint16_t* time))

 

extern size_t sdlib_write_to_file(const char *filename, const uint8_t *buf,
                                  size_t size);
extern size_t sdlib_read_from_file(const char *filename, uint8_t *buf,
                                   size_t size);

#endif // __SD_LIB_H__
#ifdef __cplusplus
}
#endif