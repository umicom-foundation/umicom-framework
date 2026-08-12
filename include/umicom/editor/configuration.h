/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/configuration.h
 *
 * PURPOSE:
 *   Define reusable per-language and per-workspace editor configuration snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_CONFIGURATION_H
#define UMICOM_EDITOR_CONFIGURATION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CONFIGURATION_CAPACITY 512U
typedef struct UmiEditorConfigurationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char language_id[128];
    uint32_t tab_size;
    int insert_spaces;
    int word_wrap;
    int line_numbers;
    int minimap;
    int auto_indent;
    int format_on_save;
    uint64_t revision;
} UmiEditorConfigurationSnapshot;
typedef struct UmiEditorConfigurationRegistry UmiEditorConfigurationRegistry;
UmiStatus umi_editor_configuration_registry_create(UmiEditorConfigurationRegistry **out_registry);
void umi_editor_configuration_registry_destroy(UmiEditorConfigurationRegistry *registry);
UmiStatus umi_editor_configuration_registry_upsert(UmiEditorConfigurationRegistry *registry,const UmiEditorConfigurationSnapshot *item);
UmiStatus umi_editor_configuration_registry_remove(UmiEditorConfigurationRegistry *registry,const char *id);
UmiStatus umi_editor_configuration_registry_find(const UmiEditorConfigurationRegistry *registry,const char *id,UmiEditorConfigurationSnapshot *out_item);
UmiStatus umi_editor_configuration_registry_at(const UmiEditorConfigurationRegistry *registry,size_t index,UmiEditorConfigurationSnapshot *out_item);
size_t umi_editor_configuration_registry_count(const UmiEditorConfigurationRegistry *registry);
uint64_t umi_editor_configuration_registry_revision(const UmiEditorConfigurationRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
