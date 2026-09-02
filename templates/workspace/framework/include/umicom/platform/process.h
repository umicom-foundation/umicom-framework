/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/platform/process.h
 *
 * PURPOSE:
 *   Declare the process contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_PROCESS_H
#define UMICOM_PLATFORM_PROCESS_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Perform process through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_process_run(const char *command, int *exit_code);

#ifdef __cplusplus
}
#endif

#endif
