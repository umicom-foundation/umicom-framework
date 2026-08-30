/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/edit_adapter.h
 *
 * PURPOSE:
 *   Define the minimal editor mutation boundary used to accept an inline AI
 *   suggestion after document-revision conflict checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_EDIT_ADAPTER_H
#define UMICOM_IDE_INTEGRATION_EDIT_ADAPTER_H
#include "umicom/ide_integration/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiIdeEditorGetRevision)(
    void *user_data,
    const char *document_id,
    uint64_t *out_revision);

typedef UmiStatus (*UmiIdeEditorInsertText)(
    void *user_data,
    const char *document_id,
    uint32_t line,
    uint32_t column,
    const char *text,
    size_t length);

typedef struct UmiIdeEditorEditAdapter {
    UmiIdeEditorGetRevision get_revision;
    UmiIdeEditorInsertText insert_text;
    void *user_data;
} UmiIdeEditorEditAdapter;

UmiStatus umi_ide_editor_edit_adapter_validate(
    const UmiIdeEditorEditAdapter *adapter);

#ifdef __cplusplus
}
#endif
#endif
