/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/preview.c
 *
 * PURPOSE:
 *   Compile semantic designer state and render it headlessly before any toolkit-specific preview is launched.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The functions below modify semantic designer state so undo, preview and
 * generation behave the same in Studio, headless tests and future hosts.
 */

#include "umicom/designer/preview.h"
#include "umicom/declarative/renderer.h"
UmiStatus umi_designer_preview(const UmiDesignerDocument *d,const UmiDeclSchema *s,char *out,size_t cap,UmiDeclDiagnosticList *diags){UmiDeclApplicationPlan plan;UmiStatus status;if(d==NULL||s==NULL||out==NULL||diags==NULL)return UMI_STATUS_INVALID_ARGUMENT;status=umi_decl_compile(umi_designer_document_declarative((UmiDesignerDocument *)d),s,&plan,diags);if(status!=UMI_STATUS_OK)return status;status=umi_decl_headless_render(&plan,out,cap);umi_decl_plan_dispose(&plan);return status;}
