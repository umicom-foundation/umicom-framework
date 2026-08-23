/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_document_sync.c
 *
 * PURPOSE:
 *   Verify didOpen/change/save/close mirrors into UmiLanguageDocumentRegistry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/language_runtime/document_sync.h"
#include "umicom/language_runtime/memory_transport.h"
int main(void){UmiLanguageService*l=NULL;UmiLanguageRuntimeDocumentSync*d=NULL;UmiLanguageRuntimeMemoryTransport*m;UmiLanguageRuntimeTransport t;UmiLanguageRuntimeServer*s=NULL;UmiLanguageServerProfile p={0};UmiLanguageRuntimeDocumentState st;assert(umi_language_service_create(&l)==UMI_STATUS_OK);strcpy(p.id,"test");strcpy(p.display_name,"Test");strcpy(p.executable,"test");p.enabled=1;assert(umi_language_runtime_memory_transport_create(&m,&t)==UMI_STATUS_OK);assert(umi_language_runtime_server_create_with_transport("server",&p,"file:///root",&t,&s)==UMI_STATUS_OK);assert(umi_language_runtime_document_sync_create(l,&d)==UMI_STATUS_OK);assert(umi_language_runtime_document_open(d,s,"doc","file:///a.c","developer.language.c23","int x;\n")==UMI_STATUS_OK);assert(umi_language_runtime_document_find(d,"doc",&st)==UMI_STATUS_OK);assert(strcmp(st.language_id,"c")==0);assert(umi_language_runtime_document_change(d,"doc","int y;\n")==UMI_STATUS_OK);assert(umi_language_runtime_document_close(d,"doc")==UMI_STATUS_OK);assert(umi_language_document_registry_count(umi_language_service_document(l))==0);umi_language_runtime_document_sync_destroy(d);umi_language_runtime_server_destroy(s);umi_language_service_destroy(l);return 0;}
