/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/property_inspector.h
 *
 * PURPOSE:
 *   Define a generic property-inspector model reusable by Studio, Designer and domain applications.
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
#ifndef UMICOM_UI_PROPERTY_INSPECTOR_H
#define UMICOM_UI_PROPERTY_INSPECTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PROPERTY_INSPECTOR_CAPACITY 2048U

/**
 * Represent the ui inspector property snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiInspectorPropertySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char object_id[128];
    char category[128];
    char name[128];
    char value[512];
    char value_type[64];
    int editable;
    int required;
    int32_t order;
    uint64_t revision;
} UmiUiInspectorPropertySnapshot;

/**
 * Represent the ui inspector property registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiInspectorPropertyRegistry UmiUiInspectorPropertyRegistry;

/**
 * Initialise ui property inspector registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ui_property_inspector_registry_create(UmiUiInspectorPropertyRegistry **out_registry);
/**
 * Release or reset state held by ui property inspector registry so the same storage can be
 * reused safely.
 */
void umi_ui_property_inspector_registry_destroy(UmiUiInspectorPropertyRegistry *registry);
/**
 * Provide the ui property inspector registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_property_inspector_registry_upsert(UmiUiInspectorPropertyRegistry *registry, const UmiUiInspectorPropertySnapshot *item);
/**
 * Remove ui property inspector registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_property_inspector_registry_remove(UmiUiInspectorPropertyRegistry *registry, const char *id);
/**
 * Find ui property inspector registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_ui_property_inspector_registry_find(const UmiUiInspectorPropertyRegistry *registry, const char *id, UmiUiInspectorPropertySnapshot *out_item);
/**
 * Find ui property inspector registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_ui_property_inspector_registry_at(const UmiUiInspectorPropertyRegistry *registry, size_t index, UmiUiInspectorPropertySnapshot *out_item);
/**
 * Return the number of records represented by ui property inspector registry without
 * changing their state.
 */
size_t umi_ui_property_inspector_registry_count(const UmiUiInspectorPropertyRegistry *registry);
/**
 * Provide the ui property inspector registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_ui_property_inspector_registry_revision(const UmiUiInspectorPropertyRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
