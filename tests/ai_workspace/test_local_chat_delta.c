/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_chat_delta.c
 *
 * PURPOSE:
 *   Verify text-only streamed completion admission and unchanged delta output on malformed or unsupported chunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
typedef struct Fixture {
    const char *name,*json;UmiStatus status;const char *text;
    int content,role,finish,usage;
} Fixture;
static const Fixture fixtures[]={
    {"content","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_OK,"Hello",1,0,0,0},
    {"unicode","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Café € 🌍\"},\"finish_reason\":null}]}",UMI_STATUS_OK,"Café € 🌍",1,0,0,0},
    {"empty-content","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"\"},\"finish_reason\":null}]}",UMI_STATUS_OK,"",1,0,0,0},
    {"role","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"role\":\"assistant\"},\"finish_reason\":null}]}",UMI_STATUS_OK,"",0,1,0,0},
    {"null-content","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":null},\"finish_reason\":null}]}",UMI_STATUS_OK,"",0,0,0,0},
    {"empty-delta","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{},\"finish_reason\":null}]}",UMI_STATUS_OK,"",0,0,0,0},
    {"stop","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":\"stop\"}]}",UMI_STATUS_OK,"Hello",1,0,1,0},
    {"length","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":\"length\"}]}",UMI_STATUS_OK,"Hello",1,0,2,0},
    {"usage","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}],\"usage\":{\"prompt_tokens\":2,\"completion_tokens\":3,\"total_tokens\":5}}",UMI_STATUS_OK,"Hello",1,0,0,1},
    {"usage-only","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[],\"usage\":{\"prompt_tokens\":2,\"completion_tokens\":3,\"total_tokens\":5}}",UMI_STATUS_OK,"",0,0,0,1},
    {"null-usage","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}],\"usage\":null}",UMI_STATUS_OK,"Hello",1,0,0,0},
    {"null-refusal","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"refusal\":null},\"finish_reason\":null}]}",UMI_STATUS_OK,"Hello",1,0,0,0},
    {"wrong-model","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"other\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"missing-model","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"missing-id","{\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"empty-id","{\"id\":\"\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"wrong-object","{\"id\":\"local-1\",\"object\":\"chat.completion\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"empty-choices","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"multiple-choices","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null},{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"choice-index","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":1,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"negative-index","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":-1,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"fractional-index","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0.5,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"missing-index","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"missing-delta","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"array-delta","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":[],\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"wrong-role","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"role\":\"system\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"null-role","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"role\":null},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"array-content","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":[]},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"nul-content","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"a\\u0000b\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"control-content","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"a\\u001bb\"},\"finish_reason\":null}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"missing-finish","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"}}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"unknown-finish","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":\"content_filter\"}]}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"usage-mismatch","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}],\"usage\":{\"prompt_tokens\":2,\"completion_tokens\":3,\"total_tokens\":4}}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"usage-negative","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}],\"usage\":{\"prompt_tokens\":-1,\"completion_tokens\":3,\"total_tokens\":2}}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"error","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}],\"error\":{\"message\":\"unavailable\"}}",UMI_STATUS_PARSE_ERROR,"Hello",1,0,0,0},
    {"tool-call","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"tool_calls\":[]},\"finish_reason\":null}]}",UMI_STATUS_PERMISSION_DENIED,"Hello",1,0,0,0},
    {"function-call","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"function_call\":{}},\"finish_reason\":null}]}",UMI_STATUS_PERMISSION_DENIED,"Hello",1,0,0,0},
    {"tool-finish","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":\"tool_calls\"}]}",UMI_STATUS_PERMISSION_DENIED,"Hello",1,0,0,0},
    {"audio","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"audio\":{}},\"finish_reason\":null}]}",UMI_STATUS_UNAVAILABLE,"Hello",1,0,0,0},
    {"refusal","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"refusal\":\"declined\"},\"finish_reason\":null}]}",UMI_STATUS_UNAVAILABLE,"Hello",1,0,0,0},
    {"reasoning","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\",\"reasoning_content\":\"separate reasoning\"},\"finish_reason\":null}]}",UMI_STATUS_UNAVAILABLE,"Hello",1,0,0,0},
    {"trailing-json","{\"id\":\"local-1\",\"object\":\"chat.completion.chunk\",\"model\":\"test-model\",\"choices\":[{\"index\":0,\"delta\":{\"content\":\"Hello\"},\"finish_reason\":null}]} {}",UMI_STATUS_PARSE_ERROR,"",0,0,0,0},
    {"malformed","{\"choices\":[",UMI_STATUS_PARSE_ERROR,"",0,0,0,0}
};
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!UmiAiWorkspaceLocalProviderAvailable()){
        fputs("SKIP: optional local HTTP/json-c support is unavailable; no codec or network execution occurred.\n",stderr);
        return 77;
    }
    UmiAiRequest *request=calloc(1U,sizeof *request);CHECK(request!=NULL);
    strcpy(request->request_id,"delta.test");strcpy(request->model_id,"test-model");
    int result=2;
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i){
        const Fixture *fixture=&fixtures[i];
        if(strcmp(argv[1],fixture->name)!=0)continue;
        AwLocalChatDelta delta,before;memset(&delta,0xa5,sizeof delta);memcpy(&before,&delta,sizeof before);
        UmiStatus status=AwLocalChatDeltaDecode(request,fixture->json,strlen(fixture->json),&delta);
        CHECK(status==fixture->status);
        if(status==UMI_STATUS_OK){
            CHECK(!delta.done && strcmp(delta.completion_id,"local-1")==0);
            CHECK(strcmp(delta.model_id,"test-model")==0 && strcmp(delta.text,fixture->text)==0);
            CHECK(delta.has_content==(bool)fixture->content && delta.has_role==(bool)fixture->role);
            CHECK(delta.has_finish==(fixture->finish!=0) && (int)delta.finish_reason==fixture->finish);
            CHECK(delta.has_usage==(bool)fixture->usage);
            CHECK(delta.usage_only==(strcmp(fixture->name,"usage-only")==0));
            if(fixture->usage)CHECK(delta.usage.input_tokens==2U && delta.usage.output_tokens==3U && delta.usage.total_tokens==5U);
        }else CHECK(memcmp(&before,&delta,sizeof delta)==0);
        result=0;break;
    }
    free(request);return result;
}
