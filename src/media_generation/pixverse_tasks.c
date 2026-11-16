/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/pixverse_tasks.c
 *
 * PURPOSE:
 *   Connect the existing PixVerse request owner to reusable task identity, transitions and persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "pixverse_internal.h"
#include "umicom/media_generation/pixverse_tasks.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static UmiStatus task_identity_copy(char *out,size_t capacity,const char *text)
{
    if(text==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;
    while(length<capacity && text[length]!='\0') ++length;
    if(length==capacity) return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(out,text,length+1U);return UMI_STATUS_OK;
}
static UmiStatus task_connection(const UmiProviderConnection *connection)
{
    UmiStatus status=UmiProviderConnectionValidate(connection);
    if(status!=UMI_STATUS_OK) return status;
    if(strcmp(connection->provider_id,"pixverse")!=0 ||
        connection->route!=UMI_PROVIDER_CONNECTION_HTTPS ||
        strcmp(connection->endpoint,UMI_PIXVERSE_ORIGIN)!=0 || strcmp(connection->model,"v6")!=0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/* Canonical request fields come from the provider owner, not a second encoder.
 * The domain and separators make the binding distinct from connection hashes.
 * Trace identity participates because replacing a creation trace can create a
 * different remote job even when the visible prompt has not changed. */
static UmiStatus task_request_digest(const UmiPixVersePlan *plan,unsigned char out[UMI_SHA256_BYTES])
{
    static const char domain[]="Umicom PixVerse request fingerprint v1\n";
    UmiSha256 hash;UmiSha256Init(&hash);
    UmiStatus status=UmiSha256Update(&hash,domain,sizeof(domain)-1U);
    const char *fields[]={plan->url,plan->request_key,plan->body};
    for(size_t i=0U;status==UMI_STATUS_OK && i<3U;++i) {
        status=UmiSha256Update(&hash,fields[i],strlen(fields[i]));
        if(status==UMI_STATUS_OK && i!=2U) status=UmiSha256Update(&hash,"\n",1U);
    }
    return status==UMI_STATUS_OK?UmiSha256Final(&hash,out):status;
}
static UmiStatus task_receipt(const UmiProviderTaskReceipt *receipt)
{
    UmiStatus status=UmiProviderTaskValidate(receipt);if(status!=UMI_STATUS_OK) return status;
    return strcmp(receipt->provider_id,"pixverse")==0 &&
        strcmp(receipt->context.operation_id,UMI_PIXVERSE_TASK_OPERATION)==0?
        UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
/* Do not round through floating point or accept an alternate spelling. A
 * canonical decimal ID is the only data allowed into the fixed status route. */
static UmiStatus task_video_id(const char *text,int64_t *out)
{
    if(text[0]<'1' || text[0]>'9') return UMI_STATUS_INVALID_STATE;
    uint64_t number=0U;
    for(size_t i=0U;text[i]!='\0';++i) {
        if(text[i]<'0' || text[i]>'9') return UMI_STATUS_INVALID_STATE;
        unsigned digit=(unsigned)(text[i]-'0');
        if(number>((uint64_t)INT64_MAX-digit)/10U) return UMI_STATUS_INVALID_STATE;
        number=number*10U+digit;
    }
    *out=(int64_t)number;return UMI_STATUS_OK;
}
UmiStatus UmiPixVerseTaskPrepare(const UmiPixVersePlan *plan,const char *application_id,
    const char *profile_id,const char *local_id,const UmiProviderConnection *connection,
    uint64_t connection_revision,UmiProviderTaskReceipt *out)
{
    if(plan==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(!plan->creates_resource || plan->consumed) return UMI_STATUS_INVALID_STATE;
    UmiStatus status=task_connection(connection);if(status!=UMI_STATUS_OK) return status;
    UmiProviderTaskContext context={0};
    status=task_identity_copy(context.application_id,sizeof(context.application_id),application_id);
    if(status==UMI_STATUS_OK) status=task_identity_copy(context.profile_id,sizeof(context.profile_id),profile_id);
    if(status==UMI_STATUS_OK) status=task_identity_copy(context.local_id,sizeof(context.local_id),local_id);
    strcpy(context.operation_id,UMI_PIXVERSE_TASK_OPERATION);
    if(status==UMI_STATUS_OK) status=task_request_digest(plan,context.request_sha256);
    if(status==UMI_STATUS_OK) status=UmiProviderTaskPrepare(&context,connection,connection_revision,out);
    return status;
}
UmiStatus UmiPixVerseTaskObserve(const UmiProviderTaskReceipt *receipt,uint64_t expected,
    const UmiPixVersePlan *plan,UmiStatus execution_status,const UmiPixVerseResult *result,
    UmiProviderTaskReceipt *out)
{
    if(plan==NULL || result==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=task_receipt(receipt);if(status!=UMI_STATUS_OK) return status;
    if(receipt->revision!=expected) return UMI_STATUS_BUSY;
    if(execution_status!=UMI_STATUS_OK) return execution_status;
    if(!plan->consumed || result->http_status!=200U || result->video_id<=0)
        return UMI_STATUS_INVALID_STATE;
    if(plan->creates_resource) {
        unsigned char digest[UMI_SHA256_BYTES];status=task_request_digest(plan,digest);
        if(status!=UMI_STATUS_OK) return status;
        if(memcmp(digest,receipt->context.request_sha256,sizeof(digest))!=0 ||
            !result->creation_confirmed || !result->creation_may_exist || result->state!=0U ||
            result->video_url[0]!='\0') return UMI_STATUS_INVALID_STATE;
        char identity[32];int count=snprintf(identity,sizeof(identity),"%" PRId64,result->video_id);
        if(count<0 || (size_t)count>=sizeof(identity)) return UMI_STATUS_CAPACITY_EXCEEDED;
        return UmiProviderTaskConfirm(receipt,expected,identity,out);
    }
    int64_t identity=0;status=task_video_id(receipt->remote_id,&identity);
    if(status!=UMI_STATUS_OK) return status;
    if(identity!=plan->video_id || identity!=result->video_id || result->creation_confirmed ||
        result->creation_may_exist) return UMI_STATUS_INVALID_STATE;
    UmiProviderTaskEvent event;
    if(result->state==5U) event=UMI_PROVIDER_TASK_OBSERVE_RUNNING;
    else if(result->state==1U) event=UMI_PROVIDER_TASK_OBSERVE_SUCCESS;
    else if(result->state==6U || result->state==7U || result->state==8U) event=UMI_PROVIDER_TASK_OBSERVE_FAILURE;
    else return UMI_STATUS_INVALID_STATE;
    return UmiProviderTaskAdvance(receipt,expected,event,out);
}
UmiStatus UmiPixVerseTaskRecordResult(UmiProviderTasks *store,const char *local_id,
    uint64_t expected,const UmiPixVersePlan *plan,UmiStatus execution_status,
    const UmiPixVerseResult *result,UmiProviderTaskReceipt *out)
{
    if(store==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderTaskReceipt current,candidate;
    UmiStatus status=UmiProviderTasksRead(store,local_id,&current);
    if(status==UMI_STATUS_OK) status=UmiPixVerseTaskObserve(&current,expected,plan,execution_status,result,&candidate);
    if(status!=UMI_STATUS_OK) return status;
    /* Reuse transactional compare-revision writes. Reading for interpretation
     * never grants permission to overwrite a record another worker changed. */
    if(plan->creates_resource) return UmiProviderTasksConfirm(store,local_id,expected,candidate.remote_id,out);
    UmiProviderTaskEvent event=candidate.phase==UMI_PROVIDER_TASK_RUNNING?UMI_PROVIDER_TASK_OBSERVE_RUNNING:
        candidate.phase==UMI_PROVIDER_TASK_SUCCEEDED?UMI_PROVIDER_TASK_OBSERVE_SUCCESS:UMI_PROVIDER_TASK_OBSERVE_FAILURE;
    return UmiProviderTasksAdvance(store,local_id,expected,event,out);
}
UmiStatus UmiPixVerseTaskStatusPlan(const UmiProviderTaskReceipt *receipt,
    const char *application_id,const char *profile_id,const UmiProviderConnection *connection,
    uint64_t connection_revision,const char *trace_id,UmiPixVersePlan **out)
{
    if(out==NULL || *out!=NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=task_receipt(receipt);if(status!=UMI_STATUS_OK) return status;
    status=task_connection(connection);if(status!=UMI_STATUS_OK) return status;
    UmiProviderTaskRecoveryAction action;
    status=UmiProviderTaskPlanRecovery(receipt,application_id,profile_id,connection,connection_revision,&action);
    if(status!=UMI_STATUS_OK) return status;
    if(action!=UMI_PROVIDER_TASK_POLL) return UMI_STATUS_INVALID_STATE;
    UmiPixVerseDraft draft={0};draft.operation=UMI_PIXVERSE_GET_VIDEO;
    status=task_video_id(receipt->remote_id,&draft.video_id);
    if(status==UMI_STATUS_OK) status=task_identity_copy(draft.trace_id,sizeof(draft.trace_id),trace_id);
    return status==UMI_STATUS_OK?UmiPixVersePrepare(&draft,out):status;
}
