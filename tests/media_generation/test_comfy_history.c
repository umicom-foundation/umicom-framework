/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_history.c
 *
 * PURPOSE:
 *   Check selected history identity, complete response limits and untrusted output metadata without I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "comfy_recovery_fixture.h"
static const char successful[]="{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{}}}";
static const struct Case { const char *name,*json; UmiStatus status; UmiComfyHistoryState state; size_t count; } cases[] = {
    {"success-empty","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,0U},
    {"image","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,1U},
    {"source-order","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"9\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"output\"},{\"filename\":\"second.png\",\"subfolder\":\"scene\",\"type\":\"temp\"}]},\"x\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"input\"}]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,3U},
    {"unicode","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"café.png\",\"subfolder\":\"é\",\"type\":\"output\"}]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,1U},
    {"duplicate-image","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"output\"},{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,2U},
    {"opaque-path","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"../untrusted.png\",\"subfolder\":\"../../outside\",\"type\":\"output\"}]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,1U},
    {"missing","{}",UMI_STATUS_NOT_FOUND,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"null-status","{\"job-1\":{\"status\":null,\"outputs\":{}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"missing-status","{\"job-1\":{\"outputs\":{}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"incomplete","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":false},\"outputs\":{}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"failed","{\"job-1\":{\"status\":{\"status_str\":\"error\",\"completed\":false},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_FAILED,0U},
    {"contradiction","{\"job-1\":{\"status\":{\"status_str\":\"error\",\"completed\":true},\"outputs\":{}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"unknown-state","{\"job-1\":{\"status\":{\"status_str\":\"queued\",\"completed\":false},\"outputs\":{}}}",UMI_STATUS_NOT_IMPLEMENTED,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"wrong-completed","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":1},\"outputs\":{}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"missing-completed","{\"job-1\":{\"status\":{\"status_str\":\"success\"},\"outputs\":{}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"missing-outputs","{\"job-1\":{\"status\":null}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"wrong-outputs","{\"job-1\":{\"status\":null,\"outputs\":[]}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"wrong-node","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":null}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"wrong-images","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":{}}}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"wrong-image","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[false]}}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"empty-name","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"missing-subfolder","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"a.png\",\"type\":\"output\"}]}}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"unknown-folder","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"custom\"}]}}}}",UMI_STATUS_NOT_IMPLEMENTED,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"empty-node","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"long-node","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn\":{\"images\":[{\"filename\":\"frame.png\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_CAPACITY_EXCEEDED,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"max-name","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,1U},
    {"long-name","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[{\"filename\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\",\"subfolder\":\"scene\",\"type\":\"output\"}]}}}}",UMI_STATUS_CAPACITY_EXCEEDED,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"unknown-output","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"video\":[\"uninterpreted\"]}}}}",UMI_STATUS_OK,UMI_COMFY_HISTORY_SUCCEEDED,0U},
    {"entry-kind","{\"job-1\":null}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"root","[]",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"duplicate","{\"job-1\":{\"outputs\":{},\"status\":null},\"job-1\":{}}",UMI_STATUS_ALREADY_EXISTS,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"other-duplicate","{\"job-1\":{\"outputs\":{},\"status\":null},\"other\":{\"x\":1,\"x\":2}}",UMI_STATUS_ALREADY_EXISTS,UMI_COMFY_HISTORY_UNKNOWN,0U},
    {"trailing","{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{}}}{}",UMI_STATUS_PARSE_ERROR,UMI_COMFY_HISTORY_UNKNOWN,0U}
};
static int task_case(const char *mode)
{
    const char *json=successful; const char *id="job-1";
    UmiProviderTaskReceipt receipt,out,before;
    CHECK(recovery_task_fixture(UMI_PROVIDER_TASK_SUBMITTED,&receipt)==0);
    uint64_t revision=receipt.revision; UmiStatus expected=UMI_STATUS_OK;
    if(strcmp(mode,"task-success")==0) {}
    else if(strcmp(mode,"task-failed")==0) json="{\"job-1\":{\"status\":{\"status_str\":\"error\",\"completed\":false},\"outputs\":{}}}";
    else if(strcmp(mode,"task-stale")==0) { --revision; expected=UMI_STATUS_BUSY; }
    else if(strcmp(mode,"task-provider")==0) { strcpy(receipt.provider_id,"other"); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"task-operation")==0) { strcpy(receipt.context.operation_id,"other"); expected=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"task-unconfirmed")==0) { CHECK(recovery_task_fixture(UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN,&receipt)==0); revision=receipt.revision; expected=UMI_STATUS_INVALID_STATE; }
    else if(strcmp(mode,"task-other")==0) { id="other"; json="{\"other\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{}}}"; expected=UMI_STATUS_NOT_FOUND; }
    else if(strcmp(mode,"task-unknown")==0) { json="{\"job-1\":{\"status\":null,\"outputs\":{}}}"; expected=UMI_STATUS_UNAVAILABLE; }
    else if(strcmp(mode,"task-terminal")==0) { CHECK(UmiProviderTaskAdvance(&receipt,revision,UMI_PROVIDER_TASK_OBSERVE_FAILURE,&receipt)==UMI_STATUS_OK); revision=receipt.revision; expected=UMI_STATUS_INVALID_STATE; }
    else CHECK(0);
    UmiComfyHistory *history=NULL;
    CHECK(UmiComfyHistoryCreate(json,strlen(json),id,NULL,&history)==UMI_STATUS_OK);
    if(expected==UMI_STATUS_OK) {
        CHECK(UmiComfyTaskObserveHistory(&receipt,revision,history,&receipt)==UMI_STATUS_OK);
        CHECK(receipt.phase==(strcmp(mode,"task-failed")==0?UMI_PROVIDER_TASK_FAILED:UMI_PROVIDER_TASK_SUCCEEDED));
        CHECK(receipt.revision==revision+1 && receipt.output_bytes==0);
        revision=receipt.revision;
        CHECK(UmiComfyTaskObserveHistory(&receipt,revision,history,&receipt)==UMI_STATUS_OK);
        CHECK(receipt.revision==revision);
    } else {
        memset(&out,0xa5,sizeof(out)); memcpy(&before,&out,sizeof(out));
        CHECK(UmiComfyTaskObserveHistory(&receipt,revision,history,&out)==expected);
        CHECK(memcmp(&before,&out,sizeof(out))==0);
    }
    UmiComfyHistoryDestroy(history); return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *mode=argv[1];
    if(strncmp(mode,"task-",5)==0) return task_case(mode);
    UmiComfyHistory *history=NULL;
    if(strcmp(mode,"nulls")==0) {
        UmiComfyImageReference image;
        CHECK(UmiComfyHistoryCreate(NULL,0,"job-1",NULL,&history)==UMI_STATUS_INVALID_ARGUMENT && history==NULL);
        CHECK(UmiComfyHistoryCreate(successful,strlen(successful),NULL,NULL,&history)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyHistoryCreate(successful,strlen(successful),"",NULL,&history)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyHistoryCreate(successful,strlen(successful),"job-1",NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyHistoryImageAt(NULL,0,&image)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyHistoryStateOf(NULL)==UMI_COMFY_HISTORY_UNKNOWN && UmiComfyHistoryImageCount(NULL)==0);
        UmiComfyHistoryDestroy(NULL); return 0;
    }
    if(strcmp(mode,"cancel")==0) {
        UmiCancellationToken *cancel=NULL; CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiComfyHistoryCreate(successful,strlen(successful),"job-1",cancel,&history)==UMI_STATUS_CANCELLED && history==NULL);
        umi_cancellation_token_destroy(cancel); return 0;
    }
    if(strcmp(mode,"max-images")==0 || strcmp(mode,"too-many-images")==0) {
        size_t count=strcmp(mode,"max-images")==0?1024U:1025U;
        const char *prefix="{\"job-1\":{\"status\":{\"status_str\":\"success\",\"completed\":true},\"outputs\":{\"7\":{\"images\":[";
        const char *item="{\"filename\":\"frame.png\",\"subfolder\":\"\",\"type\":\"output\"}";
        size_t capacity=strlen(prefix)+count*(strlen(item)+1U)+16U;
        char *body=malloc(capacity); CHECK(body!=NULL); strcpy(body,prefix);
        for(size_t i=0;i<count;++i) { if(i) strcat(body,","); strcat(body,item); }
        strcat(body,"]}}}}");
        CHECK(UmiComfyHistoryCreate(body,strlen(body),"job-1",NULL,&history)==(count==1024U?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED));
        if(count==1024U) CHECK(UmiComfyHistoryImageCount(history)==count); else CHECK(history==NULL);
        free(body); UmiComfyHistoryDestroy(history); return 0;
    }
    if(strcmp(mode,"ownership")==0) {
        char body[512]; strcpy(body,cases[1].json);
        CHECK(UmiComfyHistoryCreate(body,strlen(body),"job-1",NULL,&history)==UMI_STATUS_OK);
        memset(body,0,sizeof(body)); UmiComfyImageReference image;
        CHECK(UmiComfyHistoryImageAt(history,0,&image)==UMI_STATUS_OK && strcmp(image.filename,"frame.png")==0);
        UmiComfyHistoryDestroy(history); CHECK(strcmp(image.subfolder,"scene")==0); return 0;
    }
    const struct Case *test=NULL;
    for(size_t i=0;i<sizeof(cases)/sizeof(cases[0]);++i) if(strcmp(mode,cases[i].name)==0) {test=&cases[i];break;}
    CHECK(test!=NULL);
    CHECK(UmiComfyHistoryCreate(test->json,strlen(test->json),"job-1",NULL,&history)==test->status);
    if(test->status!=UMI_STATUS_OK) { CHECK(history==NULL); return 0; }
    CHECK(UmiComfyHistoryStateOf(history)==test->state && UmiComfyHistoryImageCount(history)==test->count);
    UmiComfyImageReference image,before; memset(&image,0xa5,sizeof(image)); memcpy(&before,&image,sizeof(image));
    CHECK(UmiComfyHistoryImageAt(history,test->count,&image)==UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&image,&before,sizeof(image))==0);
    if(test->count>0) {
        CHECK(UmiComfyHistoryImageAt(history,0,&image)==UMI_STATUS_OK);
        if(strcmp(mode,"opaque-path")==0) CHECK(strcmp(image.filename,"../untrusted.png")==0);
        if(strcmp(mode,"source-order")==0) {
            CHECK(strcmp(image.node_id,"9")==0 && image.folder==UMI_COMFY_IMAGE_OUTPUT);
            CHECK(UmiComfyHistoryImageAt(history,1,&image)==UMI_STATUS_OK && image.folder==UMI_COMFY_IMAGE_TEMP && strcmp(image.filename,"second.png")==0);
            CHECK(UmiComfyHistoryImageAt(history,2,&image)==UMI_STATUS_OK && image.folder==UMI_COMFY_IMAGE_INPUT && strcmp(image.node_id,"x")==0);
        }
        if(strcmp(mode,"max-name")==0) CHECK(strlen(image.filename)==512U);
    }
    UmiComfyHistoryDestroy(history); return 0;
}
