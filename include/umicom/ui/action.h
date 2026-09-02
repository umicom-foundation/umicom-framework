/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/action.h
 *
 * PURPOSE:
 *   Define toolkit-neutral action metadata that binds visible UI intent to one
 *   canonical Framework command identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ACTION_H
#define UMICOM_UI_ACTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_ACTION_MAX 256U

/* Optional input the frontend should collect before executing an action.
 * The command remains toolkit-neutral; adapters own only native prompting. */
typedef enum UmiUiActionArgumentKind {
    UMI_UI_ACTION_ARGUMENT_NONE = 0,
    UMI_UI_ACTION_ARGUMENT_OPEN_PATH = 1,
    UMI_UI_ACTION_ARGUMENT_SAVE_PATH = 2,
    UMI_UI_ACTION_ARGUMENT_TEXT = 3,
    UMI_UI_ACTION_ARGUMENT_FIND_REPLACE = 4,
    UMI_UI_ACTION_ARGUMENT_LINE_NUMBER = 5
} UmiUiActionArgumentKind;


/**
 * Represent the ui action snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiActionSnapshot {
    char action_id[UMI_UI_ID_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char tooltip[UMI_UI_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    char accelerator[UMI_UI_ACCELERATOR_CAPACITY];
    int enabled;
    int visible;
    int checkable;
    int checked;
    int32_t order;
    /* Optional canonical command argument used by menus, toolbars and keys. */
    char argument[UMI_UI_DESCRIPTION_CAPACITY];
    UmiUiActionArgumentKind argument_kind;
} UmiUiActionSnapshot;

/**
 * Represent the ui action model data shared with callers of this public contract.
 */
typedef struct UmiUiActionModel UmiUiActionModel;

/**
 * Initialise ui action model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_action_model_create(UmiUiActionModel **out_model);
/**
 * Release or reset state held by ui action model so the same storage can be reused safely.
 */
void umi_ui_action_model_destroy(UmiUiActionModel *model);
/**
 * Provide the ui action model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_action_model_upsert(UmiUiActionModel *model,
                                     const UmiUiActionSnapshot *item);
/**
 * Remove ui action model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_action_model_remove(UmiUiActionModel *model, const char *item_id);
/**
 * Find ui action model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_action_model_find(const UmiUiActionModel *model, const char *item_id,
                                   UmiUiActionSnapshot *out_item);
/**
 * Find ui action model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_action_model_at(const UmiUiActionModel *model, size_t index,
                                 UmiUiActionSnapshot *out_item);
/**
 * Return the number of records represented by ui action model without changing their
 * state.
 */
size_t umi_ui_action_model_count(const UmiUiActionModel *model);
/**
 * Provide the ui action model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_action_model_revision(const UmiUiActionModel *model);

#ifdef __cplusplus
}
#endif

#endif
