/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_action.h
 *
 * PURPOSE:
 *   Define editor code actions that execute through canonical command IDs.
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
#ifndef UMICOM_EDITOR_CODE_ACTION_H
#define UMICOM_EDITOR_CODE_ACTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CODE_ACTION_CAPACITY 4096U
/**
 * Represent the editor code action snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCodeActionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char title[512];
    char kind[128];
    char command_id[128];
    char argument[1024];
    int preferred;
    int enabled;
    uint64_t revision;
} UmiEditorCodeActionSnapshot;
/**
 * Represent the editor code action registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCodeActionRegistry UmiEditorCodeActionRegistry;
/**
 * Initialise editor code action registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_code_action_registry_create(UmiEditorCodeActionRegistry **out_registry);
/**
 * Release or reset state held by editor code action registry so the same storage can be
 * reused safely.
 */
void umi_editor_code_action_registry_destroy(UmiEditorCodeActionRegistry *registry);
/**
 * Provide the editor code action registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_code_action_registry_upsert(UmiEditorCodeActionRegistry *registry,const UmiEditorCodeActionSnapshot *item);
/**
 * Remove editor code action registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_code_action_registry_remove(UmiEditorCodeActionRegistry *registry,const char *id);
/**
 * Find editor code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_code_action_registry_find(const UmiEditorCodeActionRegistry *registry,const char *id,UmiEditorCodeActionSnapshot *out_item);
/**
 * Find editor code action registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_code_action_registry_at(const UmiEditorCodeActionRegistry *registry,size_t index,UmiEditorCodeActionSnapshot *out_item);
/**
 * Return the number of records represented by editor code action registry without changing
 * their state.
 */
size_t umi_editor_code_action_registry_count(const UmiEditorCodeActionRegistry *registry);
/**
 * Provide the editor code action registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_code_action_registry_revision(const UmiEditorCodeActionRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
