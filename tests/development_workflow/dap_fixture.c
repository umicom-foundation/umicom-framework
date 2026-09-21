/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/dap_fixture.c
 * PURPOSE: Deterministic out-of-process DAP peer for protocol/lifecycle tests.
 * This fixture does NOT debug a process. It emits controlled sample state so
 * ordering, timeouts and caller behaviour can be checked independently of GDB.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif
static uint64_t sequence = 1U;
static int Write(const char *text) {
    if (printf("Content-Length: %zu\r\n\r\n%s", strlen(text), text) < 0) return 0;
    return fflush(stdout) == 0;
}
static int Response(uint64_t request, const char *command, int success, const char *body) {
    char json[8192];
    if (umi_debug_runtime_build_response(sequence++, request, command, success,
        success ? "" : "Deliberate fixture refusal", body, json, sizeof(json)) != UMI_STATUS_OK) return 0;
    return Write(json);
}
static int Event(const char *event, const char *body) {
    char json[8192];
    int n=snprintf(json,sizeof(json),"{\"seq\":%llu,\"type\":\"event\",\"event\":\"%s\",\"body\":%s}",
        (unsigned long long)sequence++,event,body);
    return n>0 && (size_t)n<sizeof(json) && Write(json);
}
int main(int argc, char **argv) {
#ifdef _WIN32
    (void)_setmode(_fileno(stdin),_O_BINARY); (void)_setmode(_fileno(stdout),_O_BINARY);
#endif
    const char *mode=argc>1?argv[1]:"launch-late";
    uint64_t launch=0U; int configured=0, stepped=0, breakpointRequests=0;
    char line[256],json[65536];
    while (fgets(line,sizeof(line),stdin)!=NULL) {
        size_t length=0;
        if(sscanf(line,"Content-Length: %zu",&length)!=1 || length==0U || length>=sizeof(json)) return 2;
        while(fgets(line,sizeof(line),stdin)!=NULL && strcmp(line,"\r\n")!=0 && strcmp(line,"\n")!=0) {}
        if(fread(json,1U,length,stdin)!=length) return 3;
        json[length]='\0';
        UmiDebugRuntimeEnvelope request;
        if(umi_debug_runtime_message_parse(json,&request)!=UMI_STATUS_OK) return 4;
        const char *cmd=request.command;
        fprintf(stderr,"fixture got %s seq %llu\n",cmd,(unsigned long long)request.sequence);
        if(strcmp(cmd,"initialize")==0) {
            if(strcmp(mode,"initialized-first")==0 && !Event("initialized","{}")) return 5;
            if(!Response(request.sequence,cmd,1,"{\"supportsConfigurationDoneRequest\":true,\"supportsTerminateRequest\":true}")) return 5;
        } else if(strcmp(cmd,"launch")==0) {
            launch=request.sequence;
            if(strcmp(mode,"reject-launch")==0) { if(!Response(launch,cmd,0,"{}")) return 5; }
            else if(strcmp(mode,"no-initialized")==0) { /* Keep the pipe open, no fabricated readiness. */ }
            else {
                if(strcmp(mode,"launch-early")==0 && !Response(launch,cmd,1,"{}")) return 5;
                if(strcmp(mode,"initialized-first")!=0 && !Event("initialized","{}")) return 5;
            }
        } else if(strcmp(cmd,"setBreakpoints")==0) {
            ++breakpointRequests;
            /* Initial requests must occur before configurationDone. */
            if(!Response(request.sequence,cmd,1,strstr(json,"\"line\"")!=NULL
                ? "{\"breakpoints\":[{\"id\":1,\"verified\":true,\"line\":13}]}"
                : "{\"breakpoints\":[]}")) return 5;
        } else if(strcmp(cmd,"configurationDone")==0) {
            if(strcmp(mode,"initial-breakpoints")==0 && breakpointRequests!=1) return 6;
            configured=1;
            /* The launch response is deliberately interleaved while the client
             * waits for configurationDone; it must later consume the queue. */
            if(strcmp(mode,"launch-early")!=0 && !Response(launch,"launch",1,"{}")) return 5;
            if(!Response(request.sequence,cmd,1,"{}") || !Event("stopped","{\"reason\":\"breakpoint\",\"threadId\":1,\"allThreadsStopped\":true}")) return 5;
        } else if(strcmp(cmd,"threads")==0) {
            if(!configured) return 7;
            if(!Response(request.sequence,cmd,1,"{\"threads\":[{\"id\":1,\"name\":\"Notes worker\"}]}")) return 5;
        } else if(strcmp(cmd,"stackTrace")==0) {
            if(!Response(request.sequence,cmd,1,"{\"stackFrames\":[{\"id\":0,\"name\":\"main\",\"line\":13,\"column\":1,\"source\":{\"path\":\"notes.c\"}}],\"totalFrames\":1}")) return 5;
        } else if(strcmp(cmd,"scopes")==0) {
            if(strstr(json,"\"frameId\":0")==NULL) return 8;
            if(!Response(request.sequence,cmd,1,"{\"scopes\":[{\"name\":\"Locals\",\"variablesReference\":1,\"expensive\":false}]}")) return 5;
        } else if(strcmp(cmd,"variables")==0) {
            if(!Response(request.sequence,cmd,1,stepped
                ? "{\"variables\":[{\"name\":\"savedNotes\",\"value\":\"5\",\"type\":\"int\",\"variablesReference\":0}]}"
                : "{\"variables\":[{\"name\":\"savedNotes\",\"value\":\"2\",\"type\":\"int\",\"variablesReference\":0}]}")) return 5;
        } else if(strcmp(cmd,"evaluate")==0) {
            if(strstr(json,"\"frameId\":0")==NULL) return 9;
            if(!Response(request.sequence,cmd,1,stepped?"{\"result\":\"5\",\"type\":\"int\",\"variablesReference\":0}":"{\"result\":\"2\",\"type\":\"int\",\"variablesReference\":0}")) return 5;
        } else if(strcmp(cmd,"next")==0 || strcmp(cmd,"stepIn")==0 || strcmp(cmd,"stepOut")==0 || strcmp(cmd,"pause")==0) {
            stepped=1;
            if(!Response(request.sequence,cmd,1,"{}") || !Event("continued","{\"threadId\":1}") ||
                !Event("stopped","{\"reason\":\"step\",\"threadId\":1,\"allThreadsStopped\":true}")) return 5;
        } else if(strcmp(cmd,"continue")==0) {
            if(!Response(request.sequence,cmd,1,"{\"allThreadsContinued\":true}") || !Event("exited","{\"exitCode\":0}") || !Event("terminated","{}")) return 5;
        } else if(strcmp(cmd,"disconnect")==0 || strcmp(cmd,"terminate")==0) {
            return Response(request.sequence,cmd,1,"{}")?0:5;
        } else if(!Response(request.sequence,cmd,0,"{}")) return 5;
    }
    return 0;
}
