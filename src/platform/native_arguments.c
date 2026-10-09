/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/native_arguments.c
 * PURPOSE: Own native argument vectors independently of frontend parsing and mutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/native_arguments.h"
#include <stdlib.h>
#include <string.h>

struct UmiNativeArguments
{
    int count;
    char **values;
    char *bytes;
};

/* Measure a bounded vector before allocating or publishing any ownership. */
UmiStatus UmiNativeArgumentsCopy(int argc, const char *const *argv,
                                 UmiNativeArguments **out_arguments)
{
    size_t bytes = 0U;
    UmiNativeArguments *arguments;
    if (argc < 0 || out_arguments == NULL || (argc != 0 && argv == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((unsigned)argc > UMI_NATIVE_ARGUMENT_COUNT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (int index = 0; index < argc; ++index)
    {
        size_t length = 0U;
        if (argv[index] == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        while (length < UMI_NATIVE_ARGUMENT_BYTE_LIMIT - bytes && argv[index][length] != '\0')
            ++length;
        if (length == UMI_NATIVE_ARGUMENT_BYTE_LIMIT - bytes)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        bytes += length + 1U;
    }
    arguments = calloc(1U, sizeof(*arguments));
    if (arguments == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    arguments->values = calloc((size_t)argc + 1U, sizeof(*arguments->values));
    arguments->bytes = malloc(bytes != 0U ? bytes : 1U);
    if (arguments->values == NULL || arguments->bytes == NULL)
    {
        UmiNativeArgumentsDestroy(arguments);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    arguments->count = argc;
    size_t offset = 0U;
    for (int index = 0; index < argc; ++index)
    {
        size_t length = strlen(argv[index]) + 1U;
        arguments->values[index] = arguments->bytes + offset;
        memcpy(arguments->values[index], argv[index], length);
        offset += length;
    }
    *out_arguments = arguments;
    return UMI_STATUS_OK;
}

/* The public count remains int-compatible with standard native entry points. */
int UmiNativeArgumentsCount(const UmiNativeArguments *arguments)
{
    return arguments != NULL ? arguments->count : 0;
}

/* Pointer permutation by an option parser does not change the owner's allocation records. */
char **UmiNativeArgumentsValues(UmiNativeArguments *arguments)
{
    return arguments != NULL ? arguments->values : NULL;
}

/* Never free through argv: an application may have reordered or replaced its borrowed pointers. */
void UmiNativeArgumentsDestroy(UmiNativeArguments *arguments)
{
    if (arguments == NULL)
        return;
    free(arguments->bytes);
    free(arguments->values);
    free(arguments);
}

/* Centralise entry-point ownership while leaving product startup and error presentation in the
 * product. */
UmiStatus UmiNativeArgumentsDispatch(int argc, char **argv, UmiNativeMainFunction function,
                                     int *out_exit_code)
{
    UmiNativeArguments *arguments = NULL;
    UmiStatus status;
    int result;
    if (function == NULL || out_exit_code == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiNativeArgumentsCapture(argc, argv, &arguments);
    if (status != UMI_STATUS_OK)
        return status;
    result = function(UmiNativeArgumentsCount(arguments), UmiNativeArgumentsValues(arguments));
    UmiNativeArgumentsDestroy(arguments);
    *out_exit_code = result;
    return UMI_STATUS_OK;
}
