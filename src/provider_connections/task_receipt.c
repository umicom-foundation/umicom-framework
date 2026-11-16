/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/task_receipt.c
 *
 * PURPOSE:
 *   Stage revision-bound provider task transitions and recovery observations using canonical connection identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "internal.h"
#include "umicom/provider_connections/tasks.h"
#include <string.h>
static bool task_context_valid(const UmiProviderTaskContext *value)
{
    return value!=NULL &&
        UmiProviderConnectionIdValid(value->application_id,sizeof(value->application_id)) &&
        UmiProviderConnectionIdValid(value->profile_id,sizeof(value->profile_id)) &&
        UmiProviderConnectionIdValid(value->local_id,sizeof(value->local_id)) &&
        UmiProviderConnectionIdValid(value->operation_id,sizeof(value->operation_id));
}
static bool task_remote_valid(const char *text)
{
    if(text==NULL) return false;
    for(size_t i=0U;i<UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY;++i) {
        unsigned char c=(unsigned char)text[i];
        if(c==0U) return i!=0U;
        bool alphanumeric=(c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');
        if(!alphanumeric && (i==0U || (c!='.' && c!='_' && c!='-' && c!=':'))) return false;
    }
    return false;
}
UmiStatus UmiProviderTaskValidate(const UmiProviderTaskReceipt *receipt)
{
    if(receipt==NULL || receipt->schema!=UMI_PROVIDER_TASK_SCHEMA || receipt->revision==0U ||
        !task_context_valid(&receipt->context) || receipt->connection_revision==0U ||
        !UmiProviderConnectionIdValid(receipt->connection_id,sizeof(receipt->connection_id)) ||
        !UmiProviderConnectionIdValid(receipt->provider_id,sizeof(receipt->provider_id)) ||
        receipt->phase<UMI_PROVIDER_TASK_PREPARED || receipt->phase>UMI_PROVIDER_TASK_OUTPUT_STORED)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(receipt->phase==UMI_PROVIDER_TASK_PREPARED || receipt->phase==UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN) {
        if(receipt->remote_id[0]!='\0') return UMI_STATUS_INVALID_ARGUMENT;
    } else if(!task_remote_valid(receipt->remote_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if(receipt->phase==UMI_PROVIDER_TASK_OUTPUT_STORED) {
        if(receipt->output_bytes==0U) return UMI_STATUS_INVALID_ARGUMENT;
    } else {
        if(receipt->output_bytes!=0U) return UMI_STATUS_INVALID_ARGUMENT;
        for(size_t i=0U;i<UMI_SHA256_BYTES;++i)
            if(receipt->output_sha256[i]!=0U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
static UmiStatus task_review(const UmiProviderTaskReceipt *receipt,uint64_t expected,
    const UmiProviderTaskReceipt *output)
{
    if(output==NULL || UmiProviderTaskValidate(receipt)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    return expected==receipt->revision?UMI_STATUS_OK:UMI_STATUS_BUSY;
}
/* Only publication changes the revision. Candidate storage keeps aliases and
 * failed transitions from partially replacing a host's accepted receipt. */
static UmiStatus task_commit(const UmiProviderTaskReceipt *before,UmiProviderTaskReceipt *candidate,
    UmiProviderTaskReceipt *out)
{
    if(before->revision==UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate->revision=before->revision+1U;*out=*candidate;return UMI_STATUS_OK;
}
UmiStatus UmiProviderTaskPrepare(const UmiProviderTaskContext *context,
    const UmiProviderConnection *connection,uint64_t connection_revision,UmiProviderTaskReceipt *out)
{
    if(out==NULL || !task_context_valid(context) || connection_revision==0U ||
        UmiProviderConnectionValidate(connection)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if(!connection->enabled) return UMI_STATUS_PERMISSION_DENIED;
    UmiProviderTaskReceipt candidate={0};
    candidate.schema=UMI_PROVIDER_TASK_SCHEMA;candidate.revision=1U;candidate.context=*context;
    candidate.connection_revision=connection_revision;candidate.phase=UMI_PROVIDER_TASK_PREPARED;
    memcpy(candidate.connection_id,connection->id,strlen(connection->id)+1U);
    memcpy(candidate.provider_id,connection->provider_id,strlen(connection->provider_id)+1U);
    UmiStatus status=UmiProviderConnectionFingerprint(connection,candidate.connection_fingerprint);
    if(status==UMI_STATUS_OK) *out=candidate;
    return status;
}
UmiStatus UmiProviderTaskAdvance(const UmiProviderTaskReceipt *receipt,uint64_t expected,
    UmiProviderTaskEvent event,UmiProviderTaskReceipt *out)
{
    UmiStatus status=task_review(receipt,expected,out);if(status!=UMI_STATUS_OK) return status;
    UmiProviderTaskPhase next;
    if(event==UMI_PROVIDER_TASK_BEGIN_SUBMISSION) {
        if(receipt->phase!=UMI_PROVIDER_TASK_PREPARED) return UMI_STATUS_INVALID_STATE;
        next=UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN;
    } else {
        if(event==UMI_PROVIDER_TASK_OBSERVE_RUNNING) next=UMI_PROVIDER_TASK_RUNNING;
        else if(event==UMI_PROVIDER_TASK_OBSERVE_SUCCESS) next=UMI_PROVIDER_TASK_SUCCEEDED;
        else if(event==UMI_PROVIDER_TASK_OBSERVE_FAILURE) next=UMI_PROVIDER_TASK_FAILED;
        else return UMI_STATUS_INVALID_ARGUMENT;
        if(receipt->phase==next) { *out=*receipt;return UMI_STATUS_OK; }
        if(receipt->phase!=UMI_PROVIDER_TASK_SUBMITTED && receipt->phase!=UMI_PROVIDER_TASK_RUNNING)
            return UMI_STATUS_INVALID_STATE;
    }
    UmiProviderTaskReceipt candidate=*receipt;candidate.phase=next;
    return task_commit(receipt,&candidate,out);
}
UmiStatus UmiProviderTaskConfirm(const UmiProviderTaskReceipt *receipt,uint64_t expected,
    const char *remote_id,UmiProviderTaskReceipt *out)
{
    UmiStatus status=task_review(receipt,expected,out);if(status!=UMI_STATUS_OK) return status;
    if(!task_remote_valid(remote_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if(receipt->phase==UMI_PROVIDER_TASK_SUBMITTED && strcmp(receipt->remote_id,remote_id)==0) {
        *out=*receipt;return UMI_STATUS_OK;
    }
    if(receipt->phase!=UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN) return UMI_STATUS_INVALID_STATE;
    UmiProviderTaskReceipt candidate=*receipt;candidate.phase=UMI_PROVIDER_TASK_SUBMITTED;
    memcpy(candidate.remote_id,remote_id,strlen(remote_id)+1U);
    return task_commit(receipt,&candidate,out);
}
UmiStatus UmiProviderTaskRecordOutput(const UmiProviderTaskReceipt *receipt,uint64_t expected,
    uint64_t bytes,const unsigned char sha256[UMI_SHA256_BYTES],UmiProviderTaskReceipt *out)
{
    UmiStatus status=task_review(receipt,expected,out);if(status!=UMI_STATUS_OK) return status;
    if(bytes==0U || sha256==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(receipt->phase==UMI_PROVIDER_TASK_OUTPUT_STORED && receipt->output_bytes==bytes &&
        memcmp(receipt->output_sha256,sha256,UMI_SHA256_BYTES)==0) { *out=*receipt;return UMI_STATUS_OK; }
    if(receipt->phase!=UMI_PROVIDER_TASK_SUCCEEDED) return UMI_STATUS_INVALID_STATE;
    UmiProviderTaskReceipt candidate=*receipt;candidate.phase=UMI_PROVIDER_TASK_OUTPUT_STORED;
    candidate.output_bytes=bytes;memcpy(candidate.output_sha256,sha256,UMI_SHA256_BYTES);
    return task_commit(receipt,&candidate,out);
}
UmiStatus UmiProviderTaskPlanRecovery(const UmiProviderTaskReceipt *receipt,
    const char *application_id,const char *profile_id,const UmiProviderConnection *connection,
    uint64_t connection_revision,UmiProviderTaskRecoveryAction *out_action)
{
    if(out_action==NULL || UmiProviderTaskValidate(receipt)!=UMI_STATUS_OK ||
        !UmiProviderConnectionIdValid(application_id,UMI_PROVIDER_CONNECTION_ID_CAPACITY) ||
        !UmiProviderConnectionIdValid(profile_id,UMI_PROVIDER_CONNECTION_ID_CAPACITY) ||
        UmiProviderConnectionValidate(connection)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(receipt->context.application_id,application_id)!=0 ||
        strcmp(receipt->context.profile_id,profile_id)!=0) return UMI_STATUS_INVALID_STATE;
    if(connection_revision!=receipt->connection_revision) return UMI_STATUS_BUSY;
    if(!connection->enabled) return UMI_STATUS_PERMISSION_DENIED;
    unsigned char fingerprint[UMI_SHA256_BYTES];
    UmiStatus status=UmiProviderConnectionFingerprint(connection,fingerprint);
    if(status!=UMI_STATUS_OK) return status;
    if(strcmp(receipt->connection_id,connection->id)!=0 || strcmp(receipt->provider_id,connection->provider_id)!=0 ||
        memcmp(receipt->connection_fingerprint,fingerprint,sizeof(fingerprint))!=0) return UMI_STATUS_INVALID_STATE;
    UmiProviderTaskRecoveryAction action;
    if(receipt->phase==UMI_PROVIDER_TASK_SUBMITTED || receipt->phase==UMI_PROVIDER_TASK_RUNNING)
        action=UMI_PROVIDER_TASK_POLL;
    else if(receipt->phase==UMI_PROVIDER_TASK_SUCCEEDED) action=UMI_PROVIDER_TASK_FETCH_OUTPUT;
    else return UMI_STATUS_INVALID_STATE;
    *out_action=action;return UMI_STATUS_OK;
}
