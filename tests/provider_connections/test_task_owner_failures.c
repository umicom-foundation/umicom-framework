/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_task_owner_failures.c
 *
 * PURPOSE:
 *   Verify task-owner cleanup and failure propagation through inert lease and store boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "../../src/provider_connections/internal.h"
#include "umicom/provider_connections/task_owner.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/output_file.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char lease_marker,store_marker;
static unsigned acquired,closed,reads;
static int fail_allocation;
static UmiStatus lease_status=UMI_STATUS_OK,read_status=UMI_STATUS_OK,close_status=UMI_STATUS_OK;
static UmiProviderTaskReceipt stored;
static void *owner_calloc(size_t count,size_t size)
{ return fail_allocation?NULL:calloc(count,size); }
static UmiStatus owner_acquire(const char *path,UmiFileLease **slot)
{
    assert(path!=NULL && slot!=NULL && *slot==NULL);++acquired;
    if(lease_status==UMI_STATUS_OK) *slot=(UmiFileLease *)&lease_marker;
    return lease_status;
}
static UmiStatus owner_release(UmiFileLease **slot)
{
    assert(slot!=NULL);
    if(*slot==NULL) return UMI_STATUS_OK;
    assert(*slot==(UmiFileLease *)&lease_marker);++closed;*slot=NULL;return close_status;
}
static UmiStatus owner_read(UmiProviderTasks *store,const char *id,UmiProviderTaskReceipt *out)
{
    assert(store==(UmiProviderTasks *)&store_marker && strcmp(id,"scene-one")==0);++reads;
    if(read_status==UMI_STATUS_OK) *out=stored;
    return read_status;
}
/* Compile the actual owner body against inert resource boundaries. Canonical
 * receipt validation/encoding and path admission remain the production code.
 * Native handle behavior is separately covered by platform and owner tests. */
#define calloc owner_calloc
#define UmiFileLeaseTryAcquire owner_acquire
#define UmiFileLeaseRelease owner_release
#define UmiProviderTasksRead owner_read
#include "../../src/provider_connections/task_owner.c"
#undef UmiProviderTasksRead
#undef UmiFileLeaseRelease
#undef UmiFileLeaseTryAcquire
#undef calloc

int main(int argc,char **argv)
{
    assert(argc==2);const char *mode=argv[1];
    UmiProviderConnection connection={0};strcpy(connection.id,"video");strcpy(connection.provider_id,"example");
    strcpy(connection.label,"Example");strcpy(connection.endpoint,"https://api.example.invalid/tasks");
    strcpy(connection.model,"model");connection.route=UMI_PROVIDER_CONNECTION_HTTPS;connection.timeout_ms=30000U;connection.enabled=true;
    UmiProviderTaskContext context={0};strcpy(context.application_id,"media");strcpy(context.profile_id,"owner");
    strcpy(context.local_id,"scene-one");strcpy(context.operation_id,"video.generate");context.request_sha256[0]=9U;
    UmiProviderTaskReceipt expected;
    assert(UmiProviderTaskPrepare(&context,&connection,1U,&expected)==UMI_STATUS_OK);stored=expected;
#ifdef _WIN32
    const char *directory="C:/synthetic-owner";
#else
    const char *directory="/synthetic-owner";
#endif
    UmiStatus want=UMI_STATUS_OK;
    if(strcmp(mode,"allocation")==0) { fail_allocation=1;want=UMI_STATUS_OUT_OF_MEMORY; }
    else if(strcmp(mode,"lease-busy")==0) { lease_status=UMI_STATUS_BUSY;want=UMI_STATUS_BUSY; }
    else if(strcmp(mode,"read-failure")==0) { read_status=UMI_STATUS_PARSE_ERROR;want=UMI_STATUS_PARSE_ERROR; }
    else if(strcmp(mode,"changed")==0) { stored.context.request_sha256[0]=8U;want=UMI_STATUS_BUSY; }
    else if(strcmp(mode,"cleanup-failure")==0) { read_status=UMI_STATUS_NOT_FOUND;close_status=UMI_STATUS_IO_ERROR;want=UMI_STATUS_IO_ERROR; }
    else if(strcmp(mode,"invalid-path")==0) { directory="relative";want=UMI_STATUS_INVALID_ARGUMENT; }
    else assert(strcmp(mode,"check-failure")==0 || strcmp(mode,"release-failure")==0 || strcmp(mode,"scope-mismatch")==0);
    UmiProviderTaskOwner *owner=NULL;
    unsigned char before[sizeof(expected)];memcpy(before,&expected,sizeof(expected));
    assert(UmiProviderTaskOwnerAcquire((UmiProviderTasks *)&store_marker,directory,&expected,&owner)==want);
    assert(memcmp(before,&expected,sizeof(expected))==0);
    if(want!=UMI_STATUS_OK) {
        assert(owner==NULL);
        if(strcmp(mode,"allocation")==0 || strcmp(mode,"invalid-path")==0) assert(acquired==0U && reads==0U && closed==0U);
        else if(strcmp(mode,"lease-busy")==0) assert(acquired==1U && reads==0U && closed==0U);
        else assert(acquired==1U && reads==1U && closed==1U);
    } else {
        assert(owner!=NULL && acquired==1U && reads==1U && closed==0U);
        UmiProviderTaskOwner *saved=owner;
        if(strcmp(mode,"check-failure")==0) {
            read_status=UMI_STATUS_IO_ERROR;
            assert(UmiProviderTaskOwnerCheck(owner,&expected)==UMI_STATUS_IO_ERROR);
            assert(owner==saved && closed==0U);
            read_status=UMI_STATUS_OK;assert(UmiProviderTaskOwnerCheck(owner,&expected)==UMI_STATUS_OK);
        } else if(strcmp(mode,"scope-mismatch")==0) {
            strcpy(expected.context.profile_id,"other");
            assert(UmiProviderTaskOwnerCheck(owner,&expected)==UMI_STATUS_INVALID_ARGUMENT && reads==1U && closed==0U);
        } else close_status=UMI_STATUS_IO_ERROR;
        assert(UmiProviderTaskOwnerRelease(&owner)==close_status && owner==NULL && closed==1U);
        assert(UmiProviderTaskOwnerRelease(&owner)==UMI_STATUS_OK && closed==1U);
    }
    return 0;
}
