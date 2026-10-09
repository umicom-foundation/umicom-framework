/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/native_arguments.h
 * PURPOSE: Give native applications owned Unicode command arguments without changing global process
 * state. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_NATIVE_ARGUMENTS_H
#define UMICOM_PLATFORM_NATIVE_ARGUMENTS_H
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define UMI_NATIVE_ARGUMENT_COUNT_LIMIT 4096U
#define UMI_NATIVE_ARGUMENT_BYTE_LIMIT (4U * 1024U * 1024U)

    /** Own argument bytes separately from the writable argv pointer array. */
    typedef struct UmiNativeArguments UmiNativeArguments;
    /** Receive borrowed writable argument storage for the duration of this callback. */
    typedef int (*UmiNativeMainFunction)(int argc, char **argv);

    /**
     * Copy a complete vector, including empty strings, without quoting or shell expansion.
     * Strings must be terminated and accessible for their full length. A zero count accepts NULL.
     * Raw POSIX bytes are preserved. Failure leaves out_arguments unchanged.
     */
    UmiStatus UmiNativeArgumentsCopy(int argc, const char *const *argv,
                                     UmiNativeArguments **out_arguments);

    /**
     * Capture the original process arguments. Windows reads the Unicode process command line
     * and strictly converts it to UTF-8 using CommandLineToArgvW rules. Other hosts copy argc/argv.
     * Call at the process entry boundary, not to capture a sliced or synthetic argument vector.
     * This does not change locale, console encoding, working directory or environment.
     */
    UmiStatus UmiNativeArgumentsCapture(int argc, char *const *argv,
                                        UmiNativeArguments **out_arguments);

    /** Return the captured count. A null owner has zero arguments. */
    int UmiNativeArgumentsCount(const UmiNativeArguments *arguments);

    /**
     * Borrow a writable, NULL-terminated argv array. Callers may reorder pointers and edit bytes
     * within their existing lengths, but must not free them or retain them after Destroy.
     */
    char **UmiNativeArgumentsValues(UmiNativeArguments *arguments);

    /** Release the original allocations even if the borrowed pointer array was reordered. */
    void UmiNativeArgumentsDestroy(UmiNativeArguments *arguments);

    /**
     * Capture, invoke a native entry callback once, and release storage when it returns.
     * Publish the callback's exact exit code only on success; argument errors preserve
     * out_exit_code. Applications choose their own error presentation. Callbacks must return
     * normally for cleanup.
     */
    UmiStatus UmiNativeArgumentsDispatch(int argc, char **argv, UmiNativeMainFunction function,
                                         int *out_exit_code);
#ifdef __cplusplus
}
#endif
#endif
