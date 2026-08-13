/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_language_service.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/language/language.h"
int main(void){UmiLanguageService *p=NULL; UmiLanguageServiceSnapshot s; if(umi_language_service_create(&p)!=UMI_STATUS_OK)return 1; if(umi_language_service_snapshot(p,&s)!=UMI_STATUS_OK||s.definition_count==0U||s.item_count!=s.definition_count)return 2; umi_language_service_destroy(p); return 0;}
