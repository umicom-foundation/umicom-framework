/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/diagnostic.h
 *
 * PURPOSE:
 *   Define editor diagnostics independently of their compiler, linter or language-server provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_DIAGNOSTIC_H
#define UMICOM_EDITOR_DIAGNOSTIC_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_DIAGNOSTIC_CAPACITY 8192U
typedef struct UmiEditorDiagnosticSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char source[128];
    char code[128];
    char message[1024];
    int severity;
    uint64_t line;
    uint64_t column;
    uint64_t end_line;
    uint64_t end_column;
    uint64_t revision;
} UmiEditorDiagnosticSnapshot;
typedef struct UmiEditorDiagnosticRegistry UmiEditorDiagnosticRegistry;
UmiStatus umi_editor_diagnostic_registry_create(UmiEditorDiagnosticRegistry **out_registry);
void umi_editor_diagnostic_registry_destroy(UmiEditorDiagnosticRegistry *registry);
UmiStatus umi_editor_diagnostic_registry_upsert(UmiEditorDiagnosticRegistry *registry,const UmiEditorDiagnosticSnapshot *item);
UmiStatus umi_editor_diagnostic_registry_remove(UmiEditorDiagnosticRegistry *registry,const char *id);
UmiStatus umi_editor_diagnostic_registry_find(const UmiEditorDiagnosticRegistry *registry,const char *id,UmiEditorDiagnosticSnapshot *out_item);
UmiStatus umi_editor_diagnostic_registry_at(const UmiEditorDiagnosticRegistry *registry,size_t index,UmiEditorDiagnosticSnapshot *out_item);
size_t umi_editor_diagnostic_registry_count(const UmiEditorDiagnosticRegistry *registry);
uint64_t umi_editor_diagnostic_registry_revision(const UmiEditorDiagnosticRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
