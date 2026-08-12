/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_source_control_service_platform.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/source_control/source_control.h"
int main(void){UmiSourceControlService *p=NULL; UmiSourceControlServiceSnapshot s; if(umi_source_control_service_create(&p)!=UMI_STATUS_OK)return 1; if(umi_source_control_service_snapshot(p,&s)!=UMI_STATUS_OK||s.item_count!=0U)return 2; umi_source_control_service_destroy(p); return 0;}
