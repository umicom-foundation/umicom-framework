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
UmiStatus umi_appearance_renderer_parity_matrix_init(UmiAppearanceRendererParityMatrix *matrix){if(matrix==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(matrix,0,sizeof *matrix);return UMI_STATUS_OK;}
UmiStatus umi_appearance_renderer_parity_matrix_add(UmiAppearanceRendererParityMatrix *matrix,const UmiAppearanceRendererParityEntry *entry){if(matrix==NULL||entry==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(matrix->count>=8U)return UMI_STATUS_CAPACITY_EXCEEDED;matrix->items[matrix->count++]=*entry;return UMI_STATUS_OK;}
int umi_appearance_renderer_parity_matrix_passed(const UmiAppearanceRendererParityMatrix *matrix){size_t i;if(matrix==NULL||matrix->count==0U)return 0;for(i=0;i<matrix->count;i++){const UmiAppearanceRendererParityEntry *e=&matrix->items[i];if(!(e->theme&&e->typography&&e->scaling&&e->accessibility))return 0;}return 1;}
