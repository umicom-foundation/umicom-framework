/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/renderer_parity_matrix.c
 *
 * PURPOSE:
 *   Track renderer-by-renderer appearance conformance for semantic theme capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_parity_matrix.h"
#include <string.h>
/*
 * Initialise appearance renderer parity matrix from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_appearance_renderer_parity_matrix_init(UmiAppearanceRendererParityMatrix *matrix){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(matrix,0,sizeof *matrix);return UMI_STATUS_OK;}
/*
 * Add appearance renderer parity matrix only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_appearance_renderer_parity_matrix_add(UmiAppearanceRendererParityMatrix *matrix,const UmiAppearanceRendererParityEntry *entry){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||entry==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix->count>=8U)return UMI_STATUS_CAPACITY_EXCEEDED;matrix->items[matrix->count++]=*entry;return UMI_STATUS_OK;}
/*
 * Provide the appearance renderer parity matrix passed operation used by this module and
 * its client applications.
 */
int umi_appearance_renderer_parity_matrix_passed(const UmiAppearanceRendererParityMatrix *matrix){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(matrix==NULL||matrix->count==0U)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<matrix->count;i++){const UmiAppearanceRendererParityEntry *e=&matrix->items[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!(e->theme&&e->typography&&e->scaling&&e->accessibility))return 0;}return 1;}
