/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/watch_edit/test_reply.c
 * PURPOSE: Check malformed, oversized and Unicode watch response values atomically.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/watch_evaluation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiDebugWatchValue value = {{0}, {0}}; strcpy(value.value, "unchanged"); UmiDebugWatchValue before = value;
    const char *json = NULL; UmiStatus expected = UMI_STATUS_PARSE_ERROR; char large[2200];
    if (strcmp(name, "valid") == 0) { json = "{\"body\":{\"result\":\"2\",\"type\":\"int\"}}"; expected = UMI_STATUS_OK; }
    else if (strcmp(name, "empty") == 0) { json = "{\"body\":{\"result\":\"\"}}"; expected = UMI_STATUS_OK; }
    else if (strcmp(name, "unicode") == 0) { json = "{\"body\":{\"result\":\"caf\\u00e9 \\ud83d\\ude80\",\"type\":\"text\"}}"; expected = UMI_STATUS_OK; }
    else if (strcmp(name, "missing") == 0) json = "{\"body\":{\"value\":\"2\"}}";
    else if (strcmp(name, "wrong-result") == 0) json = "{\"body\":{\"result\":2}}";
    else if (strcmp(name, "wrong-type") == 0) json = "{\"body\":{\"result\":\"2\",\"type\":null}}";
    else if (strcmp(name, "wrong-body") == 0) json = "{\"body\":[]}";
    else if (strcmp(name, "duplicate") == 0) json = "{\"body\":{\"result\":\"2\",\"re\\u0073ult\":\"3\"}}";
    else if (strcmp(name, "duplicate-type") == 0) json = "{\"body\":{\"result\":\"2\",\"type\":\"int\",\"type\":\"float\"}}";
    else if (strcmp(name, "duplicate-body") == 0) json = "{\"body\":{\"result\":\"2\"},\"body\":{\"result\":\"3\"}}";
    else if (strcmp(name, "nul") == 0) json = "{\"body\":{\"result\":\"hidden\\u0000suffix\"}}";
    else if (strcmp(name, "surrogate") == 0) json = "{\"body\":{\"result\":\"\\ud800\"}}";
    else if (strcmp(name, "long-result") == 0 || strcmp(name, "long-type") == 0 || strcmp(name, "boundary") == 0) {
        char text[1025]; size_t n = strcmp(name, "long-type") == 0 ? 256U : strcmp(name, "boundary") == 0 ? 1023U : 1024U;
        memset(text, 'x', n); text[n] = '\0';
        (void)snprintf(large, sizeof large, strcmp(name, "long-type") == 0 ?
            "{\"body\":{\"result\":\"2\",\"type\":\"%s\"}}" : "{\"body\":{\"result\":\"%s\"}}", text);
        json = large; expected = strcmp(name, "boundary") == 0 ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    } else return 2;
    CHECK(UmiDebugRuntimeDecodeWatchValue(json, &value) == expected);
    if (expected != UMI_STATUS_OK) CHECK(memcmp(&value, &before, sizeof value) == 0);
    else if (strcmp(name, "valid") == 0) CHECK(strcmp(value.value, "2") == 0 && strcmp(value.type, "int") == 0);
    else if (strcmp(name, "empty") == 0) CHECK(value.value[0] == '\0' && value.type[0] == '\0');
    else if (strcmp(name, "boundary") == 0) CHECK(strlen(value.value) == 1023U);
    else CHECK(strcmp(value.value, "caf\xc3\xa9 \xf0\x9f\x9a\x80") == 0);
    return 0;
}
