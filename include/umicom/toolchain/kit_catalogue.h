/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_catalogue.h
 * PURPOSE: Store developer kits and deterministic consumer-to-kit bindings.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiToolchainKitBindingKind {
    UMI_TOOLCHAIN_KIT_BINDING_UNKNOWN = 0,
    UMI_TOOLCHAIN_KIT_BINDING_PROJECT = 1,
    UMI_TOOLCHAIN_KIT_BINDING_APPLICATION = 2,
    UMI_TOOLCHAIN_KIT_BINDING_WORKSPACE = 3
} UmiToolchainKitBindingKind;

typedef struct UmiToolchainKitBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiToolchainKitBindingKind kind;
    char consumer_id[UMI_TOOLCHAIN_KIT_CONSUMER_ID_CAPACITY];
    char kit_id[UMI_TOOLCHAIN_KIT_ID_CAPACITY];
    uint64_t revision;
} UmiToolchainKitBindingSnapshot;

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

typedef struct UmiToolchainKitCatalogue UmiToolchainKitCatalogue;

UmiStatus umi_toolchain_kit_catalogue_create(
    UmiToolchainKitCatalogue **out_catalogue);
void umi_toolchain_kit_catalogue_destroy(UmiToolchainKitCatalogue *catalogue);
UmiStatus umi_toolchain_kit_catalogue_upsert(
    UmiToolchainKitCatalogue *catalogue,
    const UmiToolchainKitSnapshot *kit);
UmiStatus umi_toolchain_kit_catalogue_remove(
    UmiToolchainKitCatalogue *catalogue,
    const char *kit_id);
UmiStatus umi_toolchain_kit_catalogue_find(
    const UmiToolchainKitCatalogue *catalogue,
    const char *kit_id,
    UmiToolchainKitSnapshot *out_kit);
UmiStatus umi_toolchain_kit_catalogue_at(
    const UmiToolchainKitCatalogue *catalogue,
    size_t index,
    UmiToolchainKitSnapshot *out_kit);
UmiStatus umi_toolchain_kit_catalogue_bind(
    UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id,
    const char *kit_id);
UmiStatus umi_toolchain_kit_catalogue_unbind(
    UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id);
UmiStatus umi_toolchain_kit_catalogue_resolve_binding(
    const UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitBindingKind kind,
    const char *consumer_id,
    UmiToolchainKitSnapshot *out_kit);
UmiStatus umi_toolchain_kit_catalogue_binding_at(
    const UmiToolchainKitCatalogue *catalogue,
    size_t index,
    UmiToolchainKitBindingSnapshot *out_binding);
UmiStatus umi_toolchain_kit_catalogue_snapshot(
    const UmiToolchainKitCatalogue *catalogue,
    UmiToolchainKitCatalogueSnapshot *out_snapshot);
size_t umi_toolchain_kit_catalogue_count(
    const UmiToolchainKitCatalogue *catalogue);

#ifdef __cplusplus
}
#endif
#endif
