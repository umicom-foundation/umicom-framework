/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_dpi_class.c
 *
 * PURPOSE:
 *   Verify classify physical display density for asset selection without hard-coding platform DPI APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/dpi_class.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAppearanceDpiClass c;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_appearance_dpi_class_from_dpi(192U,&c)!=UMI_STATUS_OK||c!=UMI_APPEARANCE_DPI_VERY_HIGH)return 1;return 0;}
