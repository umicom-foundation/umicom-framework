/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_arguments/test_copy.c
 * PURPOSE: Verify exact argument ownership, parser mutation and explicit resource bounds.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "Line %d: %s\n", __LINE__, #value);                                    \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiNativeArguments *arguments = NULL;
    if (strcmp(mode, "copy") == 0 || strcmp(mode, "permutation") == 0)
    {
        char first[] = "application", second[] = "café with spaces", third[] = "";
        const char *values[] = {first, second, third};
        CHECK(UmiNativeArgumentsCopy(3, values, &arguments) == UMI_STATUS_OK);
        memset(second, 'x', sizeof(second) - 1U);
        CHECK(UmiNativeArgumentsCount(arguments) == 3);
        char **copied = UmiNativeArgumentsValues(arguments);
        CHECK(strcmp(copied[1], "café with spaces") == 0 && copied[2][0] == '\0' &&
              copied[3] == NULL);
        if (strcmp(mode, "permutation") == 0)
        {
            char *temporary = copied[0];
            copied[0] = copied[2];
            copied[2] = temporary;
            copied[1] =
                first; /* Owner destruction must never free this caller-owned stack string. */
        }
    }
    else if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiNativeArgumentsCopy(0, NULL, &arguments) == UMI_STATUS_OK);
        CHECK(UmiNativeArgumentsCount(arguments) == 0 &&
              UmiNativeArgumentsValues(arguments)[0] == NULL);
    }
    else if (strcmp(mode, "raw-bytes") == 0)
    {
        const char bytes[] = {'x', (char)0xff, '\0'};
        const char *values[] = {bytes};
        CHECK(UmiNativeArgumentsCopy(1, values, &arguments) == UMI_STATUS_OK);
        CHECK(memcmp(UmiNativeArgumentsValues(arguments)[0], bytes, sizeof(bytes)) == 0);
    }
    else
    {
        CHECK(UmiNativeArgumentsCopy(0, NULL, &arguments) == UMI_STATUS_OK);
        UmiNativeArguments *sentinel = arguments;
        if (strcmp(mode, "invalid") == 0)
        {
            const char *values[] = {NULL};
            CHECK(UmiNativeArgumentsCopy(-1, NULL, &arguments) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiNativeArgumentsCopy(1, NULL, &arguments) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiNativeArgumentsCopy(1, values, &arguments) == UMI_STATUS_INVALID_ARGUMENT);
        }
        else if (strcmp(mode, "count-boundary") == 0)
        {
            const char **values = calloc(UMI_NATIVE_ARGUMENT_COUNT_LIMIT + 1U, sizeof(*values));
            CHECK(values != NULL);
            for (unsigned index = 0U; index <= UMI_NATIVE_ARGUMENT_COUNT_LIMIT; ++index)
                values[index] = "";
            UmiNativeArguments *maximal = NULL;
            CHECK(UmiNativeArgumentsCopy((int)UMI_NATIVE_ARGUMENT_COUNT_LIMIT, values, &maximal) ==
                  UMI_STATUS_OK);
            CHECK(UmiNativeArgumentsCopy((int)UMI_NATIVE_ARGUMENT_COUNT_LIMIT + 1, values,
                                         &arguments) == UMI_STATUS_CAPACITY_EXCEEDED);
            UmiNativeArgumentsDestroy(maximal);
            free(values);
        }
        else if (strcmp(mode, "byte-boundary") == 0)
        {
            char *bytes = malloc(UMI_NATIVE_ARGUMENT_BYTE_LIMIT + 1U);
            CHECK(bytes != NULL);
            memset(bytes, 'x', UMI_NATIVE_ARGUMENT_BYTE_LIMIT);
            bytes[UMI_NATIVE_ARGUMENT_BYTE_LIMIT - 1U] = '\0';
            const char *values[] = {bytes};
            UmiNativeArguments *maximal = NULL;
            CHECK(UmiNativeArgumentsCopy(1, values, &maximal) == UMI_STATUS_OK);
            bytes[UMI_NATIVE_ARGUMENT_BYTE_LIMIT - 1U] = 'x';
            bytes[UMI_NATIVE_ARGUMENT_BYTE_LIMIT] = '\0';
            CHECK(UmiNativeArgumentsCopy(1, values, &arguments) == UMI_STATUS_CAPACITY_EXCEEDED);
            UmiNativeArgumentsDestroy(maximal);
            free(bytes);
        }
        else
            CHECK(0);
        CHECK(arguments == sentinel);
    }
    UmiNativeArgumentsDestroy(arguments);
    UmiNativeArgumentsDestroy(NULL);
    return 0;
}
