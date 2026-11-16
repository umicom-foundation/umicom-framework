/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/ollama_pull_event.c
 *
 * PURPOSE:
 *   Decode provider pull status with shared strict JSON ownership and exact byte counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_pull.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/security/secrets.h"
#include <string.h>

static UmiStatus PullText(const UmiJsonTree *tree,const char *name,char *out,size_t capacity)
{
    int node=-1;UmiStatus status=UmiJsonTreeMember(tree,0,name,&node);
    if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
    status=UmiJsonTreeText(tree,node,out,capacity);
    if(status==UMI_STATUS_OK && out[0]=='\0')return UMI_STATUS_PARSE_ERROR;
    return status;
}
static UmiStatus PullNumber(const UmiJsonTree *tree,const char *name,uint64_t *out,
    uint32_t bit,uint32_t *known)
{
    int node=-1;UmiStatus status=UmiJsonTreeMember(tree,0,name,&node);
    if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    int64_t number=0;status=UmiJsonTreeInteger(tree,node,&number);
    if(status!=UMI_STATUS_OK)return status;
    if(number<0)return UMI_STATUS_PARSE_ERROR;
    *out=(uint64_t)number;*known|=bit;return UMI_STATUS_OK;
}
UmiStatus UmiOllamaPullEventDecode(const void *bytes,size_t length,
    const UmiCancellationToken *cancel,UmiOllamaPullEvent *out)
{
    if(bytes==NULL || length==0U || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(length>UMI_OLLAMA_PULL_LINE_BYTES)return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiJsonTreeLimits limits={UMI_OLLAMA_PULL_LINE_BYTES,512U,16U};
    UmiJsonTree *tree=NULL;UmiOllamaPullEvent event={0};
    UmiStatus status=UmiJsonTreeCreate(bytes,length,&limits,cancel,&tree);
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,0)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT){status=UMI_STATUS_PARSE_ERROR;goto done;}
    status=UmiJsonTreeValidateUniqueKeys(tree,0,cancel);
    if(status!=UMI_STATUS_OK)goto done;
    status=PullText(tree,"status",event.status,sizeof event.status);if(status!=UMI_STATUS_OK)goto done;
    status=PullText(tree,"error",event.error,sizeof event.error);if(status!=UMI_STATUS_OK)goto done;
    status=PullText(tree,"digest",event.digest,sizeof event.digest);if(status!=UMI_STATUS_OK)goto done;
    status=PullNumber(tree,"total",&event.total,UMI_OLLAMA_PULL_TOTAL_KNOWN,&event.known);if(status!=UMI_STATUS_OK)goto done;
    status=PullNumber(tree,"completed",&event.completed,UMI_OLLAMA_PULL_COMPLETED_KNOWN,&event.known);if(status!=UMI_STATUS_OK)goto done;
    if((event.status[0]=='\0' && event.error[0]=='\0') ||
        (event.known==(UMI_OLLAMA_PULL_TOTAL_KNOWN|UMI_OLLAMA_PULL_COMPLETED_KNOWN) && event.completed>event.total)){
        status=UMI_STATUS_PARSE_ERROR;goto done;
    }
    /* A server can report failure after HTTP headers were accepted. Error text
     * takes precedence over status; never promote a contradictory success field. */
    event.kind=event.error[0]!='\0'?UMI_OLLAMA_PULL_ERROR:
        strcmp(event.status,"success")==0?UMI_OLLAMA_PULL_SUCCESS:UMI_OLLAMA_PULL_STATUS;
    if(umi_cancellation_token_is_requested(cancel)){status=UMI_STATUS_CANCELLED;goto done;}
    *out=event;
done:
    umi_secret_clear(&event,sizeof event);UmiJsonTreeDestroy(tree);return status;
}
