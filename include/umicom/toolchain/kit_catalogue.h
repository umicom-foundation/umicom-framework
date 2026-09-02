/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_catalogue.h
 * PURPOSE: Store developer kits and deterministic consumer-to-kit bindings.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_CATALOGUE_H
#define UMICOM_TOOLCHAIN_KIT_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/toolchain/kit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_KIT_CATALOGUE_API_VERSION 1U
#define UMI_TOOLCHAIN_KIT_CATALOGUE_CAPACITY 64U
#define UMI_TOOLCHAIN_KIT_BINDING_CAPACITY 256U
#define UMI_TOOLCHAIN_KIT_CONSUMER_ID_CAPACITY 128U

/**
 * List the named toolchain kit binding kind values accepted by this public contract.
 */
typedef enum UmiToolchainKitBindingKind {
    UMI_TOOLCHAIN_KIT_BINDING_UNKNOWN = 0,
    UMI_TOOLCHAIN_KIT_BINDING_PROJECT = 1,
    UMI_TOOLCHAIN_KIT_BINDING_APPLICATION = 2,
    UMI_TOOLCHAIN_KIT_BINDING_WORKSPACE = 3
} UmiToolchainKitBindingKind;

/**
 * Represent the toolchain kit binding snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainKitBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiToolchainKitBindingKind kind;
    char consumer_id[UMI_TOOLCHAIN_KIT_CONSUMER_ID_CAPACITY];
    char kit_id[UMI_TOOLCHAIN_KIT_ID_CAPACITY];
    uint64_t revision;
} UmiToolchainKitBindingSnapshot;

/**
 * Represent the toolchain kit catalogue snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainKitCatalogueSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t kit_count;
    size_t ready_count;
    size_t degraded_count;
    size_t cross_compile_count;
    size_t binding_count;
    uint64_t revision;
} UmiToolchainKitCatalogueSnapshot;

/**
 * Represent the toolchain kit catalogue data shared with callers of this public contract.
 */
typedef struct UmiToolchainKitCatalogue UmiToolchainKitCatalogue;

/**
 * Initialise toolchain kit catalogue from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_toolchain_kit_catalogue_create(
    UmiToolchainKitCatalogue **out_catalogue);
/**
 * Release or reset state held by toolchain kit catalogue so the same storage can be reused
 * safely.
 */
void umi_toolchain_kit_catalogue_destroy(UmiToolchainKitCatalogue *catalogue);
/**
 * Provide the toolchain kit catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_catalogue_upsert(
    UmiToolchainKitCatalogue *catalogue,
    const UmiToolchainKitSnapshot *kit);
/**
 * Remove toolchain kit catalogue while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_toolchain_kit_catalogue_remove(
    UmiToolchainKitCatalogue *catalogue,
    const char *kit_id);
/**
 * Find toolchain kit catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_toolchain_kit_catalogue_find(
    const UmiToolchainKitCatalogue *catalogue,
    const char *kit_id,
    UmiToolchainKitSnapshot *out_kit);
/**
 * Find toolchain kit catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_toolchain_kit_catalogue_at(
    const UmiToolchainKitCatalogue *catalogue,
    size_t index,
    UmiToolchainKitSnapshot *out_kit);
/**
 * Provide the toolchain kit catalogue bind operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_catalogue_bind(
    UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id,
    const char *kit_id);
/**
 * Provide the toolchain kit catalogue unbind operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_catalogue_unbind(
    UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id);
/**
 * Provide the toolchain kit catalogue resolve binding operation used by this module and
 * its client applications.
 */
UmiStatus umi_toolchain_kit_catalogue_resolve_binding(
    const UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id,
    UmiToolchainKitSnapshot *out_kit);
/**
 * Find toolchain kit catalogue binding while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_toolchain_kit_catalogue_binding_at(
    const UmiToolchainKitCatalogue *catalogue,
    size_t index,
    UmiToolchainKitBindingSnapshot *out_binding);
/**
 * Provide the toolchain kit catalogue snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_catalogue_snapshot(
    const UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitCatalogueSnapshot *out_snapshot);
/**
 * Return the number of records represented by toolchain kit catalogue without changing
 * their state.
 */
size_t umi_toolchain_kit_catalogue_count(
    const UmiToolchainKitCatalogue *catalogue);

#ifdef __cplusplus
}
#endif
#endif
