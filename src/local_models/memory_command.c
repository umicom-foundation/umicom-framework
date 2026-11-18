/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/memory_command.c
 *
 * PURPOSE:
 *   Parse bounded decimal cache geometry and report checked token capacity without model side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/local_models/memory_command.h"
#include "umicom/local_models/memory_plan.h"
#include <inttypes.h>
#include <string.h>

static int Decimal(const char *text,uint64_t *out)
{
    if(text==NULL || *text=='\0') return 0;
    uint64_t value=0;
    /* Bound parsing as well as multiplication: uint64 has at most 20 decimal
     * digits. Leading zeroes are accepted within the same input-size limit. */
    for(size_t i=0;text[i]!='\0';++i) {
        if(i==20 || text[i]<'0' || text[i]>'9') return 0;
        const uint64_t digit=(uint64_t)(text[i]-'0');
        if(value>(UINT64_MAX-digit)/10) return 0;
        value=value*10+digit;
    }
    *out=value;return 1;
}
static int ReportFailure(FILE *stream,const char *message,int code)
{
    return fputs(message,stream)==EOF || fflush(stream)!=0 || ferror(stream) ? 1:code;
}
int UmiModelDenseKvCommand(int argc,const char *const *argv,
    FILE *output,FILE *diagnostics)
{
    if(output==NULL || diagnostics==NULL || output==diagnostics) return 2;
    const char *usage=
        "Umicom dense KV-cache planner\n"
        "Usage: cache-plan BYTES TOKENS SEQUENCES LAYERS KEY_HEADS VALUE_HEADS KEY_DIM VALUE_DIM KEY_BYTES VALUE_BYTES\n"
        "Decimal inputs only. Element bytes: 1, 2, 4 or 8.\n"
        "Budget covers KV payload only; no model is loaded or memory reserved.\n";
    if(argc==1 && argv!=NULL && argv[0]!=NULL && strcmp(argv[0],"--help")==0)
        return ReportFailure(output,usage,0);
    if(argc!=10 || argv==NULL) return ReportFailure(diagnostics,usage,2);
    uint64_t value[10];
    for(size_t i=0;i<10;++i)
        if(!Decimal(argv[i],&value[i]))
            return ReportFailure(diagnostics,"Expected ten bounded unsigned decimal values.\n",2);
    if(value[8]>UINT32_MAX || value[9]>UINT32_MAX)
        return ReportFailure(diagnostics,"Element width is outside the supported range.\n",2);
    const UmiModelDenseKvShape shape={value[1],value[2],value[3],value[4],
        value[5],value[6],value[7],(uint32_t)value[8],(uint32_t)value[9]};
    UmiModelKvTokenPlan plan;
    const UmiStatus status=UmiModelDenseKvPlanTokens(&shape,value[0],&plan);
    if(status!=UMI_STATUS_OK)
        return ReportFailure(diagnostics,"Dense KV geometry is invalid or exceeds integer capacity.\n",
            status==UMI_STATUS_INVALID_ARGUMENT?2:1);
    /* Keep the output complete and explicit: a mathematically fitting cache
     * excludes weights, workspace, alignment, runtime and all GPU allocations. */
    if(fprintf(output,
        "Umicom dense KV-cache plan\n"
        "requested_tokens=%" PRIu64 "\naffordable_tokens=%" PRIu64
        "\nbytes_per_token=%" PRIu64 "\ncache_bytes=%" PRIu64
        "\nunused_budget_bytes=%" PRIu64 "\nrequest_fits=%s\n"
        "Scope: KV payload only. This is not permission to load a model.\n",
        plan.requested_tokens,plan.affordable_tokens,plan.bytes_per_token,
        plan.cache_bytes,plan.unused_budget_bytes,plan.request_fits?"yes":"no")<0 ||
        fflush(output)!=0 || ferror(output)) return 1;
    return 0;
}
