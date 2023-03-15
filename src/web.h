#ifdef PLATFORM_WEB
    #ifndef PLATFORM_WEB_H
    #define PLATFORM_WEB_H

#include <raylib.h>
#include <stdint.h>

bool saveGameToIndexedDB(const uint8_t* data, uint32_t size);
uint8_t* loadGameFromIndexedDB(uint32_t* bytes_read);

    #endif
#endif
