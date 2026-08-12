/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_service.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/test_platform/test_platform.h"
int main(void){UmiTestPlatformService *p=NULL; UmiTestPlatformServiceSnapshot s; if(umi_test_platform_service_create(&p)!=UMI_STATUS_OK)return 1; if(umi_test_platform_service_snapshot(p,&s)!=UMI_STATUS_OK||s.total_count!=0U)return 2; umi_test_platform_service_destroy(p); return 0;}
