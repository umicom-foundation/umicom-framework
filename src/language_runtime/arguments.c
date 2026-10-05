/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/arguments.c
 *
 * PURPOSE:
 *   Implement shell-independent quoted argument parsing suitable for Windows paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/arguments.h"
#include "umicom/base/arguments.h"
#include <ctype.h>
#include <string.h>
/*
 * Read language runtime arguments into validated module state and return a status when
 * input cannot be used.
 */
/* Argument parsing now belongs to the base library so builds, debuggers and language servers use one grammar.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_arguments_parse(const char*t,UmiLanguageRuntimeArguments*out){const char*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));p=t;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p){char q=0,*d;size_t u=0;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*p))p++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!*p)break;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out->count>=UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;d=out->storage[out->count];/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p){char c=*p;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!q&&isspace((unsigned char)c))break;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\''||c=='"'){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!q){q=c;p++;continue;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==c){q=0;p++;continue;}}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c=='\\'&&(p[1]=='\''||p[1]=='"'||p[1]=='\\'||isspace((unsigned char)p[1]))){c=p[1];p+=2;}/* Use this fallback path when the earlier condition does not apply. */ else p++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(u+1>=UMI_LANGUAGE_RUNTIME_ARGUMENT_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;d[u++]=c;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q)return UMI_STATUS_PARSE_ERROR;d[u]=0;out->values[out->count++]=d;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(isspace((unsigned char)*p))p++;}return UMI_STATUS_OK;}
#endif
UmiStatus umi_language_runtime_arguments_parse(const char *text, UmiLanguageRuntimeArguments *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    UmiArguments parsed;
    UmiStatus status = UmiArgumentsParse(text, &parsed);
    if (status != UMI_STATUS_OK) return status;
    /* Keep the established public layout while borrowing the common grammar.
     * Rebind each pointer to the caller's storage before the local copy expires. */
    _Static_assert(UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS >= UMI_ARGUMENTS_CAPACITY,
        "language argument count must hold the shared parser output");
    _Static_assert(UMI_LANGUAGE_RUNTIME_ARGUMENT_CAPACITY >= UMI_ARGUMENT_TEXT_CAPACITY,
        "language argument text must hold the shared parser output");
    for (size_t index = 0U; index < parsed.count; ++index) {
        strcpy(out->storage[index], parsed.values[index]);
        out->values[index] = out->storage[index];
    }
    out->count = parsed.count;
    return UMI_STATUS_OK;
}
