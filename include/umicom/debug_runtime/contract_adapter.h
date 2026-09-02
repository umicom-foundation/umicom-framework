/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/contract_adapter.h
 *
 * PURPOSE:
 *   Expose a live DAP connection through the existing stable
 *   UmiDebugAdapterDescriptor C ABI used by UmiDebugAdvancedPlatform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_CONTRACT_ADAPTER_H
#define UMICOM_DEBUG_RUNTIME_CONTRACT_ADAPTER_H

#include "umicom/debug/adapter_contract.h"
#include "umicom/debug_runtime/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug runtime contract adapter data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugRuntimeContractAdapter
    UmiDebugRuntimeContractAdapter;

/**
 * Initialise debug runtime contract adapter from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_runtime_contract_adapter_create(
    UmiDebugRuntimeAdapter *adapter,
    const char *descriptor_id,
    const char *label,
    const char *debugger_kind,
    uint64_t capabilities,
    uint32_t timeout_ms,
    UmiDebugRuntimeContractAdapter **out_owner,
    UmiDebugAdapterDescriptor *out_descriptor);

/**
 * Release or reset state held by debug runtime contract adapter so the same storage can be
 * reused safely.
 */
void umi_debug_runtime_contract_adapter_destroy(
    UmiDebugRuntimeContractAdapter *owner);

/**
 * Provide the debug runtime contract adapter connection operation used by this module and
 * its client applications.
 */
UmiDebugRuntimeAdapter *umi_debug_runtime_contract_adapter_connection(
    UmiDebugRuntimeContractAdapter *owner);

#ifdef __cplusplus
}
#endif
#endif
