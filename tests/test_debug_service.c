/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_service.c
 *
 * PURPOSE:
 *   Implement the test debug service behavior for
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
 * File: tests/test_debug_service.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/debug/debug.h"
int main(void){UmiDebugService *p=NULL; UmiDebugServiceSnapshot s; if(umi_debug_service_create(&p)!=UMI_STATUS_OK)return 1; if(umi_debug_service_snapshot(p,&s)!=UMI_STATUS_OK||s.item_count!=0U)return 2; umi_debug_service_destroy(p); return 0;}
