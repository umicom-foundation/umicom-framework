/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_test_output/test_tail.c
 * PURPOSE: Check eviction, binary bytes, counter limits and rejected aliasing at the shared boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/output_tail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char bytes[9] = {0};
    UmiOutputTailState state = {0};
    if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, NULL, 0U) == UMI_STATUS_OK);
        CHECK(state.length == 0U && state.total_bytes == 0U && !state.truncated);
    }
    else if (strcmp(mode, "exact") == 0)
    {
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "12345678", 8U) == UMI_STATUS_OK);
        CHECK(state.length == 8U && state.total_bytes == 8U && !state.truncated && bytes[8] == '\0');
    }
    else if (strcmp(mode, "rolling") == 0)
    {
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "123456", 6U) == UMI_STATUS_OK);
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "789AB", 5U) == UMI_STATUS_OK);
        CHECK(strcmp(bytes, "456789AB") == 0 && state.total_bytes == 11U && state.truncated);
    }
    else if (strcmp(mode, "oversized") == 0)
    {
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "0123456789ABCDEF", 16U) == UMI_STATUS_OK);
        CHECK(strcmp(bytes, "89ABCDEF") == 0 && state.total_bytes == 16U && state.truncated);
    }
    else if (strcmp(mode, "binary") == 0)
    {
        const char raw[] = {'x', '\0', 'y', (char)0xff, 'z'};
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, raw, sizeof(raw)) == UMI_STATUS_OK);
        CHECK(state.length == sizeof(raw) && memcmp(bytes, raw, sizeof(raw)) == 0 && bytes[5] == '\0');
    }
    else if (strcmp(mode, "saturated") == 0)
    {
        state.total_bytes = UINT64_MAX - 1U;
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "ab", 2U) == UMI_STATUS_OK);
        CHECK(state.total_bytes == UINT64_MAX && state.counters_saturated && state.length == 2U);
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "c", 1U) == UMI_STATUS_OK);
        CHECK(state.total_bytes == UINT64_MAX && strcmp(bytes, "abc") == 0);
    }
    else if (strcmp(mode, "alias") == 0)
    {
        strcpy(bytes, "abc");
        state.length = 3U;
        state.total_bytes = 3U;
        UmiOutputTailState before = state;
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, bytes + 1U, 2U) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputTailAppend((char *)&state, sizeof(state), &state, "x", 1U) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, (const char *)&state, 1U) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&state, &before, sizeof(state)) == 0 && strcmp(bytes, "abc") == 0);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiOutputTailAppend(NULL, sizeof(bytes), &state, "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputTailAppend(bytes, 1U, &state, "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), NULL, "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        state.length = sizeof(bytes);
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        state.length = 1U;
        CHECK(UmiOutputTailAppend(bytes, sizeof(bytes), &state, "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(bytes[0] == '\0' && state.total_bytes == 0U);
    }
    else
        return 2;
    return 0;
}
