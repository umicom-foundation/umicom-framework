/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/document_view.h
 *
 * PURPOSE:
 *   Define toolkit-neutral document-tab state including identity, title, dirty
 *   state, activation and pinning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_DOCUMENT_VIEW_H
#define UMICOM_UI_DOCUMENT_VIEW_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_DOCUMENT_VIEW_MAX 256U
#define UMI_UI_DOCUMENT_URI_CAPACITY 1024U
#define UMI_UI_DOCUMENT_CONTENT_CAPACITY 16384U
#define UMI_UI_PRIMARY_EDITOR_GROUP_ID "editor.primary"
#define UMI_UI_SECONDARY_EDITOR_GROUP_ID "editor.secondary"


/**
 * Represent the ui document view snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiDocumentViewSnapshot {
    char view_id[UMI_UI_ID_CAPACITY];
    char document_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    int dirty;
    int active;
    int pinned;
    int32_t order;
    char uri[UMI_UI_DOCUMENT_URI_CAPACITY];
    char language_id[UMI_UI_ID_CAPACITY];
    /*
     * A bounded presentation copy used by simple adapters and welcome pages.
     * Full documents remain owned by the editor text-buffer service.
     */
    char source_text[UMI_UI_DOCUMENT_CONTENT_CAPACITY];
    /* Adapter-independent caret and selection state used by Find and Go To. */
    size_t cursor_offset;
    size_t selection_length;
    /*
     * Advanced editor-workspace presentation. Empty group IDs are treated as
     * the primary editor group so older callers remain source compatible.
     */
    char group_id[UMI_UI_ID_CAPACITY];
    int preview;
    int read_only;
    int closable;
    int word_wrap;
    int show_line_numbers;
} UmiUiDocumentViewSnapshot;

/**
 * Represent the ui document close result data shared with callers of this public contract.
 */
typedef struct UmiUiDocumentCloseResult {
    size_t closed_count;
    size_t dirty_count;
    size_t pinned_count;
    size_t non_closable_count;
} UmiUiDocumentCloseResult;

/**
 * Represent the ui document view model data shared with callers of this public contract.
 */
typedef struct UmiUiDocumentViewModel UmiUiDocumentViewModel;

/**
 * Initialise ui document view model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_document_view_model_create(UmiUiDocumentViewModel **out_model);
/**
 * Release or reset state held by ui document view model so the same storage can be reused
 * safely.
 */
void umi_ui_document_view_model_destroy(UmiUiDocumentViewModel *model);
/**
 * Provide the ui document view model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_view_model_upsert(UmiUiDocumentViewModel *model,
                                     const UmiUiDocumentViewSnapshot *item);
/**
 * Remove ui document view model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_document_view_model_remove(UmiUiDocumentViewModel *model, const char *item_id);
/**
 * Find ui document view model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_document_view_model_find(const UmiUiDocumentViewModel *model, const char *item_id,
                                   UmiUiDocumentViewSnapshot *out_item);
/**
 * Find ui document view model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_document_view_model_at(const UmiUiDocumentViewModel *model, size_t index,
                                 UmiUiDocumentViewSnapshot *out_item);
/**
 * Return the number of records represented by ui document view model without changing
 * their state.
 */
size_t umi_ui_document_view_model_count(const UmiUiDocumentViewModel *model);
/**
 * Provide the ui document view model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_document_view_model_revision(const UmiUiDocumentViewModel *model);

/* Open a replaceable preview without disturbing dirty or pinned editors. */
UmiStatus umi_ui_document_view_model_open_preview(
    UmiUiDocumentViewModel *model,
    const UmiUiDocumentViewSnapshot *item);

/* Professional editor-tab operations shared by every frontend adapter. */
UmiStatus umi_ui_document_view_model_set_pinned(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int pinned);
/**
 * Provide the ui document view model promote preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_promote_preview(
    UmiUiDocumentViewModel *model,
    const char *item_id);
/**
 * Provide the ui document view model set word wrap operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_set_word_wrap(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int word_wrap);
/**
 * Provide the ui document view model move to group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_move_to_group(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    const char *group_id);
/*
 * Place one editor at a zero-based position inside a group.  This single
 * operation is used by command handlers, saved layouts and native tab
 * drag-and-drop so every frontend observes the same authoritative order.
 */
UmiStatus umi_ui_document_view_model_place(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    const char *group_id,
    size_t position);
/**
 * Provide the ui document view model activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_view_model_activate(
    UmiUiDocumentViewModel *model,
    const char *item_id);
/**
 * Provide the ui document view model activate relative operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_document_view_model_activate_relative(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int direction,
    char *out_item_id,
    size_t capacity);
/**
 * Provide the ui document view model activate group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_activate_group(
    UmiUiDocumentViewModel *model,
    const char *group_id,
    char *out_item_id,
    size_t capacity);
/**
 * Provide the ui document view model merge group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_merge_group(
    UmiUiDocumentViewModel *model,
    const char *source_group_id,
    const char *target_group_id);

/* Close operations never discard unsaved or pinned editors. */
UmiStatus umi_ui_document_view_model_close_others(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    UmiUiDocumentCloseResult *out_result);
/**
 * Provide the ui document view model close all operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_close_all(
    UmiUiDocumentViewModel *model,
    UmiUiDocumentCloseResult *out_result);
/**
 * Return the number of records represented by ui document view model group without
 * changing their state.
 */
size_t umi_ui_document_view_model_group_count(
    const UmiUiDocumentViewModel *model,
    const char *group_id);

#ifdef __cplusplus
}
#endif

#endif
