/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_csv_export.c
 *
 * PURPOSE:
 *   Exercise the csv export enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/csv_export.h"
#include <stdio.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){char out[32];/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_csv_export_field("a,b",out,sizeof out)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(out,"\"a,b\"")!=0)return 2;puts("ok");return 0;}
