/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_service_bridge.c
 *
 * PURPOSE:
 *   Verify decoded completion publishes into the existing UmiLanguageService.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/service_bridge.h"
int main(void){UmiLanguageService*l=NULL;UmiLanguageRuntimeServiceBridge b;UmiLanguageRuntimeCompletionResult r={0};assert(umi_language_service_create(&l)==UMI_STATUS_OK);assert(umi_language_runtime_service_bridge_init(&b,l)==UMI_STATUS_OK);strcpy(r.items[0].label,"printf");strcpy(r.items[0].insert_text,"printf");r.count=1;assert(umi_language_runtime_publish_completion(&b,"doc",1,2,&r)==UMI_STATUS_OK);assert(umi_language_completion_registry_count(umi_language_service_completion(l))==1);umi_language_service_destroy(l);return 0;}
