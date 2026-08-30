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

/* BEGINNER NOTE:
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

typedef struct UmiUiDocumentLayoutEntry {
    char view_id[UMI_UI_ID_CAPACITY];
    char group_id[UMI_UI_ID_CAPACITY];
    int32_t position;
    int active;
} UmiUiDocumentLayoutEntry;

typedef struct UmiUiDocumentLayoutSnapshot {
    UmiUiDocumentLayoutEntry entries[UMI_UI_DOCUMENT_LAYOUT_ENTRY_MAX];
    size_t count;
    uint64_t revision;
} UmiUiDocumentLayoutSnapshot;

typedef struct UmiUiDocumentLayoutApplyResult {
    size_t applied_count;
    size_t missing_count;
} UmiUiDocumentLayoutApplyResult;

void umi_ui_document_layout_init(UmiUiDocumentLayoutSnapshot *layout);
UmiStatus umi_ui_document_layout_capture(
    const UmiUiDocumentViewModel *documents,
    UmiUiDocumentLayoutSnapshot *out_layout);
UmiStatus umi_ui_document_layout_apply(
    UmiUiDocumentViewModel *documents,
    const UmiUiDocumentLayoutSnapshot *layout,
    UmiUiDocumentLayoutApplyResult *out_result);
UmiStatus umi_ui_document_layout_entry_encode(
    const UmiUiDocumentLayoutEntry *entry,
    char *out_text,
    size_t capacity);
UmiStatus umi_ui_document_layout_entry_decode(
    const char *text,
    UmiUiDocumentLayoutEntry *out_entry);

#ifdef __cplusplus
}
#endif

#endif
