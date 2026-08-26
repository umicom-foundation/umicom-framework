/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_renderer_parity_matrix.c
 *
 * PURPOSE:
 *   Verify track renderer-by-renderer appearance conformance for semantic theme capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_parity_matrix.h"
int main(void){UmiAppearanceRendererParityMatrix m;UmiAppearanceRendererParityEntry e={UMI_APPEARANCE_RENDERER_GTK4,true,true,true,true};if(umi_appearance_renderer_parity_matrix_init(&m)!=UMI_STATUS_OK)return 1;if(umi_appearance_renderer_parity_matrix_add(&m,&e)!=UMI_STATUS_OK)return 2;if(!umi_appearance_renderer_parity_matrix_passed(&m))return 3;return 0;}
