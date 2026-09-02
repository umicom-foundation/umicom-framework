/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_virtualizer.c
 *
 * PURPOSE:
 *   Exercise the row virtualizer enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_virtualizer.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntRowVirtualizer v={0};UmiUiEntViewport p={2U,3U,1U,2U,1U,1U};/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_ui_ent_row_virtualizer_update(&v,&p,10U)!=UMI_STATUS_OK)return 1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v.materialised.count==0U||v.generation!=1U)return 2;puts("ok");return 0;}
