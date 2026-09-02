/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_language_service.c
 *
 * PURPOSE:
 *   Implement the test language service behavior for
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
 * File: tests/test_language_service.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/language/language.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageService *p=NULL; UmiLanguageServiceSnapshot s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_service_create(&p)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_service_snapshot(p,&s)!=UMI_STATUS_OK||s.definition_count==0U||s.item_count!=s.definition_count)return 2; umi_language_service_destroy(p); return 0;}
