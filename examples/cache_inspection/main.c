/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/cache_inspection/main.c
 *
 * PURPOSE:
 *   Inspect literal CMake cache metadata through Framework without running CMake or repairing files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cmake_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: cache-inspection inspect|duplicate|prefix|read ABSOLUTE_CACHE_FILE\n");
        return 2;
    }
    UmiCMakeCache *cache = calloc(1U, sizeof *cache);
    if (cache == NULL) return 1;
    const char *text = "CMAKE_C_COMPILER:FILEPATH=/training/tools/cc\nCMAKE_GENERATOR:INTERNAL=Ninja\n";
    UmiStatus expected = UMI_STATUS_OK;
    UmiStatus status;
    if (argc == 3 && !strcmp(argv[1], "read")) status = UmiCMakeCacheRead(argv[2], cache);
    else if (argc == 2) {
        if (!strcmp(argv[1], "duplicate")) {
            text = "CMAKE_C_COMPILER:FILEPATH=/first\nCMAKE_C_COMPILER:FILEPATH=/second\n";
            expected = UMI_STATUS_ALREADY_EXISTS;
        } else if (!strcmp(argv[1], "prefix")) {
            text = "# CMAKE_C_COMPILER:FILEPATH=/training/tools/cc\nCMAKE_C_COMPILER:FILEPATH=/training/tools/cc-extra\n";
        } else if (strcmp(argv[1], "inspect")) { free(cache); return 2; }
        /* The parser borrows these bytes only until it returns. The resulting
         * value record owns its field storage and can outlive the input. */
        status = UmiCMakeCacheParse(text, strlen(text), cache);
    } else { free(cache); return 2; }
    if (status != expected) {
        fprintf(stderr, "Cache inspection: %s\n", umi_status_text(status));
        free(cache); return 1;
    }
    if (status != UMI_STATUS_OK) puts("Ambiguous duplicate refused; no file or previous observation changed.");
    else {
        printf("Compiler entry: %s\n", (cache->present & UMI_CMAKE_CACHE_COMPILER) ? cache->compiler : "<absent>");
        printf("Generator entry: %s\n", (cache->present & UMI_CMAKE_CACHE_GENERATOR) ? cache->generator : "<absent>");
        puts("Metadata only: configure, build, tests and repair were not executed.");
    }
    free(cache);
    return 0;
}
