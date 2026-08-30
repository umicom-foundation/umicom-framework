/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/service_dependency.h
 *
 * PURPOSE:
 *   Evaluate background-service dependency readiness.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_SERVICE_DEPENDENCY_H
#define UMICOM_TERMINAL_REMOTE_SERVICE_DEPENDENCY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
bool umi_terminal_remote_service_dependency_ready(size_t required_count,size_t ready_count,bool optional_failures);
#ifdef __cplusplus
}
#endif
#endif
