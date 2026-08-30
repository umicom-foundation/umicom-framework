/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/document.h
 *
 * PURPOSE:
 *   Wrap a declarative document with designer revision and dirty-state semantics used by authoring hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_DOCUMENT_H
#define UMICOM_DESIGNER_DOCUMENT_H
#include "umicom/designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDesignerDocument UmiDesignerDocument;
typedef struct UmiDesignerDocumentSnapshot { char application_id[UMI_DECL_ID_CAPACITY]; size_t component_count; uint64_t revision; int dirty; } UmiDesignerDocumentSnapshot;
UmiStatus umi_designer_document_create(const char *application_id,UmiDesignerDocument **out_document);
UmiStatus umi_designer_document_from_declarative(UmiDeclDocument *document,int take_ownership,UmiDesignerDocument **out_document);
void umi_designer_document_destroy(UmiDesignerDocument *document);
UmiDeclDocument *umi_designer_document_declarative(UmiDesignerDocument *document);
UmiStatus umi_designer_document_snapshot(const UmiDesignerDocument *document,UmiDesignerDocumentSnapshot *out_snapshot);
void umi_designer_document_mark_saved(UmiDesignerDocument *document);
void umi_designer_document_mark_changed(UmiDesignerDocument *document);
#ifdef __cplusplus
}
#endif
#endif
