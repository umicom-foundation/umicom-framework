/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_platform.c
 *
 * PURPOSE:
 *   Verify end-to-end completion: memory server -> request -> framed response -> Language Service -> Editor Session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/platform.h"
#include "umicom/language_runtime/memory_transport.h"
int main(void){UmiLanguageRuntimePlatform*p=NULL;UmiLanguageRuntimeMemoryTransport*m;UmiLanguageRuntimeTransport t;UmiLanguageRuntimeServer*s=NULL;UmiLanguageServerProfile sp={0};UmiLanguageRuntimeInitializeResult caps={0};char fr[1024];size_t n=0;int handled=0;assert(umi_language_runtime_platform_create(&p)==UMI_STATUS_OK);strcpy(sp.id,"test");strcpy(sp.display_name,"Test");strcpy(sp.executable,"test");sp.enabled=1;assert(umi_language_runtime_memory_transport_create(&m,&t)==UMI_STATUS_OK);assert(umi_language_runtime_server_create_with_transport("server",&sp,"file:///root",&t,&s)==UMI_STATUS_OK);caps.completion=1;assert(umi_language_runtime_platform_attach_server(p,"c","file:///root",s,&caps)==UMI_STATUS_OK);assert(umi_language_runtime_platform_open_document(p,"file:///root",".","doc","file:///root/a.c","c","a.c","int x;\n",0)==UMI_STATUS_OK);assert(umi_language_runtime_platform_request_completion(p,"doc",0,3)==UMI_STATUS_OK);assert(umi_language_runtime_frame_encode("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":[{\"label\":\"item\"}]}",fr,sizeof(fr),&n)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_push_read(m,fr,n)==UMI_STATUS_OK);assert(umi_language_runtime_platform_pump_document(p,"doc",0,&handled)==UMI_STATUS_OK);assert(handled);assert(umi_language_completion_registry_count(umi_language_service_completion(umi_language_runtime_platform_language(p)))==1);assert(umi_editor_completion_registry_count(umi_editor_session_completion(umi_language_runtime_platform_editor(p)))==1);umi_language_runtime_platform_destroy(p);return 0;}
