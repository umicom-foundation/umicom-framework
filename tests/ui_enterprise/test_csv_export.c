/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_csv_export.c
 *
 * PURPOSE:
 *   Exercise the csv export enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/csv_export.h"
#include <stdio.h>
#include <string.h>
int main(void){char out[32];if(umi_ui_ent_csv_export_field("a,b",out,sizeof out)!=UMI_STATUS_OK)return 1;if(strcmp(out,"\"a,b\"")!=0)return 2;puts("ok");return 0;}
