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
/*
 * Initialise request test fixture from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus request_test_fixture_create(RequestTestFixture*f){UmiLanguageRuntimeTransport t;UmiLanguageServerProfile p={0};UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!f)return UMI_STATUS_INVALID_ARGUMENT;memset(f,0,sizeof(*f));strcpy(p.id,"test.profile");strcpy(p.display_name,"Test");strcpy(p.executable,"test-server");p.enabled=1;p.revision=1;q=umi_language_runtime_memory_transport_create(&f->memory,&t);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;q=umi_language_runtime_server_create_with_transport("test.server",&p,"file:///workspace",&t,&f->server);if(q!=UMI_STATUS_OK&&t.instance)t.destroy(t.instance);return q;}
/*
 * Release or reset state held by request test fixture so the same storage can be reused
 * safely.
 */
void request_test_fixture_destroy(RequestTestFixture*f){/* Apply this branch only when its contract condition is satisfied. */ if(!f)return;umi_language_runtime_server_destroy(f->server);f->server=NULL;f->memory=NULL;}
