/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Opaque plan ownership and length-delimited review fingerprints. No pointer
 * into caller storage survives PlanCreate. No old script is invoked here. */
#include "internal.h"
#include <stdlib.h>
#include <string.h>
void UmiNativeReportInit(UmiNativeStageReport *report) { if(report!=NULL) *report=(UmiNativeStageReport){0}; }
static void CopyDetail(char *out,size_t capacity,const char *value)
{
    size_t n=strlen(value); if(n>=capacity)n=capacity-1U; memcpy(out,value,n);out[n]='\0';
}
UmiStatus UmiNativeReportSet(UmiNativeStageReport *report,UmiStatus status,const char *phase,const char *detail,int systemError)
{
    if(report!=NULL) {
        report->status=status;report->systemError=systemError;
        CopyDetail(report->phase,sizeof report->phase,phase);CopyDetail(report->detail,sizeof report->detail,detail);
    }
    return status;
}
const char *UmiNativeStageStatusText(UmiStatus status)
{
    switch(status) {
        case UMI_STATUS_OK:return "completed";
        case UMI_STATUS_INVALID_ARGUMENT:return "invalid input";
        case UMI_STATUS_INVALID_STATE:return "invalid state";
        case UMI_STATUS_CAPACITY_EXCEEDED:return "capacity exceeded";
        case UMI_STATUS_NOT_FOUND:return "not found";
        case UMI_STATUS_ALREADY_EXISTS:return "already exists";
        case UMI_STATUS_OUT_OF_MEMORY:return "out of memory";
        case UMI_STATUS_PERMISSION_DENIED:return "permission denied";
        case UMI_STATUS_IO_ERROR:return "input/output error";
        case UMI_STATUS_PARSE_ERROR:return "invalid format";
        case UMI_STATUS_UNAVAILABLE:return "not available on this host";
        case UMI_STATUS_BUSY:return "input changed; review again";
        default:return "not completed";
    }
}
void UmiNativeStagePlanDestroy(UmiNativeStagePlan *plan)
{
    if(plan==NULL)return;
    free(plan->icon);free(plan->entry);free(plan);
}
const UmiNativeStageManifest *UmiNativeStagePlanManifest(const UmiNativeStagePlan *plan) {return plan==NULL?NULL:&plan->manifest;}
const char *UmiNativeStagePlanEntry(const UmiNativeStagePlan *plan) {return plan==NULL?NULL:plan->entry;}
const char *UmiNativeStagePlanFingerprint(const UmiNativeStagePlan *plan) {return plan==NULL?NULL:plan->fingerprint;}
UmiStatus UmiNativePlanFingerprint(UmiNativeStagePlan *plan)
{
    if(plan==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    char *text=malloc(UMI_NATIVE_STAGE_TEXT_CAPACITY);size_t length=0U;
    if(text==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status=UmiNativeStageManifestWrite(&plan->manifest,text,UMI_NATIVE_STAGE_TEXT_CAPACITY,&length);
    if(status==UMI_STATUS_OK)status=UmiNativeSha256Buffer(text,length,plan->fingerprint);
    free(text);return status;
}
