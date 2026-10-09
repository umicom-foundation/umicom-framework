/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_attach/test_process_id.c
 * PURPOSE: Reject ambiguous process identifiers before an attach request can be created.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/native_attach.h"
#include <stdio.h>
#include <string.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    struct Case
    {
        const char *name, *text;
        UmiStatus status;
        uint64_t value;
    } cases[] = {{"ordinary", "12345", UMI_STATUS_OK, 12345U},
                 {"minimum", "1", UMI_STATUS_OK, 1U},
                 {"maximum", "2147483647", UMI_STATUS_OK, INT32_MAX},
                 {"leading-zero", "0000000123", UMI_STATUS_OK, 123U},
                 {"overflow", "2147483648", UMI_STATUS_CAPACITY_EXCEEDED, 0U},
                 {"zero", "0", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"empty", "", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"space", " 12", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"trailing", "12 ", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"sign", "+12", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"negative", "-12", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"hex", "0x12", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"newline", "12\n", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"long", "000000000001", UMI_STATUS_INVALID_ARGUMENT, 0U},
                 {"nul", NULL, UMI_STATUS_INVALID_ARGUMENT, 0U}};
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index)
    {
        if (strcmp(cases[index].name, argv[1]) != 0)
            continue;
        uint64_t value = 99U;
        CHECK(UmiDebugNativeProcessIdRead(cases[index].text, &value) == cases[index].status);
        CHECK(value == (cases[index].status == UMI_STATUS_OK ? cases[index].value : 99U));
        CHECK(UmiDebugNativeProcessIdRead("12", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    return 2;
}
