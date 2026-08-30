/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/preview_matrix.h
 *
 * PURPOSE:
 *   Maintain a bounded multi-device preview matrix for desktop, tablet, mobile and web verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_PREVIEW_MATRIX_H
#define UMICOM_DESIGNER_ADAPTIVE_PREVIEW_MATRIX_H
#include "umicom/designer/adaptive/preview_case.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerPreviewMatrix {
    UmiDesignerPreviewCase cases[UMI_DESIGNER_ADAPTIVE_MAX_PREVIEWS];
    size_t count;
} UmiDesignerPreviewMatrix;
/* Initialise an empty preview matrix. */
void umi_designer_preview_matrix_init(UmiDesignerPreviewMatrix *matrix);
/* Add one uniquely identified responsive preview case. */
UmiStatus umi_designer_preview_matrix_add(UmiDesignerPreviewMatrix *matrix,
                                          const UmiDesignerPreviewCase *preview_case);
/* Find one preview case by stable id. */
const UmiDesignerPreviewCase *umi_designer_preview_matrix_find(const UmiDesignerPreviewMatrix *matrix,
                                                               const char *case_id);

#ifdef __cplusplus
}
#endif
#endif
