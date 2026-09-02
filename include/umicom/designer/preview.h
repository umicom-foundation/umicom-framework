/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/preview.h
 *
 * PURPOSE:
 *   Compile/render semantic designer state and provide reusable logical preview
 *   viewport profiles for desktop, tablet, phone and custom hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the designer preview viewport data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerPreviewViewport {
    UmiDesignerPreviewProfile profile;
    UmiDesignerRect rect;
} UmiDesignerPreviewViewport;

/**
 * Provide the designer preview operation used by this module and its client applications.
 */
UmiStatus umi_designer_preview(
    const UmiDesignerDocument *document,
    const UmiDeclSchema *schema,
    char *out_text,
    size_t capacity,
    UmiDeclDiagnosticList *diagnostics
);

/**
 * Initialise designer preview viewport from caller-provided values so later operations
 * receive a known state.
 */
void umi_designer_preview_viewport_init(UmiDesignerPreviewViewport *viewport);

/**
 * Provide the designer preview apply profile operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_preview_apply_profile(
    UmiDesignerPreviewViewport *viewport,
    UmiDesignerPreviewProfile profile
);

/**
 * Provide the designer preview set custom operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_preview_set_custom(
    UmiDesignerPreviewViewport *viewport,
    int width,
    int height
);

#ifdef __cplusplus
}
#endif

#endif
