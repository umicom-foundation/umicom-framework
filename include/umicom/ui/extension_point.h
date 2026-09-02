/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/extension_point.h
 *
 * PURPOSE:
 *   Define named UI extension points so products and plug-ins can contribute without private coupling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_UI_EXTENSION_POINT_H
#define UMICOM_UI_EXTENSION_POINT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_EXTENSION_POINT_CAPACITY 512U

/**
 * Represent the ui extension point snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiExtensionPointSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char owner[128];
    char kind[64];
    char location[128];
    char schema_id[128];
    int enabled;
    int multiple;
    int32_t order;
    uint64_t revision;
} UmiUiExtensionPointSnapshot;

/**
 * Represent the ui extension point registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiExtensionPointRegistry UmiUiExtensionPointRegistry;

/**
 * Initialise ui extension point registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_extension_point_registry_create(UmiUiExtensionPointRegistry **out_registry);
/**
 * Release or reset state held by ui extension point registry so the same storage can be
 * reused safely.
 */
void umi_ui_extension_point_registry_destroy(UmiUiExtensionPointRegistry *registry);
/**
 * Provide the ui extension point registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_extension_point_registry_upsert(UmiUiExtensionPointRegistry *registry, const UmiUiExtensionPointSnapshot *item);
/**
 * Remove ui extension point registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_extension_point_registry_remove(UmiUiExtensionPointRegistry *registry, const char *id);
/**
 * Find ui extension point registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_extension_point_registry_find(const UmiUiExtensionPointRegistry *registry, const char *id, UmiUiExtensionPointSnapshot *out_item);
/**
 * Find ui extension point registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_extension_point_registry_at(const UmiUiExtensionPointRegistry *registry, size_t index, UmiUiExtensionPointSnapshot *out_item);
/**
 * Return the number of records represented by ui extension point registry without changing
 * their state.
 */
size_t umi_ui_extension_point_registry_count(const UmiUiExtensionPointRegistry *registry);
/**
 * Provide the ui extension point registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_extension_point_registry_revision(const UmiUiExtensionPointRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
