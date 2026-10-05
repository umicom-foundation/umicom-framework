/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_secure_random.c
 * PURPOSE: Check random-provider argument and output boundaries without treating samples as an entropy proof.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/secure_random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #c);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1], *cases[] = {"empty", "null", "capacity", "bounded", "single", "maximum"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    unsigned char guard[34];
    memset(guard, 0xa5, sizeof(guard));
    if (strcmp(name, "empty") == 0)
    {
        CHECK(UmiSecureRandomBytes(NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiSecureRandomBytes(guard, 0U) == UMI_STATUS_OK && guard[0] == 0xa5U);
        return 0;
    }
    if (strcmp(name, "null") == 0)
    {
        CHECK(UmiSecureRandomBytes(NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "capacity") == 0)
    {
        CHECK(UmiSecureRandomBytes(guard, UMI_SECURE_RANDOM_MAXIMUM_BYTES + 1U) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              guard[0] == 0xa5U);
        return 0;
    }
    size_t bytes = strcmp(name, "maximum") == 0  ? UMI_SECURE_RANDOM_MAXIMUM_BYTES
                   : strcmp(name, "single") == 0 ? 1U
                                                 : 32U;
    unsigned char *buffer = malloc(bytes + 2U);
    CHECK(buffer != NULL);
    memset(buffer, 0xa5, bytes + 2U);
    UmiStatus status = UmiSecureRandomBytes(buffer + 1U, bytes);
    CHECK(buffer[0] == 0xa5U && buffer[bytes + 1U] == 0xa5U);
    free(buffer);
    if (status == UMI_STATUS_NOT_IMPLEMENTED)
        return 77;
    CHECK(status == UMI_STATUS_OK);
    return 0;
}
