/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/preview_matrix.c
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
#include "umicom/designer/adaptive/preview_matrix.h"
#include <string.h>

/* Reset the bounded preview matrix. */
void umi_designer_preview_matrix_init(UmiDesignerPreviewMatrix *matrix)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(matrix!=NULL)memset(matrix,0,sizeof *matrix);
}
/* Add unique cases so test and preview results have stable correspondence. */
UmiStatus umi_designer_preview_matrix_add(UmiDesignerPreviewMatrix *matrix,
                                          const UmiDesignerPreviewCase *preview_case)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(matrix==NULL||preview_case==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_designer_preview_matrix_find(matrix,preview_case->case_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(matrix->count>=UMI_DESIGNER_ADAPTIVE_MAX_PREVIEWS)return UMI_STATUS_CAPACITY_EXCEEDED;
    matrix->cases[matrix->count++]=*preview_case;
    return UMI_STATUS_OK;
}
/* Locate a preview case by stable identity. */
const UmiDesignerPreviewCase *umi_designer_preview_matrix_find(const UmiDesignerPreviewMatrix *matrix,
                                                               const char *case_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(matrix==NULL||case_id==NULL)return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<matrix->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(matrix->cases[i].case_id,case_id)==0)return &matrix->cases[i];
    return NULL;
}
