/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/preview.h
 *
 * PURPOSE:
 *   Compile and render a designer document into deterministic preview text for tests and non-GUI hosts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_PREVIEW_H
#define UMICOM_DESIGNER_PREVIEW_H
#include "umicom/designer/document.h"
#include "umicom/declarative/compiler.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_designer_preview(const UmiDesignerDocument *document,const UmiDeclSchema *schema,char *out_text,size_t capacity,UmiDeclDiagnosticList *diagnostics);
#ifdef __cplusplus
}
#endif
#endif
