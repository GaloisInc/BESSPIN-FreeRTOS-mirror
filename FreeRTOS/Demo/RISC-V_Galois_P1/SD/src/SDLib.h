#ifdef __cplusplus
extern "C" {
#endif
#ifndef __SD_LIB_H__
#define __SD_LIB_H__

#include <stddef.h>
#include <stdint.h>

/**
 * Initialize SD card
 */
extern int sdlib_initialize(void);

/**
 * Open or create file specified by `filename` (O_READ | O_WRITE | O_CREAT | O_APPEND)
 * and write `buf` into it.
 * Sync and close afterwards.
 * Return number of bytes written, or 0 if an error occured
 */
extern size_t sdlib_write_to_file(const char *filename, const uint8_t *buf,
                                  size_t size);

/**
 * Read from a file specified by `filename` (O_READ)
 * Return 0 if no bytes were read.
 */
extern size_t sdlib_read_from_file(const char *filename, uint8_t *buf,
                                   size_t size);

#endif // __SD_LIB_H__
#ifdef __cplusplus
}
#endif