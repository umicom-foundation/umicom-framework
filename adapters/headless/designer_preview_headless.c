/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/designer_preview_headless.c
 *
 * PURPOSE:
 *   Expose a headless designer-preview entry point without introducing GUI dependencies into the designer engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The headless adapter turns semantic state into deterministic testable output
 * without requiring a graphical display or GTK event loop.
 */

#include "umicom/designer/preview.h"
UmiStatus umi_designer_headless_preview(const UmiDesignerDocument *document,const UmiDeclSchema *schema,char *out_text,size_t capacity,UmiDeclDiagnosticList *diagnostics)
{
    return umi_designer_preview(document,schema,out_text,capacity,diagnostics);
}
