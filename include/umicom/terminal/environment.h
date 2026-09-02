/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/environment.h
 *
 * PURPOSE:
 *   Maintain explicit terminal environment overrides without modifying the parent process environment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_ENVIRONMENT_H
#define UMICOM_TERMINAL_ENVIRONMENT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the terminal environment data shared with callers of this public contract.
 */
typedef struct UmiTerminalEnvironment UmiTerminalEnvironment;

/**
 * Initialise terminal environment from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_terminal_environment_create(
    UmiTerminalEnvironment **out_environment
);
/**
 * Release or reset state held by terminal environment so the same storage can be reused
 * safely.
 */
void umi_terminal_environment_destroy(UmiTerminalEnvironment *environment);
/**
 * Copy terminal environment into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_terminal_environment_set(UmiTerminalEnvironment *environment,
                                       const char *name,
                                       const char *value);
/**
 * Remove terminal environment while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_terminal_environment_remove(
    UmiTerminalEnvironment *environment,
    const char *name
);
/**
 * Provide the terminal environment get operation used by this module and its client
 * applications.
 */
const char *umi_terminal_environment_get(
    const UmiTerminalEnvironment *environment,
    const char *name
);
/**
 * Return the number of records represented by terminal environment without changing their
 * state.
 */
size_t umi_terminal_environment_count(
    const UmiTerminalEnvironment *environment
);
/**
 * Provide the terminal environment export operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_environment_export(
    const UmiTerminalEnvironment *environment,
    UmiEnvironmentVariable *out_variables,
    size_t capacity,
    size_t *out_count
);

#ifdef __cplusplus
}
#endif

#endif
