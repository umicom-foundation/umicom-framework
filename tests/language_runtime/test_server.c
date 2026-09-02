/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_server.c
 *
 * PURPOSE:
 *   Verify request framing and response correlation through an in-memory server transport.
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
#include "umicom/language_runtime/memory_transport.h"
#include "umicom/language_runtime/server.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiLanguageRuntimeMemoryTransport*m;UmiLanguageRuntimeTransport t;UmiLanguageRuntimeServer*s=NULL;UmiLanguageServerProfile p={0};UmiLanguageRuntimeEnvelope e;char fr[512];size_t n=0;uint64_t id=0;strcpy(p.id,"test");strcpy(p.display_name,"Test");strcpy(p.executable,"test");p.enabled=1;assert(umi_language_runtime_memory_transport_create(&m,&t)==UMI_STATUS_OK);assert(umi_language_runtime_server_create_with_transport("server",&p,"file:///root",&t,&s)==UMI_STATUS_OK);assert(umi_language_runtime_server_send_request(s,"textDocument/hover","{}","doc",&id)==UMI_STATUS_OK);assert(id==1);assert(umi_language_runtime_frame_encode("{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}",fr,sizeof(fr),&n)==UMI_STATUS_OK);assert(umi_language_runtime_memory_transport_push_read(m,fr,n)==UMI_STATUS_OK);assert(umi_language_runtime_server_receive(s,0,&e)==UMI_STATUS_OK);assert(strcmp(e.method,"textDocument/hover")==0);umi_language_runtime_server_destroy(s);return 0;}
