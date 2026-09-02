/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_selection.h
 * PURPOSE: Select the best Framework-owned developer kit for a consumer/target.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_SELECTION_H
#define UMICOM_TOOLCHAIN_KIT_SELECTION_H

#include <stdint.h>

#include "umicom/toolchain/kit_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_KIT_SELECTION_API_VERSION 1U
#define UMI_TOOLCHAIN_KIT_SELECTION_REASON_CAPACITY 256U

/**
 * List the named toolchain kit selection source values accepted by this public contract.
 */
typedef enum UmiToolchainKitSelectionSource {
    UMI_TOOLCHAIN_KIT_SELECTION_NONE = 0,
    UMI_TOOLCHAIN_KIT_SELECTION_PREFERRED = 1,
    UMI_TOOLCHAIN_KIT_SELECTION_BINDING = 2,
    UMI_TOOLCHAIN_KIT_SELECTION_RANKED = 3
} UmiToolchainKitSelectionSource;

/**
 * Represent the toolchain kit selection request data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainKitSelectionRequest {
    uint32_t struct_size;
    uint32_t api_version;
    UmiToolchainKitBindingKind binding_kind;
    const char *consumer_id;
    const char *preferred_kit_id;
    const char *target_triple;
    UmiToolchainKitCapabilityFlags required_capabilities;
    int allow_degraded;
    int allow_cross_compile;
} UmiToolchainKitSelectionRequest;

/**
 * Represent the toolchain kit selection snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainKitSelectionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiToolchainKitSnapshot kit;
    UmiToolchainKitSelectionSource source;
    uint64_t score;
    char reason[UMI_TOOLCHAIN_KIT_SELECTION_REASON_CAPACITY];
} UmiToolchainKitSelectionSnapshot;

/**
 * Initialise toolchain kit selection request from caller-provided values so later
 * operations receive a known state.
 */
void umi_toolchain_kit_selection_request_init(
    UmiToolchainKitSelectionRequest *request);
/**
 * Provide the toolchain kit select operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_select(
    const UmiToolchainKitCatalogue *catalogue,
    const UmiToolchainKitSelectionRequest *request,
    UmiToolchainKitSelectionSnapshot *out_selection);
/**
 * Provide the toolchain kit selection source text operation used by this module and its
 * client applications.
 */
const char *umi_toolchain_kit_selection_source_text(
    UmiToolchainKitSelectionSource source);

#ifdef __cplusplus
}
#endif
#endif
