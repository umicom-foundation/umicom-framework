/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/extension_point.h
 *
 * PURPOSE:
 *   Define named UI extension points so products and plug-ins can contribute without private coupling.
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
#ifndef UMICOM_UI_EXTENSION_POINT_H
#define UMICOM_UI_EXTENSION_POINT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_EXTENSION_POINT_CAPACITY 512U

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

typedef struct UmiUiExtensionPointRegistry UmiUiExtensionPointRegistry;

UmiStatus umi_ui_extension_point_registry_create(UmiUiExtensionPointRegistry **out_registry);
void umi_ui_extension_point_registry_destroy(UmiUiExtensionPointRegistry *registry);
UmiStatus umi_ui_extension_point_registry_upsert(UmiUiExtensionPointRegistry *registry, const UmiUiExtensionPointSnapshot *item);
UmiStatus umi_ui_extension_point_registry_remove(UmiUiExtensionPointRegistry *registry, const char *id);
UmiStatus umi_ui_extension_point_registry_find(const UmiUiExtensionPointRegistry *registry, const char *id, UmiUiExtensionPointSnapshot *out_item);
UmiStatus umi_ui_extension_point_registry_at(const UmiUiExtensionPointRegistry *registry, size_t index, UmiUiExtensionPointSnapshot *out_item);
size_t umi_ui_extension_point_registry_count(const UmiUiExtensionPointRegistry *registry);
uint64_t umi_ui_extension_point_registry_revision(const UmiUiExtensionPointRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
