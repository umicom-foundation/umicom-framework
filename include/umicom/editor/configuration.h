/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/configuration.h
 *
 * PURPOSE:
 *   Define reusable per-language and per-workspace editor configuration snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Represent the editor configuration snapshot data shared with callers of this public
 * contract.
 */
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
/**
 * Represent the editor configuration registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorConfigurationRegistry UmiEditorConfigurationRegistry;
/**
 * Initialise editor configuration registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_configuration_registry_create(UmiEditorConfigurationRegistry **out_registry);
/**
 * Release or reset state held by editor configuration registry so the same storage can be
 * reused safely.
 */
void umi_editor_configuration_registry_destroy(UmiEditorConfigurationRegistry *registry);
/**
 * Provide the editor configuration registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_configuration_registry_upsert(UmiEditorConfigurationRegistry *registry,const UmiEditorConfigurationSnapshot *item);
/**
 * Remove editor configuration registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_configuration_registry_remove(UmiEditorConfigurationRegistry *registry,const char *id);
/**
 * Find editor configuration registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_configuration_registry_find(const UmiEditorConfigurationRegistry *registry,const char *id,UmiEditorConfigurationSnapshot *out_item);
/**
 * Find editor configuration registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_configuration_registry_at(const UmiEditorConfigurationRegistry *registry,size_t index,UmiEditorConfigurationSnapshot *out_item);
/**
 * Return the number of records represented by editor configuration registry without
 * changing their state.
 */
size_t umi_editor_configuration_registry_count(const UmiEditorConfigurationRegistry *registry);
/**
 * Provide the editor configuration registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_configuration_registry_revision(const UmiEditorConfigurationRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
