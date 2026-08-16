/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_action_provider.h
 *
 * PURPOSE:
 *   Define provider capabilities for Framework-owned quick fixes, source
 *   actions and refactorings without coupling applications to language tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_CODE_ACTION_PROVIDER_H
#define UMICOM_EDITOR_CODE_ACTION_PROVIDER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_CODE_ACTION_PROVIDER_API_VERSION 1U
#define UMI_EDITOR_CODE_ACTION_PROVIDER_ID_CAPACITY 128U
#define UMI_EDITOR_CODE_ACTION_PROVIDER_LABEL_CAPACITY 256U
#define UMI_EDITOR_CODE_ACTION_PROVIDER_LANGUAGE_CAPACITY 64U

typedef uint32_t UmiEditorCodeActionKindMask;

enum {
    UMI_EDITOR_CODE_ACTION_KIND_QUICK_FIX = 1U << 0,
    UMI_EDITOR_CODE_ACTION_KIND_REFACTOR = 1U << 1,
    UMI_EDITOR_CODE_ACTION_KIND_REFACTOR_EXTRACT = 1U << 2,
    UMI_EDITOR_CODE_ACTION_KIND_REFACTOR_INLINE = 1U << 3,
    UMI_EDITOR_CODE_ACTION_KIND_REFACTOR_MOVE = 1U << 4,
    UMI_EDITOR_CODE_ACTION_KIND_REWRITE = 1U << 5,
    UMI_EDITOR_CODE_ACTION_KIND_SOURCE = 1U << 6,
    UMI_EDITOR_CODE_ACTION_KIND_ORGANISE_IMPORTS = 1U << 7,
    UMI_EDITOR_CODE_ACTION_KIND_FIX_ALL = 1U << 8,
    UMI_EDITOR_CODE_ACTION_KIND_ALL = (1U << 9) - 1U
};

typedef struct UmiEditorCodeActionProviderItem {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_CODE_ACTION_PROVIDER_ID_CAPACITY];
    char label[UMI_EDITOR_CODE_ACTION_PROVIDER_LABEL_CAPACITY];
    char language_id[UMI_EDITOR_CODE_ACTION_PROVIDER_LANGUAGE_CAPACITY];
    UmiEditorCodeActionKindMask kind_mask;
    int32_t priority;
    int supports_diagnostics;
    int supports_selection;
    int supports_resolution;
    int supports_workspace_edits;
    int supports_preview;
    int enabled;
} UmiEditorCodeActionProviderItem;

typedef struct UmiEditorCodeActionProviderSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t provider_count;
    size_t enabled_provider_count;
    size_t language_count;
    UmiEditorCodeActionKindMask provided_kind_mask;
    uint64_t revision;
} UmiEditorCodeActionProviderSnapshot;

typedef struct UmiEditorCodeActionProviderRegistry
    UmiEditorCodeActionProviderRegistry;

UmiStatus umi_editor_code_action_provider_registry_create(
    UmiEditorCodeActionProviderRegistry **out_registry);
void umi_editor_code_action_provider_registry_destroy(
    UmiEditorCodeActionProviderRegistry *registry);
UmiStatus umi_editor_code_action_provider_registry_clear(
    UmiEditorCodeActionProviderRegistry *registry);
UmiStatus umi_editor_code_action_provider_registry_upsert(
    UmiEditorCodeActionProviderRegistry *registry,
    const UmiEditorCodeActionProviderItem *provider);
UmiStatus umi_editor_code_action_provider_registry_remove(
    UmiEditorCodeActionProviderRegistry *registry,
    const char *provider_id);
UmiStatus umi_editor_code_action_provider_registry_find(
    const UmiEditorCodeActionProviderRegistry *registry,
    const char *provider_id,
    UmiEditorCodeActionProviderItem *out_provider);
UmiStatus umi_editor_code_action_provider_registry_at(
    const UmiEditorCodeActionProviderRegistry *registry,
    size_t index,
    UmiEditorCodeActionProviderItem *out_provider);
UmiStatus umi_editor_code_action_provider_registry_snapshot(
    const UmiEditorCodeActionProviderRegistry *registry,
    UmiEditorCodeActionProviderSnapshot *out_snapshot);
int umi_editor_code_action_provider_supports(
    const UmiEditorCodeActionProviderItem *provider,
    const char *language_id,
    UmiEditorCodeActionKindMask kind_mask);
size_t umi_editor_code_action_provider_registry_count(
    const UmiEditorCodeActionProviderRegistry *registry);
uint64_t umi_editor_code_action_provider_registry_revision(
    const UmiEditorCodeActionProviderRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_CODE_ACTION_PROVIDER_H */
