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
int main(void){UmiUiWorkbenchServices *p=NULL; UmiUiWorkbenchServicesSnapshot s; if(umi_ui_workbench_services_create(&p)!=UMI_STATUS_OK)return 1; if(umi_ui_workbench_services_snapshot(p,&s)!=UMI_STATUS_OK||s.item_count!=0U)return 2; umi_ui_workbench_services_destroy(p); return 0;}
