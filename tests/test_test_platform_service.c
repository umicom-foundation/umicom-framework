/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_service.c
 *
 * PURPOSE:
 *   Implement the test test platform service behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_service.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/test_platform/test_platform.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiTestPlatformService *p=NULL; UmiTestPlatformServiceSnapshot s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_test_platform_service_create(&p)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_test_platform_service_snapshot(p,&s)!=UMI_STATUS_OK||s.total_count!=0U)return 2; umi_test_platform_service_destroy(p); return 0;}
