/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/task_owner.c
 *
 * PURPOSE:
 *   Bind native guard lifetime to stable task scope and canonical persisted receipt checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "internal.h"
#include "umicom/provider_connections/task_owner.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiProviderTaskOwner {
    UmiProviderTasks *store;
    UmiFileLease *lease;
    char application_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char profile_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char local_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
};

UmiStatus UmiProviderTaskOwnerPath(const char *directory,
    const UmiProviderTaskReceipt *receipt,char *out,size_t capacity)
{
    if(directory==NULL || out==NULL || UmiProviderTaskValidate(receipt)!=UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;
    while(length<UMI_PATH_CAPACITY && directory[length]!='\0') ++length;
    if(length==UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(length==0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Identifier grammar excludes NUL within visible bytes. Including each
     * terminator frames the fields, so scope boundaries cannot be ambiguous. */
    static const char domain[]="Umicom provider task owner v1";
    const char *fields[]={receipt->context.application_id,receipt->context.profile_id,
        receipt->context.local_id};
    UmiSha256 hash;UmiSha256Init(&hash);
    UmiStatus status=UmiSha256Update(&hash,domain,sizeof(domain));
    for(size_t i=0U;status==UMI_STATUS_OK && i<3U;++i)
        status=UmiSha256Update(&hash,fields[i],strlen(fields[i])+1U);
    unsigned char digest[UMI_SHA256_BYTES];char hex[UMI_SHA256_HEX_CAPACITY];
    if(status==UMI_STATUS_OK) status=UmiSha256Final(&hash,digest);
    if(status!=UMI_STATUS_OK) return status;
    UmiSha256Hex(digest,hex);
    bool separator=directory[length-1U]=='/';
#ifdef _WIN32
    separator=separator || directory[length-1U]=='\\';
#endif
    char candidate[UMI_PATH_CAPACITY];
    int count=snprintf(candidate,sizeof(candidate),"%s%sumicom-provider-task-%s.lock",
        directory,separator?"":"/",hex);
    if(count<0 || (size_t)count>=sizeof(candidate) || (size_t)count>=capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status=UmiOutputFileValidatePath(candidate);
    if(status==UMI_STATUS_OK) memcpy(out,candidate,(size_t)count+1U);
    return status;
}

UmiStatus UmiProviderTaskOwnerCheck(UmiProviderTaskOwner *owner,
    const UmiProviderTaskReceipt *expected)
{
    if(owner==NULL || owner->lease==NULL || UmiProviderTaskValidate(expected)!=UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(owner->application_id,expected->context.application_id)!=0 ||
        strcmp(owner->profile_id,expected->context.profile_id)!=0 ||
        strcmp(owner->local_id,expected->context.local_id)!=0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderTaskReceipt current;
    UmiStatus status=UmiProviderTasksRead(owner->store,owner->local_id,&current);
    if(status!=UMI_STATUS_OK) return status;
    /* Revision alone cannot detect restored or externally replaced content.
     * Reuse the storage codec instead of comparing C structure padding or
     * maintaining another list of fields that can drift from persistence. */
    char expected_wire[UMI_PROVIDER_TASK_WIRE_CAPACITY],current_wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];
    size_t expected_length=0U,current_length=0U;
    status=UmiProviderTaskEncode(expected,expected_wire,sizeof(expected_wire),&expected_length);
    if(status==UMI_STATUS_OK)
        status=UmiProviderTaskEncode(&current,current_wire,sizeof(current_wire),&current_length);
    if(status==UMI_STATUS_OK && (expected_length!=current_length ||
        memcmp(expected_wire,current_wire,expected_length)!=0)) status=UMI_STATUS_BUSY;
    return status;
}

UmiStatus UmiProviderTaskOwnerAcquire(UmiProviderTasks *store,const char *directory,
    const UmiProviderTaskReceipt *expected,UmiProviderTaskOwner **out)
{
    if(store==NULL || out==NULL || *out!=NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_PATH_CAPACITY];
    UmiStatus status=UmiProviderTaskOwnerPath(directory,expected,path,sizeof(path));
    if(status!=UMI_STATUS_OK) return status;
    UmiProviderTaskOwner *owner=calloc(1U,sizeof(*owner));
    if(owner==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->store=store;
    strcpy(owner->application_id,expected->context.application_id);
    strcpy(owner->profile_id,expected->context.profile_id);
    strcpy(owner->local_id,expected->context.local_id);
    status=UmiFileLeaseTryAcquire(path,&owner->lease);
    if(status==UMI_STATUS_OK) status=UmiProviderTaskOwnerCheck(owner,expected);
    if(status!=UMI_STATUS_OK) {
        UmiStatus cleanup=UmiProviderTaskOwnerRelease(&owner);
        /* A failed native close must remain observable; the underlying lease
         * contract consumes its descriptor once and never retries it blindly. */
        return cleanup==UMI_STATUS_OK?status:cleanup;
    }
    *out=owner;return UMI_STATUS_OK;
}

UmiStatus UmiProviderTaskOwnerRelease(UmiProviderTaskOwner **slot)
{
    if(slot==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(*slot==NULL) return UMI_STATUS_OK;
    UmiProviderTaskOwner *owner=*slot;
    UmiStatus status=UmiFileLeaseRelease(&owner->lease);
    free(owner);*slot=NULL;return status;
}
