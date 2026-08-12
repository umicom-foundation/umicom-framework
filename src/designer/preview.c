/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/preview.c
 *
 * PURPOSE:
 *   Compile semantic designer state, render it headlessly and provide reusable
 *   logical preview viewport profiles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Preview dimensions are semantic authoring settings and deliberately contain
 * no GTK4 types, so every future Umicom application can reuse them.
 */

#include "umicom/designer/preview.h"
#include "umicom/declarative/renderer.h"

UmiStatus umi_designer_preview(
    const UmiDesignerDocument *document,
    const UmiDeclSchema *schema,
    char *out_text,
    size_t capacity,
    UmiDeclDiagnosticList *diagnostics)
{
    UmiDeclApplicationPlan plan;
    UmiStatus status;

    if (document == NULL || schema == NULL ||
        out_text == NULL || diagnostics == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_decl_compile(
        umi_designer_document_declarative((UmiDesignerDocument *)document),
        schema,
        &plan,
        diagnostics
    );
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_decl_headless_render(&plan, out_text, capacity);
    umi_decl_plan_dispose(&plan);
    return status;
}

void umi_designer_preview_viewport_init(UmiDesignerPreviewViewport *viewport)
{
    if (viewport == NULL) {
        return;
    }

    viewport->profile = UMI_DESIGNER_PREVIEW_DESKTOP;
    viewport->rect.x = 0;
    viewport->rect.y = 0;
    viewport->rect.width = 1440;
    viewport->rect.height = 900;
}

UmiStatus umi_designer_preview_apply_profile(
    UmiDesignerPreviewViewport *viewport,
    UmiDesignerPreviewProfile profile)
{
    int width;
    int height;

    if (viewport == NULL || profile == UMI_DESIGNER_PREVIEW_CUSTOM) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    switch (profile) {
        case UMI_DESIGNER_PREVIEW_DESKTOP:
            width = 1440; height = 900; break;
        case UMI_DESIGNER_PREVIEW_LAPTOP:
            width = 1366; height = 768; break;
        case UMI_DESIGNER_PREVIEW_TABLET_LANDSCAPE:
            width = 1024; height = 768; break;
        case UMI_DESIGNER_PREVIEW_TABLET_PORTRAIT:
            width = 768; height = 1024; break;
        case UMI_DESIGNER_PREVIEW_PHONE_LANDSCAPE:
            width = 844; height = 390; break;
        case UMI_DESIGNER_PREVIEW_PHONE_PORTRAIT:
            width = 390; height = 844; break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    viewport->profile = profile;
    viewport->rect.x = 0;
    viewport->rect.y = 0;
    viewport->rect.width = width;
    viewport->rect.height = height;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_preview_set_custom(
    UmiDesignerPreviewViewport *viewport,
    int width,
    int height)
{
    if (viewport == NULL || width <= 0 || height <= 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    viewport->profile = UMI_DESIGNER_PREVIEW_CUSTOM;
    viewport->rect.x = 0;
    viewport->rect.y = 0;
    viewport->rect.width = width;
    viewport->rect.height = height;
    return UMI_STATUS_OK;
}
