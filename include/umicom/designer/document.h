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

/*
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_DOCUMENT_H
#define UMICOM_DESIGNER_DOCUMENT_H
#include "umicom/designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the designer document data shared with callers of this public contract.
 */
typedef struct UmiDesignerDocument UmiDesignerDocument;
/**
 * Represent the designer document snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerDocumentSnapshot { char application_id[UMI_DECL_ID_CAPACITY]; size_t component_count; uint64_t revision; int dirty; } UmiDesignerDocumentSnapshot;
/**
 * Initialise designer document from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_document_create(const char *application_id,UmiDesignerDocument **out_document);
/**
 * Provide the designer document from declarative operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_document_from_declarative(UmiDeclDocument *document,int take_ownership,UmiDesignerDocument **out_document);
/**
 * Release or reset state held by designer document so the same storage can be reused
 * safely.
 */
void umi_designer_document_destroy(UmiDesignerDocument *document);
/**
 * Provide the designer document declarative operation used by this module and its client
 * applications.
 */
UmiDeclDocument *umi_designer_document_declarative(UmiDesignerDocument *document);
/**
 * Provide the designer document snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_document_snapshot(const UmiDesignerDocument *document,UmiDesignerDocumentSnapshot *out_snapshot);
/**
 * Provide the designer document mark saved operation used by this module and its client
 * applications.
 */
void umi_designer_document_mark_saved(UmiDesignerDocument *document);
/**
 * Provide the designer document mark changed operation used by this module and its client
 * applications.
 */
void umi_designer_document_mark_changed(UmiDesignerDocument *document);
#ifdef __cplusplus
}
#endif
#endif
