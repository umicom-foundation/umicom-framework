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

typedef struct UmiDebugRuntimeContractAdapter
    UmiDebugRuntimeContractAdapter;

UmiStatus umi_debug_runtime_contract_adapter_create(
    UmiDebugRuntimeAdapter *adapter,
    const char *descriptor_id,
    const char *label,
    const char *debugger_kind,
    uint64_t capabilities,
    uint32_t timeout_ms,
    UmiDebugRuntimeContractAdapter **out_owner,
    UmiDebugAdapterDescriptor *out_descriptor);

void umi_debug_runtime_contract_adapter_destroy(
    UmiDebugRuntimeContractAdapter *owner);

UmiDebugRuntimeAdapter *umi_debug_runtime_contract_adapter_connection(
    UmiDebugRuntimeContractAdapter *owner);

#ifdef __cplusplus
}
#endif
#endif
