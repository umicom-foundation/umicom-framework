/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/contract.h
 *
 * PURPOSE:
 *   Expose explicit Framework version, ABI and module-lifecycle contract data
 *   so applications and tests can detect compatibility drift at runtime and
 *   compile time without duplicating structural assumptions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_CONTRACT_H
#define UMICOM_RUNTIME_CONTRACT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/runtime/capability_registry.h"
#include "umicom/runtime/command_registry.h"
#include "umicom/runtime/inventory.h"
#include "umicom/runtime/module.h"
#include "umicom/runtime/service_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RUNTIME_CONTRACT_API_VERSION 1U

/**
 * Represent the runtime contract snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiRuntimeContractSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiVersion framework_version;
    uint32_t framework_abi_version;
    size_t command_descriptor_size;
    size_t service_descriptor_size;
    size_t capability_descriptor_size;
    size_t module_lifecycle_size;
    size_t module_descriptor_size;
    size_t inventory_snapshot_size;
} UmiRuntimeContractSnapshot;

/**
 * Represent the module contract report data shared with callers of this public contract.
 */
typedef struct UmiModuleContractReport {
    uint32_t structure_size;
    uint32_t api_version;
    int structure_compatible;
    int abi_compatible;
    int identity_valid;
    int kind_valid;
    int lifecycle_balanced;
    UmiStatus status;
} UmiModuleContractReport;

/**
 * Provide the runtime contract snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_runtime_contract_snapshot(UmiRuntimeContractSnapshot *out_snapshot);

/**
 * Provide the runtime contract version at least operation used by this module and its
 * client applications.
 */
int umi_runtime_contract_version_at_least(const UmiVersion *actual,
                                          const UmiVersion *minimum);
/**
 * Provide the runtime contract abi matches operation used by this module and its client
 * applications.
 */
int umi_runtime_contract_abi_matches(uint32_t abi_version);

/**
 * Check that runtime module contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_runtime_module_contract_validate(
    const UmiModuleDescriptor *descriptor,
    UmiModuleContractReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
