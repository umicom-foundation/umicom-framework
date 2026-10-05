/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/arguments.h
 * PURPOSE: Share bounded, shell-independent argument lists between build and debugger workflows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BASE_ARGUMENTS_H
#define UMICOM_BASE_ARGUMENTS_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_ARGUMENTS_CAPACITY 32U
#define UMI_ARGUMENT_TEXT_CAPACITY 1024U

/* Values point into this record's own storage. Do not copy these pointers to a
 * longer-lived object; copy the strings or parse into that object's record. */
typedef struct UmiArguments {
    char storage[UMI_ARGUMENTS_CAPACITY][UMI_ARGUMENT_TEXT_CAPACITY];
    const char *values[UMI_ARGUMENTS_CAPACITY];
    size_t count;
} UmiArguments;

/* Spaces separate arguments. Single or double quotes group text, including an
 * empty argument. A backslash escapes quotes, backslashes and whitespace; other
 * backslashes stay literal, allowing ordinary Windows paths. No shell, variable,
 * wildcard or command expansion occurs. Failure clears the output completely.
 * Input must be a terminated string and must not overlap the output record. */
UmiStatus UmiArgumentsParse(const char *text, UmiArguments *out);

/* Encode a vector in the grammar above, including empty values and paths that
 * end in a backslash. On failure a nonempty output buffer is cleared. Input and
 * output storage must not overlap. This is for saved/reviewed argument text,
 * never for constructing a shell command. */
UmiStatus UmiArgumentsFormat(const char *const *values, size_t count,
    char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
