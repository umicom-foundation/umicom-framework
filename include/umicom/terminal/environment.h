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

typedef struct UmiTerminalEnvironment UmiTerminalEnvironment;

UmiStatus umi_terminal_environment_create(
    UmiTerminalEnvironment **out_environment
);
void umi_terminal_environment_destroy(UmiTerminalEnvironment *environment);
UmiStatus umi_terminal_environment_set(UmiTerminalEnvironment *environment,
                                       const char *name,
                                       const char *value);
UmiStatus umi_terminal_environment_remove(
    UmiTerminalEnvironment *environment,
    const char *name
);
const char *umi_terminal_environment_get(
    const UmiTerminalEnvironment *environment,
    const char *name
);
size_t umi_terminal_environment_count(
    const UmiTerminalEnvironment *environment
);
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
