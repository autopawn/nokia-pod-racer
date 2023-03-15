#ifdef PLATFORM_WEB
// ^ IMPORTANT check
#include "web.h"
#include <assert.h>
#include <emscripten/emscripten.h>
#include <raylib.h>
#include <stdint.h>
#include <string.h>

bool saveGameToIndexedDB(const uint8_t* data, uint32_t size)
{
    bool lock = true;
    bool success = false;

    EM_ASM({
        const mountPoint = "/nokia-pod-racer";
        const savegameName = "nokia-pod-racer/savegame.dat";

        if (!FS.analyzePath(mountPoint).exists){
            FS.mkdir(mountPoint);
        }
        FS.mount(IDBFS, {}, mountPoint);

        let data = Module.HEAPU8.subarray($2, $2 + $3);
        let file = FS.open(savegameName, 'w+');
        FS.write(file, data, 0, data.length, 0);
        FS.close(file);

        FS.syncfs(function (e) {

            if(!e) {
                Module.setValue($1, true, "i8"); // success -> success
            } else {
                console.log("Fatal error saving to indexedDB.");
                console.log(e);
            }
            FS.unmount(mountPoint);
            Module.setValue($0, false, "i8"); // lock -> false
        });
    }, &lock, &success, data, size);

    /**
     * Waits for Async Javascript to end
     * REQUIRES linking with -sASYNCIFY **/
    while (lock) {
        emscripten_sleep(1); // <- NOTE: Necessary to be detected by ASYNCIFY compiler
    }
    /*************************************/

    return success;
}

uint8_t* loadGameFromIndexedDB(uint32_t* bytes_read)
{
    bool lock = true;
    uint8_t* data = NULL;
    int32_t data_length = 0;

    EM_ASM({
        const mountPoint = "/nokia-pod-racer";
        const savegameName = "nokia-pod-racer/savegame.dat";

        if (!FS.analyzePath(mountPoint).exists){
            FS.mkdir(mountPoint);
        }
        FS.mount(IDBFS, {}, mountPoint);

        FS.syncfs(true, function (e) {
            if (!e) {
                if (FS.analyzePath(savegameName).exists) {
                    let data = Uint8Array.from(
                        FS.readFile(savegameName, { encoding: "binary" })
                    );

                    const outPtr = Module._malloc(data.byteLength + 1);
                    Module.writeArrayToMemory(data, outPtr);
                    Module.setValue($1, outPtr, "i8*"); // outPtr -> outPtr
                    Module.setValue($2, data.byteLength, "i32"); // length -> bytesRead
                }
            } else {
                console.log(e);
                console.log("Fatal error loading from indexedDB.");
            }
            FS.unmount(mountPoint);
            Module.setValue($0, false, "i8"); // lock -> false
        });
    }, &lock, &data, &data_length);

    /**
     * Waits for Async Javascript to end
     * REQUIRES linking with -sASYNCIFY **/
    while (lock) {
        emscripten_sleep(1); // <- NOTE: Necessary to be detected by ASYNCIFY compiler
    }
    /*************************************/
    *bytes_read = (uint32_t)data_length;
    return data;
}

#endif
