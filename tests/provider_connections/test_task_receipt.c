/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_task_receipt.c
 *
 * PURPOSE:
 *   Exercise remote task uncertainty, staged revision transitions and recovery binding without network effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include "umicom/provider_connections/tasks.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static UmiProviderConnection task_connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"video");strcpy(c.provider_id,"example");
    strcpy(c.label,"Example video");strcpy(c.endpoint,"https://api.example.invalid/tasks");
    strcpy(c.model,"video-model");strcpy(c.secret_reference,"vault:video-key");
    c.route=UMI_PROVIDER_CONNECTION_HTTPS;c.timeout_ms=30000U;c.enabled=true;return c;
}
static UmiProviderTaskContext task_context(void)
{
    UmiProviderTaskContext c={0};strcpy(c.application_id,"media");strcpy(c.profile_id,"owner");
    strcpy(c.local_id,"scene-one");strcpy(c.operation_id,"video.generate");c.request_sha256[0]=42U;return c;
}
static UmiProviderTaskReceipt fixture(UmiProviderTaskPhase phase)
{
    UmiProviderTaskReceipt r;UmiProviderConnection c=task_connection();UmiProviderTaskContext context=task_context();
    assert(UmiProviderTaskPrepare(&context,&c,7U,&r)==UMI_STATUS_OK);
    /* Construct consistent state values for exhaustive transition admission;
     * lifecycle below reaches the same states through the public operations. */
    r.phase=phase;r.revision=5U;
    if(phase>=UMI_PROVIDER_TASK_SUBMITTED) strcpy(r.remote_id,"Remote-123");
    if(phase==UMI_PROVIDER_TASK_OUTPUT_STORED) { r.output_bytes=42U;r.output_sha256[0]=9U; }
    assert(UmiProviderTaskValidate(&r)==UMI_STATUS_OK);return r;
}
static void untouched(const UmiProviderTaskReceipt *out,const unsigned char *before)
{ assert(memcmp(out,before,sizeof(*out))==0); }
static int transition_matrix(void)
{
    const int next[7][4]={{2,0,0,0},{0,0,0,0},{0,4,5,6},{0,4,5,6},{0,0,5,0},{0,0,0,6},{0,0,0,0}};
    for(unsigned phase=1U;phase<=7U;++phase) for(unsigned event=1U;event<=4U;++event) {
        UmiProviderTaskReceipt r=fixture((UmiProviderTaskPhase)phase),out;
        memset(&out,0xa5,sizeof(out));unsigned char before[sizeof(out)];memcpy(before,&out,sizeof(out));
        UmiStatus status=UmiProviderTaskAdvance(&r,r.revision,(UmiProviderTaskEvent)event,&out);
        if(next[phase-1U][event-1U]==0) { assert(status==UMI_STATUS_INVALID_STATE);untouched(&out,before); }
        else {
            assert(status==UMI_STATUS_OK && out.phase==(UmiProviderTaskPhase)next[phase-1U][event-1U]);
            assert(out.revision==r.revision+(out.phase!=r.phase?1U:0U));
            assert(memcmp(out.connection_fingerprint,r.connection_fingerprint,UMI_SHA256_BYTES)==0);
            assert(out.context.request_sha256[0]==42U && strcmp(out.context.local_id,"scene-one")==0);
            assert(UmiProviderTaskValidate(&out)==UMI_STATUS_OK);
        }
    }
    return 0;
}
int main(int argc,char **argv)
{
    assert(argc==2);const char *mode=argv[1];
    UmiProviderConnection c=task_connection();UmiProviderTaskContext context=task_context();
    UmiProviderTaskReceipt r=fixture(UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN),out;
    memset(&out,0xa5,sizeof(out));unsigned char before[sizeof(out)];memcpy(before,&out,sizeof(out));
    if(strcmp(mode,"transition-matrix")==0) return transition_matrix();
    if(strcmp(mode,"lifecycle")==0) {
        assert(UmiProviderTaskPrepare(&context,&c,7U,&r)==UMI_STATUS_OK && r.revision==1U);
        assert(UmiProviderTaskAdvance(&r,1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&r)==UMI_STATUS_OK);
        UmiProviderTaskRecoveryAction action=(UmiProviderTaskRecoveryAction)99;
        assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,&action)==UMI_STATUS_INVALID_STATE);
        assert(action==(UmiProviderTaskRecoveryAction)99);
        assert(UmiProviderTaskConfirm(&r,2U,"Remote-123",&r)==UMI_STATUS_OK);
        assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,&action)==UMI_STATUS_OK && action==UMI_PROVIDER_TASK_POLL);
        assert(UmiProviderTaskAdvance(&r,3U,UMI_PROVIDER_TASK_OBSERVE_RUNNING,&r)==UMI_STATUS_OK);
        assert(UmiProviderTaskAdvance(&r,4U,UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&r)==UMI_STATUS_OK);
        assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,&action)==UMI_STATUS_OK && action==UMI_PROVIDER_TASK_FETCH_OUTPUT);
        unsigned char digest[UMI_SHA256_BYTES]={9U};
        assert(UmiProviderTaskRecordOutput(&r,5U,42U,digest,&r)==UMI_STATUS_OK && r.revision==6U);
        assert(UmiProviderTaskRecordOutput(&r,6U,42U,r.output_sha256,&r)==UMI_STATUS_OK && r.revision==6U);
        assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,&action)==UMI_STATUS_INVALID_STATE);
        return 0;
    }
    if(strcmp(mode,"prepare-refusals")==0) {
        c.enabled=false;assert(UmiProviderTaskPrepare(&context,&c,7U,&out)==UMI_STATUS_PERMISSION_DENIED);untouched(&out,before);
        c.enabled=true;assert(UmiProviderTaskPrepare(&context,&c,0U,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        context.application_id[0]='\0';assert(UmiProviderTaskPrepare(&context,&c,7U,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        assert(UmiProviderTaskPrepare(NULL,&c,7U,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        context=task_context();assert(UmiProviderTaskPrepare(&context,NULL,7U,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        assert(UmiProviderTaskPrepare(&context,&c,7U,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    if(strcmp(mode,"confirmation")==0) {
        assert(UmiProviderTaskConfirm(&r,5U,"Remote-123",&r)==UMI_STATUS_OK && r.revision==6U);
        assert(UmiProviderTaskConfirm(&r,6U,r.remote_id,&r)==UMI_STATUS_OK && r.revision==6U);
        assert(UmiProviderTaskConfirm(&r,6U,"different",&out)==UMI_STATUS_INVALID_STATE);untouched(&out,before);
        assert(UmiProviderTaskConfirm(&r,5U,"Remote-123",&out)==UMI_STATUS_BUSY);untouched(&out,before);return 0;
    }
    if(strcmp(mode,"remote-id-bounds")==0) {
        const char *bad[]={NULL,""," bad","_bad","https://host.invalid/id","id with space","caf\xc3\xa9"};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) { assert(UmiProviderTaskConfirm(&r,5U,bad[i],&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before); }
        char id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];memset(id,'a',sizeof(id));
        assert(UmiProviderTaskConfirm(&r,5U,id,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        id[sizeof(id)-1U]='\0';assert(UmiProviderTaskConfirm(&r,5U,id,&out)==UMI_STATUS_OK);return 0;
    }
    if(strcmp(mode,"recovery-matrix")==0) {
        for(unsigned phase=1U;phase<=7U;++phase) {
            r=fixture((UmiProviderTaskPhase)phase);UmiProviderTaskRecoveryAction action=(UmiProviderTaskRecoveryAction)99;
            UmiStatus status=UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,&action);
            if(phase==3U || phase==4U) assert(status==UMI_STATUS_OK && action==UMI_PROVIDER_TASK_POLL);
            else if(phase==5U) assert(status==UMI_STATUS_OK && action==UMI_PROVIDER_TASK_FETCH_OUTPUT);
            else assert(status==UMI_STATUS_INVALID_STATE && action==(UmiProviderTaskRecoveryAction)99);
        }return 0;
    }
    if(strcmp(mode,"recovery-binding")==0) {
        r=fixture(UMI_PROVIDER_TASK_SUBMITTED);UmiProviderTaskRecoveryAction action=(UmiProviderTaskRecoveryAction)99;
        assert(UmiProviderTaskPlanRecovery(&r,"studio","owner",&c,7U,&action)==UMI_STATUS_INVALID_STATE);
        assert(UmiProviderTaskPlanRecovery(&r,"media","other",&c,7U,&action)==UMI_STATUS_INVALID_STATE);
        assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,8U,&action)==UMI_STATUS_BUSY);
        c.enabled=false;assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,&action)==UMI_STATUS_PERMISSION_DENIED);
        c.enabled=true;strcpy(c.model,"changed");assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,&action)==UMI_STATUS_INVALID_STATE);
        assert(action==(UmiProviderTaskRecoveryAction)99);return 0;
    }
    if(strcmp(mode,"output-refusals")==0) {
        unsigned char digest[UMI_SHA256_BYTES]={9U};
        assert(UmiProviderTaskRecordOutput(&r,5U,42U,digest,&out)==UMI_STATUS_INVALID_STATE);untouched(&out,before);
        r=fixture(UMI_PROVIDER_TASK_SUCCEEDED);
        assert(UmiProviderTaskRecordOutput(&r,5U,0U,digest,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        assert(UmiProviderTaskRecordOutput(&r,5U,42U,NULL,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        r=fixture(UMI_PROVIDER_TASK_OUTPUT_STORED);digest[0]=10U;
        assert(UmiProviderTaskRecordOutput(&r,5U,42U,digest,&out)==UMI_STATUS_INVALID_STATE);untouched(&out,before);return 0;
    }
    if(strcmp(mode,"stale-and-overflow")==0) {
        r=fixture(UMI_PROVIDER_TASK_SUBMITTED);
        assert(UmiProviderTaskAdvance(&r,4U,UMI_PROVIDER_TASK_OBSERVE_RUNNING,&out)==UMI_STATUS_BUSY);untouched(&out,before);
        r.revision=UINT64_MAX;
        assert(UmiProviderTaskAdvance(&r,UINT64_MAX,UMI_PROVIDER_TASK_OBSERVE_RUNNING,&out)==UMI_STATUS_CAPACITY_EXCEEDED);untouched(&out,before);
        r.phase=UMI_PROVIDER_TASK_RUNNING;
        assert(UmiProviderTaskAdvance(&r,UINT64_MAX,UMI_PROVIDER_TASK_OBSERVE_RUNNING,&r)==UMI_STATUS_OK && r.revision==UINT64_MAX);return 0;
    }
    if(strcmp(mode,"borrowed-inputs")==0) {
        out=fixture(UMI_PROVIDER_TASK_SUBMITTED);
        assert(UmiProviderTaskConfirm(&r,5U,out.remote_id,&out)==UMI_STATUS_OK);
        assert(strcmp(out.remote_id,"Remote-123")==0);
        assert(UmiProviderTaskPrepare(&out.context,&c,7U,&out)==UMI_STATUS_OK && strcmp(out.context.local_id,"scene-one")==0);
        r=fixture(UMI_PROVIDER_TASK_SUCCEEDED);out=fixture(UMI_PROVIDER_TASK_OUTPUT_STORED);
        assert(UmiProviderTaskRecordOutput(&r,5U,42U,out.output_sha256,&out)==UMI_STATUS_OK && out.output_sha256[0]==9U);return 0;
    }
    if(strcmp(mode,"receipt-invariants")==0) {
        for(unsigned i=0U;i<12U;++i) {
            r=fixture(UMI_PROVIDER_TASK_SUBMITTED);
            switch(i) {
                case 0:r.schema=0U;break;case 1:r.revision=0U;break;case 2:r.connection_revision=0U;break;
                case 3:r.phase=(UmiProviderTaskPhase)99;break;case 4:r.remote_id[0]='\0';break;
                case 5:r.output_bytes=1U;break;case 6:r.output_sha256[0]=1U;break;
                case 7:r.context.local_id[0]='\0';break;case 8:r.connection_id[0]='\0';break;
                case 9:r.phase=UMI_PROVIDER_TASK_PREPARED;break;
                case 10:r.phase=UMI_PROVIDER_TASK_OUTPUT_STORED;break;
                case 11:memset(r.remote_id,'x',sizeof(r.remote_id));break;
            }
            assert(UmiProviderTaskValidate(&r)==UMI_STATUS_INVALID_ARGUMENT);
            assert(UmiProviderTaskAdvance(&r,r.revision,UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        }return 0;
    }
    if(strcmp(mode,"null-and-unknown")==0) {
        assert(UmiProviderTaskValidate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiProviderTaskAdvance(NULL,5U,UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        assert(UmiProviderTaskAdvance(&r,5U,(UmiProviderTaskEvent)99,&out)==UMI_STATUS_INVALID_ARGUMENT);untouched(&out,before);
        assert(UmiProviderTaskConfirm(&r,5U,"Remote-123",NULL)==UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiProviderTaskPlanRecovery(&r,"media","owner",&c,7U,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
    return 2;
}
