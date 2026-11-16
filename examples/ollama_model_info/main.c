/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/ollama_model_info/main.c
 *
 * PURPOSE:
 *   Use the installed SDK to distinguish declared model metadata from unknown or runtime-qualified capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_model_info.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
int main(void)
{
    const char sample[]="{\"details\":{\"format\":\"gguf\",\"family\":\"example\"},"
        "\"model_info\":{\"general.architecture\":\"example\",\"example.context_length\":8192},"
        "\"capabilities\":[\"completion\",\"future-feature\"]}";
    UmiOllamaModelInfo *info=NULL;
    UmiStatus status=UmiOllamaModelInfoCreate("synthetic:model",sample,strlen(sample),NULL,&info);
    UmiOllamaModelInfoSummary summary;bool declared=false;
    if(status==UMI_STATUS_OK)status=UmiOllamaModelInfoInspect(info,&summary);
    if(status==UMI_STATUS_OK)status=UmiOllamaModelInfoDeclares(info,"completion",&declared);
    if(status==UMI_STATUS_OK && (!declared || !(summary.known&UMI_OLLAMA_MODEL_CONTEXT_LENGTH)))
        status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK)
        printf("Observed context metadata: %" PRIu64 "; completion declared by synthetic input.\n",summary.context_length);
    UmiOllamaModelInfoDestroy(info);info=NULL;
    if(status!=UMI_STATUS_OK)return 1;
    /* Missing metadata stays unknown. A false result from a known list has a
     * different meaning; neither result measures actual inference support. */
    status=UmiOllamaModelInfoCreate("synthetic:model","{}",2U,NULL,&info);
    declared=true;
    if(status==UMI_STATUS_OK && (UmiOllamaModelInfoDeclares(info,"completion",&declared)!=UMI_STATUS_NOT_FOUND || !declared))
        status=UMI_STATUS_INVALID_STATE;
    UmiOllamaModelInfoDestroy(info);info=NULL;
    if(status!=UMI_STATUS_OK)return 1;
    const char empty[]="{\"capabilities\":[]}";
    status=UmiOllamaModelInfoCreate("synthetic:model",empty,strlen(empty),NULL,&info);
    if(status==UMI_STATUS_OK)status=UmiOllamaModelInfoDeclares(info,"completion",&declared);
    if(status==UMI_STATUS_OK && declared)status=UMI_STATUS_INVALID_STATE;
    UmiOllamaModelInfoDestroy(info);
    if(status!=UMI_STATUS_OK)return 1;
    puts("Missing and explicitly empty capability lists remain distinct. No provider was contacted.");
    return fflush(stdout)==0 && !ferror(stdout)?0:1;
}
