/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/profile_name/test_profile_name_ffi.c
 *
 * PURPOSE:
 *   Check FFI buffer bounds, overlap, explicit byte lengths and unchanged failure output through the canonical policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/interop/profile_name.h"
#include "../support/assertions.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
int main(int argc, char **argv)
{
    uint8_t input[64], output[80], before[80];
    const uint8_t *name = input;
    uint8_t *destination = output;
    size_t length = 3U, capacity = 49U;
    int32_t expected = 0;
    if (argc != 2) return 2;
    const char *mode = argv[1];
    memset(input, 'A', sizeof(input));
    memset(output, 0xa5, sizeof(output));
    memcpy(before, output, sizeof(output));
    if (!strcmp(mode, "version")) {
        assert(UmiInteropProfileNameVersion() == 1U && UmiInteropProfileNameCapacity() == 49U); return 0;
    } else if (!strcmp(mode, "basic")) { memcpy(input, "SaM_9-", 6U); length = 6U; }
    else if (!strcmp(mode, "exact-span")) input[3] = '!';
    else if (!strcmp(mode, "minimum")) { /* Three letters are the lower bound. */ }
    else if (!strcmp(mode, "maximum")) length = 48U;
    else if (!strcmp(mode, "too-short")) { length = 2U; expected = 1; }
    else if (!strcmp(mode, "too-long")) { length = 49U; expected = 1; }
    else if (!strcmp(mode, "embedded-zero")) { input[1] = 0U; expected = 1; }
    else if (!strcmp(mode, "nonascii")) { input[1] = 0xc3U; input[2] = 0xa9U; expected = 1; }
    else if (!strcmp(mode, "invalid-leading")) { input[0] = '9'; expected = 1; }
    else if (!strcmp(mode, "invalid-character")) { input[2] = '/'; expected = 1; }
    else if (!strcmp(mode, "null-input")) { name = NULL; expected = 1; }
    else if (!strcmp(mode, "null-output")) { destination = NULL; expected = 1; }
    else if (!strcmp(mode, "small-capacity")) { capacity = 48U; expected = 3; }
    else if (!strcmp(mode, "zero-capacity")) { capacity = 0U; expected = 3; }
    else if (!strcmp(mode, "excess-capacity")) capacity = sizeof(output);
    else if (!strcmp(mode, "overlap") || !strcmp(mode, "partial-overlap")) {
        memcpy(output, "SaM_9-", 6U); name = output; length = 6U;
        destination = output + (!strcmp(mode, "partial-overlap") ? 1U : 0U);
        memcpy(before, output, sizeof(output));
    } else if (!strcmp(mode, "readonly-input")) { name = (const uint8_t *)"SaM"; }
    else if (!strcmp(mode, "failure-unchanged")) { input[47] = '!'; length = 48U; expected = 1; }
    else if (!strcmp(mode, "padding")) { /* All unused output bytes must be zero. */ }
    else return 2;
    assert(UmiInteropProfileNameNormalize(name, length, destination, capacity) == expected);
    if (expected != 0) assert(memcmp(output, before, sizeof(output)) == 0);
    else {
        const char *text = !strcmp(mode, "basic") || !strcmp(mode, "overlap") ||
            !strcmp(mode, "partial-overlap") ? "sam_9-" : !strcmp(mode, "readonly-input") ? "sam" : NULL;
        if (text != NULL) assert(!strcmp((const char *)destination, text));
        else for (size_t i = 0U; i < length; ++i) assert(destination[i] == 'a');
        for (size_t i = length; i < 49U; ++i) assert(destination[i] == 0U);
        size_t offset = (size_t)(destination - output);
        for (size_t i = 0U; i < offset; ++i) assert(output[i] == before[i]);
        for (size_t i = offset + 49U; i < sizeof(output); ++i) assert(output[i] == before[i]);
    }
    return 0;
}
