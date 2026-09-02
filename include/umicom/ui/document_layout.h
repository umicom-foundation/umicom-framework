/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/document_layout.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral snapshot and persistence format for ordered
 *   editor tabs distributed across workbench editor groups.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A layout remembers where an existing document belongs; it never owns or
 * serialises the document text.  Products remain free to restore documents
 * from their normal document and recovery services before applying a layout.
 */

#ifndef UMICOM_UI_DOCUMENT_LAYOUT_H
#define UMICOM_UI_DOCUMENT_LAYOUT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ui/document_view.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_DOCUMENT_LAYOUT_ENTRY_MAX UMI_UI_DOCUMENT_VIEW_MAX
#define UMI_UI_DOCUMENT_LAYOUT_ENTRY_TEXT_CAPACITY 256U

/**
 * Represent the ui document layout entry data shared with callers of this public contract.
 */
typedef struct UmiUiDocumentLayoutEntry {
    char view_id[UMI_UI_ID_CAPACITY];
    char group_id[UMI_UI_ID_CAPACITY];
    int32_t position;
    int active;
} UmiUiDocumentLayoutEntry;

/**
 * Represent the ui document layout snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiDocumentLayoutSnapshot {
    UmiUiDocumentLayoutEntry entries[UMI_UI_DOCUMENT_LAYOUT_ENTRY_MAX];
    size_t count;
    uint64_t revision;
} UmiUiDocumentLayoutSnapshot;

/**
 * Represent the ui document layout apply result data shared with callers of this public
 * contract.
 */
typedef struct UmiUiDocumentLayoutApplyResult {
    size_t applied_count;
    size_t missing_count;
} UmiUiDocumentLayoutApplyResult;

/**
 * Initialise ui document layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_document_layout_init(UmiUiDocumentLayoutSnapshot *layout);
/**
 * Provide the ui document layout capture operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_layout_capture(
    const UmiUiDocumentViewModel *documents,
    UmiUiDocumentLayoutSnapshot *out_layout);
/**
 * Perform ui document layout through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ui_document_layout_apply(
    UmiUiDocumentViewModel *documents,
    const UmiUiDocumentLayoutSnapshot *layout,
    UmiUiDocumentLayoutApplyResult *out_result);
/**
 * Write ui document layout entry in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ui_document_layout_entry_encode(
    const UmiUiDocumentLayoutEntry *entry,
    char *out_text,
    size_t capacity);
/**
 * Read ui document layout entry into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ui_document_layout_entry_decode(
    const char *text,
    UmiUiDocumentLayoutEntry *out_entry);

#ifdef __cplusplus
}
#endif

#endif
