/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/preview.h
 *
 * PURPOSE:
 *   Compile/render semantic designer state and provide reusable logical preview
 *   viewport profiles for desktop, tablet, phone and custom hosts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Device/profile dimensions are Framework policy. A GTK4 or web application
 * only renders the viewport; it should not maintain another device catalogue.
 */

#ifndef UMICOM_DESIGNER_PREVIEW_H
#define UMICOM_DESIGNER_PREVIEW_H

#include "umicom/designer/document.h"
#include "umicom/declarative/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerPreviewViewport {
    UmiDesignerPreviewProfile profile;
    UmiDesignerRect rect;
} UmiDesignerPreviewViewport;

UmiStatus umi_designer_preview(
    const UmiDesignerDocument *document,
    const UmiDeclSchema *schema,
    char *out_text,
    size_t capacity,
    UmiDeclDiagnosticList *diagnostics
);

void umi_designer_preview_viewport_init(UmiDesignerPreviewViewport *viewport);

UmiStatus umi_designer_preview_apply_profile(
    UmiDesignerPreviewViewport *viewport,
    UmiDesignerPreviewProfile profile
);

UmiStatus umi_designer_preview_set_custom(
    UmiDesignerPreviewViewport *viewport,
    int width,
    int height
);

#ifdef __cplusplus
}
#endif

#endif
