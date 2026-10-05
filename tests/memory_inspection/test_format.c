/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/memory_inspection/test_format.c
 * PURPOSE: Check report sizing, unsigned bytes and full-capacity formatting without truncation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/debug_runtime/memory_inspection_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    UmiDebugMemoryCapture *capture = calloc(1U, sizeof *capture);
    CHECK(capture != NULL);
    strcpy(capture->result.address, "18446744073709551616");
    capture->requested = 4096U;
    capture->offset = -16;
    capture->result.count = strcmp(name, "empty") == 0 ? 0U : strcmp(name, "maximum") == 0 ? 4096U : 16U;
    for (size_t i = 0U; i < capture->result.count; ++i)
        capture->result.bytes[i] = (unsigned char)(i + 32U);
    size_t needed = 0U;
    CHECK(UmiDebugMemoryCaptureFormat(capture, NULL, 0U, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
    char *text = malloc(needed);
    CHECK(text != NULL);
    memset(text, 0x5A, needed);
    CHECK(UmiDebugMemoryCaptureFormat(capture, text, needed - 1U, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t i = 0U; i < needed; ++i)
        CHECK(text[i] == 'Z');
    CHECK(UmiDebugMemoryCaptureFormat(capture, text, needed, &needed) == UMI_STATUS_OK);
    CHECK(strlen(text) + 1U == needed);
    CHECK(strstr(text, "Address: 18446744073709551616") != NULL);
    if (strcmp(name, "ascii") == 0)
        CHECK(strstr(text, "| !\"#$%&'()*+,-./|") != NULL);
    else if (strcmp(name, "maximum") == 0)
        CHECK(strstr(text, "+0FF0") != NULL && strstr(text, "+1000") == NULL);
    else if (strcmp(name, "empty") == 0)
        CHECK(strstr(text, "end of readable memory") != NULL && strstr(text, "+0000") == NULL);
    else
        CHECK(strcmp(name, "capacity") == 0);
    free(text);
    UmiDebugMemoryCaptureDestroy(capture);
    return 0;
}
