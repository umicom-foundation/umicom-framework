/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_editor_bridge.c
 *
 * PURPOSE:
 *   Verify Language Service completion projects into the existing Editor Session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/editor_bridge.h"
#include "umicom/language_runtime/service_bridge.h"
int main(void){UmiLanguageService*l=NULL;UmiEditorSession*e=NULL;UmiLanguageRuntimeServiceBridge lb;UmiLanguageRuntimeEditorBridge eb;UmiLanguageRuntimeCompletionResult r={0};assert(umi_language_service_create(&l)==UMI_STATUS_OK);assert(umi_editor_session_create(&e)==UMI_STATUS_OK);assert(umi_language_runtime_service_bridge_init(&lb,l)==UMI_STATUS_OK);assert(umi_language_runtime_editor_bridge_init(&eb,l,e)==UMI_STATUS_OK);strcpy(r.items[0].label,"main");strcpy(r.items[0].insert_text,"main");r.count=1;assert(umi_language_runtime_publish_completion(&lb,"doc",0,0,&r)==UMI_STATUS_OK);assert(umi_language_runtime_editor_bridge_sync_completion(&eb,"doc")==UMI_STATUS_OK);assert(umi_editor_completion_registry_count(umi_editor_session_completion(e))==1);umi_editor_session_destroy(e);umi_language_service_destroy(l);return 0;}
