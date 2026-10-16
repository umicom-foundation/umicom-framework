/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/build_invocation.h
 * PURPOSE: Validate development command options before discovery or mutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOLCHAIN_BUILD_INVOCATION_H
#define UMICOM_TOOLCHAIN_BUILD_INVOCATION_H
#include "umicom/toolchain/build_execution.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Own resolved paths. Non-path option strings and launch arguments are borrowed
 * from argv until execution returns. Do not copy this record after parsing:
 * request/options contain pointers into its owned buffers. */
typedef struct UmiBuildInvocation {
    UmiBuildRequest request;
    UmiBuildExecutionOptions options;
    char source[UMI_PATH_CAPACITY];
    char binary[UMI_PATH_CAPACITY];
    char executable[UMI_PATH_CAPACITY];
    char prefix[UMI_PATH_CAPACITY];
    char log[UMI_PATH_CAPACITY];
    const char *definitions[16];
} UmiBuildInvocation;

/** Parse exact options and resolve relative paths against source (source itself
 * is relative to the supplied absolute cwd). No filesystem or process access.
 * Install/deliver require explicit build and prefix; launch aliases require
 * build, so a preset's custom binaryDir is never guessed. '--' ends CLI options
 * for launch only. Unknown, duplicate or inapplicable options are errors. */
UmiStatus UmiBuildInvocationParse(UmiBuildAction action, int argc,
    const char *const *argv, const char *cwd, UmiBuildInvocation *invocation,
    char *message, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
