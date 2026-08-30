/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/host_sync.h
 *
 * PURPOSE:
 *   Push the current Framework Studio snapshot through the thin toolkit host
 *   adapter after refresh/command execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_HOST_SYNC_H
#define UMICOM_STUDIO_RUNTIME_HOST_SYNC_H
#include "umicom/studio_runtime/host_adapter.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_host_sync(
    UmiStudioRuntimePlatform *platform,
    const UmiStudioRuntimeHostAdapter *adapter);
#ifdef __cplusplus
}
#endif
#endif
