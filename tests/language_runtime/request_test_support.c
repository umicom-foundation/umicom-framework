/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/request_test_support.c
 *
 * PURPOSE:
 *   Implement the shared in-memory language-server request fixture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "request_test_support.h"
#include <string.h>
UmiStatus request_test_fixture_create(RequestTestFixture*f){UmiLanguageRuntimeTransport t;UmiLanguageServerProfile p={0};UmiStatus q;if(!f)return UMI_STATUS_INVALID_ARGUMENT;memset(f,0,sizeof(*f));strcpy(p.id,"test.profile");strcpy(p.display_name,"Test");strcpy(p.executable,"test-server");p.enabled=1;p.revision=1;q=umi_language_runtime_memory_transport_create(&f->memory,&t);if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_server_create_with_transport("test.server",&p,"file:///workspace",&t,&f->server);if(q!=UMI_STATUS_OK&&t.instance)t.destroy(t.instance);return q;}
void request_test_fixture_destroy(RequestTestFixture*f){if(!f)return;umi_language_runtime_server_destroy(f->server);f->server=NULL;f->memory=NULL;}
