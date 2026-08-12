/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/property_inspector.h
 *
 * PURPOSE:
 *   Define a generic property-inspector model reusable by Studio, Designer and domain applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiUiInspectorPropertyRegistry UmiUiInspectorPropertyRegistry;

UmiStatus umi_ui_property_inspector_registry_create(UmiUiInspectorPropertyRegistry **out_registry);
void umi_ui_property_inspector_registry_destroy(UmiUiInspectorPropertyRegistry *registry);
UmiStatus umi_ui_property_inspector_registry_upsert(UmiUiInspectorPropertyRegistry *registry, const UmiUiInspectorPropertySnapshot *item);
UmiStatus umi_ui_property_inspector_registry_remove(UmiUiInspectorPropertyRegistry *registry, const char *id);
UmiStatus umi_ui_property_inspector_registry_find(const UmiUiInspectorPropertyRegistry *registry, const char *id, UmiUiInspectorPropertySnapshot *out_item);
UmiStatus umi_ui_property_inspector_registry_at(const UmiUiInspectorPropertyRegistry *registry, size_t index, UmiUiInspectorPropertySnapshot *out_item);
size_t umi_ui_property_inspector_registry_count(const UmiUiInspectorPropertyRegistry *registry);
uint64_t umi_ui_property_inspector_registry_revision(const UmiUiInspectorPropertyRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
