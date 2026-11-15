/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_local_chat/main.c
 *
 * PURPOSE:
 *   Demonstrate explicit local-model consent and joined streaming through an installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/local_stream.h"
#include "umicom/platform/threading.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static volatile sig_atomic_t interrupted;
static void Interrupt(int signal_number){(void)signal_number;interrupted=1;}
static int Failure(const char *operation,UmiStatus status)
{
    fprintf(stderr,"%s: %s\n",operation,umi_status_text(status));return 1;
}
int main(int argc,char **argv)
{
    if(argc!=5 || strcmp(argv[1],"--allow-network")!=0){
        fputs("Umicom local chat SDK example\nUsage: umicom-sdk-local-chat --allow-network PORT MODEL \"PROMPT\"\n"
            "Contacts an existing trusted server on 127.0.0.1. Ctrl+C requests cancellation.\n",stderr);return 2;
    }
    char *end=NULL;errno=0;unsigned long port=strtoul(argv[2],&end,10);
    if(errno!=0 || end==argv[2] || *end!='\0' || port<1024UL || port>65535UL){
        fputs("PORT must be an integer from 1024 through 65535.\n",stderr);return 2;
    }
    UmiAiRequest *request=calloc(1U,sizeof *request);
    if(request==NULL)return Failure("Allocate request",UMI_STATUS_OUT_OF_MEMORY);
    if(argv[3][0]=='\0' || strlen(argv[3])>=sizeof request->model_id ||
        argv[4][0]=='\0' || strlen(argv[4])>=sizeof request->messages[0].text){
        free(request);fputs("MODEL or PROMPT is empty or exceeds the SDK request capacity.\n",stderr);return 2;
    }
    strcpy(request->request_id,"sdk.local.stream");strcpy(request->model_id,argv[3]);
    request->message_count=1U;request->max_output_tokens=512U;
    request->messages[0].role=UMI_AI_ROLE_USER;strcpy(request->messages[0].text,argv[4]);
    /* Establish HTTP lifetime before this program starts any worker. GUI hosts
     * do this at startup, before initializing a threaded toolkit. */
    UmiAiLocalTransport *transport=NULL;UmiAiLocalStream *job=NULL;
    UmiStatus status=UmiAiLocalTransportCreate(&transport);
    if(status!=UMI_STATUS_OK){free(request);return Failure("Initialize local transport",status);}
    UmiAiLocalStreamOptions options={.port=(uint16_t)port,.timeout_millis=60000U,
        .maximum_text_bytes=1024U*1024U,.maximum_wire_bytes=16U*1024U*1024U,
        .identity={1U,1U},.allow_network=true};
    status=UmiAiLocalStreamCreate(transport,request,&options,&job);free(request);
    if(status!=UMI_STATUS_OK){
        (void)UmiAiLocalTransportDestroy(&transport);return Failure("Create stream",status);
    }
    if(signal(SIGINT,Interrupt)==SIG_ERR){
        (void)UmiAiLocalStreamDestroy(&job);(void)UmiAiLocalTransportDestroy(&transport);
        fputs("Could not install Ctrl+C handling; no request started.\n",stderr);return 1;
    }
    status=UmiAiLocalStreamStart(job);size_t printed=0U;
    while(status==UMI_STATUS_OK){
        if(interrupted)status=UmiAiLocalStreamCancel(job);
        if(status!=UMI_STATUS_OK)break;
        UmiAiLocalStreamSnapshot observation;
        status=UmiAiLocalStreamPoll(job,&observation);if(status!=UMI_STATUS_OK)break;
        char *text=NULL;UmiAiResponseStreamSnapshot snapshot;
        status=UmiAiLocalStreamCopyText(job,&text,&snapshot);if(status!=UMI_STATUS_OK)break;
        if(snapshot.text_bytes<printed)status=UMI_STATUS_INVALID_STATE;
        else if(snapshot.text_bytes>printed){
            size_t amount=snapshot.text_bytes-printed;
            if(fwrite(text+printed,1U,amount,stdout)!=amount || fflush(stdout)!=0)status=UMI_STATUS_IO_ERROR;
            else printed=snapshot.text_bytes;
        }
        UmiAiResponseStreamTextFree(text);
        if(status!=UMI_STATUS_OK || observation.state==UMI_AI_LOCAL_STREAM_FINISHED)break;
        umi_thread_sleep_ms(25U);
    }
    if(status!=UMI_STATUS_OK)(void)UmiAiLocalStreamCancel(job);
    UmiStatus joined=UmiAiLocalStreamJoin(job);
    if(joined!=UMI_STATUS_OK){
        /* Keep job/runtime storage if native quiescence cannot be established. */
        return Failure("Join request; ownership retained until process exit",joined);
    }
    UmiAiLocalStreamResult result;
    UmiStatus observed=UmiAiLocalStreamReadResult(job,&result);
    if(status==UMI_STATUS_OK)status=observed;
    if(status==UMI_STATUS_OK)status=result.operation_status;
    if(status==UMI_STATUS_OK)status=result.observation_status;
    UmiStatus released=UmiAiLocalStreamDestroy(&job);
    if(released!=UMI_STATUS_OK)return Failure("Release request; ownership retained until process exit",released);
    released=UmiAiLocalTransportDestroy(&transport);
    if(status==UMI_STATUS_OK)status=released;
    if(putchar('\n')==EOF && status==UMI_STATUS_OK)status=UMI_STATUS_IO_ERROR;
    if(status==UMI_STATUS_CANCELLED){fputs("Request cancelled; printed text remains partial.\n",stderr);return 130;}
    return status==UMI_STATUS_OK?0:Failure("Local streaming request",status);
}
