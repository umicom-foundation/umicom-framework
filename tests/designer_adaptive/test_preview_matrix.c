/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_preview_matrix.c
 *
 * PURPOSE:
 *   Validate maintain a bounded multi-device preview matrix for desktop, tablet, mobile and web verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/preview_matrix.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerPreviewMatrix m; UmiDesignerResponsivePreview p; UmiDesignerPreviewCase c; UmiAdaptiveViewport v={800,600}; UmiAdaptiveInsets s={0,0,0,0};
    umi_designer_preview_matrix_init(&m);
    CHECK(umi_designer_responsive_preview_init(&p,"p",v,s,UMI_ADAPTIVE_INPUT_POINTER,UMI_RAD_TARGET_QT6)==UMI_STATUS_OK);
    CHECK(umi_designer_preview_case_init(&c,"case",&p,UMI_DESIGN_SIZE_MEDIUM,UMI_ADAPTIVE_NAV_DRAWER)==UMI_STATUS_OK);
    CHECK(umi_designer_preview_matrix_add(&m,&c)==UMI_STATUS_OK);
    CHECK(umi_designer_preview_matrix_find(&m,"case")!=NULL);
    return 0;
}
