/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_renderer_parity_matrix.c
 *
 * PURPOSE:
 *   Verify track renderer-by-renderer appearance conformance for semantic theme capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_parity_matrix.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAppearanceRendererParityMatrix m;UmiAppearanceRendererParityEntry e={UMI_APPEARANCE_RENDERER_GTK4,true,true,true,true};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_renderer_parity_matrix_init(&m)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_renderer_parity_matrix_add(&m,&e)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_appearance_renderer_parity_matrix_passed(&m))return 3;return 0;}
