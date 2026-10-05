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
/* Inspect the received JSON independently of the launch formatter. Existing
 * scenarios remain unchanged unless the dedicated argument marker is present. */
static int CheckLaunchArguments(const char *json)
{
    if (strstr(json, "--umicom-argument-check") == NULL) return 1;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof(*document));
    if (document == NULL) return 0;
    int valid = umi_language_runtime_json_parse(json, document) == UMI_STATUS_OK;
    int launch = valid ? umi_language_runtime_json_object_get(document, 0, "arguments") : -1;
    int args = launch >= 0 ? umi_language_runtime_json_object_get(document, launch, "args") : -1;
    const char *expected[] = {"--umicom-argument-check", "two words", "", "C:\\Folder with spaces\\", "caf\xc3\xa9", "literal;$(value)"};
    if (args < 0 || umi_language_runtime_json_array_count(document, args) != 6U) valid = 0;
    for (size_t index = 0U; valid && index < 6U; ++index) {
        char value[128];
        int item = umi_language_runtime_json_array_at(document, args, index);
        if (umi_language_runtime_json_string(document, item, value, sizeof(value)) != UMI_STATUS_OK ||
            strcmp(value, expected[index]) != 0) valid = 0;
    }
    free(document);
    return valid;
}

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
#include "../watch_edit/peer.inc"
#include "../variable_inspection/peer.inc"
#include "../scope_inspection/peer.inc"
#include "../variable_assignment/peer.inc"
#include "../memory_inspection/peer.inc"
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
        /* Memory scenarios negotiate their own capability and verify each read.
         * All older fixture modes keep their existing replies below. */
        if (strncmp(mode, "memory-inspection-", 18) == 0 &&
            (strcmp(cmd, "initialize") == 0 || strcmp(cmd, "variables") == 0 || strcmp(cmd, "readMemory") == 0)) {
            if (!MemoryPeerReply(&request, mode, json)) return 19;
            continue;
        }
        if (strncmp(mode, "variable-assignment-", 20) == 0 &&
            (strcmp(cmd, "variables") == 0 || strcmp(cmd, "setVariable") == 0 || strcmp(cmd, "setExpression") == 0)) {
            if (!AssignmentPeerReply(&request, mode, json)) return 18;
            continue;
        }
        if(strcmp(cmd,"initialize")==0) {
            if(strcmp(mode,"initialized-first")==0 && !Event("initialized","{}")) return 5;
/* Property synchronization tests explicitly negotiate optional capabilities; older fixture modes keep their original advertised set. The previous implementation remains for engineering review. */
#if 0
            if(!Response(request.sequence,cmd,1,"{\"supportsConfigurationDoneRequest\":true,\"supportsTerminateRequest\":true}")) return 5;
#endif
            /* Assignment modes negotiate only their own optional capability. */
            if (strncmp(mode, "variable-assignment-", 20) == 0 && strcmp(mode, "variable-assignment-unsupported") != 0) {
                if (!Response(request.sequence, cmd, 1,
                    "{\"supportsConfigurationDoneRequest\":true,\"supportsTerminateRequest\":true,\"supportsSetVariable\":true,\"supportsSetExpression\":true}")) return 5;
                continue;
            }
            if (strncmp(mode, "properties-", 11) == 0 && strcmp(mode, "properties-unsupported") != 0) {
                if (!Response(request.sequence, cmd, 1,
                    "{\"supportsConfigurationDoneRequest\":true,\"supportsTerminateRequest\":true,\"supportsConditionalBreakpoints\":true,\"supportsLogPoints\":true}")) return 5;
            } else {
                if(!Response(request.sequence,cmd,1,"{\"supportsConfigurationDoneRequest\":true,\"supportsTerminateRequest\":true}")) return 5;
            }
        } else if(strcmp(cmd,"launch")==0) {
            if (!CheckLaunchArguments(json)) return 16;
            launch=request.sequence;
            if(strcmp(mode,"reject-launch")==0) { if(!Response(launch,cmd,0,"{}")) return 5; }
            else if(strcmp(mode,"no-initialized")==0) { /* Keep the pipe open, no fabricated readiness. */ }
            else {
                if(strcmp(mode,"launch-early")==0 && !Response(launch,cmd,1,"{}")) return 5;
                if(strcmp(mode,"initialized-first")!=0 && !Event("initialized","{}")) return 5;
            }
        } else if(strcmp(cmd,"setBreakpoints")==0) {
            ++breakpointRequests;
            if (strncmp(mode, "properties-", 11) == 0) {
                if (strcmp(mode, "properties-unsupported") == 0) return 12; /* Client must reject before sending. */
                if (strstr(json, "\"condition\":\"count > 3\"") == NULL ||
                    strstr(json, "\"logMessage\":\"count={count}\"") == NULL) return 13;
                if (strcmp(mode, "properties-rejected") == 0) {
                    if (!Response(request.sequence, cmd, 0, "{}")) return 5;
                } else if (strcmp(mode, "properties-short") == 0) {
                    if (!Response(request.sequence, cmd, 1, "{\"breakpoints\":[]}")) return 5;
                } else if (strcmp(mode, "properties-malformed") == 0) {
                    if (!Response(request.sequence, cmd, 1, "{\"breakpoints\":[{\"verified\":true,\"line\":-3}]}")) return 5;
                } else if (!Response(request.sequence, cmd, 1,
                    "{\"breakpoints\":[{\"verified\":true,\"line\":15}]}")) return 5;
                continue;
            }
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
            /* Explicit scope modes keep expensive references unloaded at startup. */
            if (strncmp(mode, "scope-inspection-", 17) == 0) {
                if (!ScopePeerScopes(&request, mode)) return 16;
                continue;
            }
            if(!Response(request.sequence,cmd,1,"{\"scopes\":[{\"name\":\"Locals\",\"variablesReference\":1,\"expensive\":false}]}")) return 5;
        } else if(strcmp(cmd,"variables")==0) {
            if (strncmp(mode, "scope-inspection-", 17) == 0) {
                if (!ScopePeerVariables(&request, mode, json)) return 17;
                continue;
            }
            /* Owned child inspection modes are independent of legacy fixture replies. */
            if (strncmp(mode, "variable-inspection-", 20) == 0) {
                if (!VariablePeerReply(&request, mode, json)) return 15;
                continue;
            }
            if(!Response(request.sequence,cmd,1,stepped
                ? "{\"variables\":[{\"name\":\"savedNotes\",\"value\":\"5\",\"type\":\"int\",\"variablesReference\":0}]}"
                : "{\"variables\":[{\"name\":\"savedNotes\",\"value\":\"2\",\"type\":\"int\",\"variablesReference\":0}]}")) return 5;
        } else if(strcmp(cmd,"evaluate")==0) {
            /* New watch modes validate explicit row requests; all existing modes
             * retain their prior protocol behaviour below. */
            if (strncmp(mode, "watch-edit-", 11) == 0) {
                if (!WatchPeerReply(&request, mode, json)) return 14;
                continue;
            }
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
