/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_workbench_services.c
 *
 * PURPOSE:
 *   Implement the test ui workbench services behavior for
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
 * File: tests/test_ui_workbench_services.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/ui/workbench_services.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiWorkbenchServices *p=NULL; UmiUiWorkbenchServicesSnapshot s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_workbench_services_create(&p)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_workbench_services_snapshot(p,&s)!=UMI_STATUS_OK||s.item_count!=0U)return 2; umi_ui_workbench_services_destroy(p); return 0;}
