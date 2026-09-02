/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_descriptor.c
 *
 * PURPOSE:
 *   Exercise the row descriptor enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_descriptor.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntRowDescriptor v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_row_descriptor_init(&v)!=UMI_STATUS_OK)return 1;v.row_key=42U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_row_descriptor_validate(&v))return 9;puts("ok");return 0;}
