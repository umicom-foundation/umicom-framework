/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/source.c
 *
 * PURPOSE:
 *   Recognise supported source files and measure simple complexity/coupling
 *   indicators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/source.h"
#include "source_reader.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
/* Provide the ends operation used by this module and its client applications. */
static int ends(const char *p,const char *s){size_t a,b;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||s==NULL)return 0;a=strlen(p);b=strlen(s);return a>=b&&strcmp(p+a-b,s)==0;}
/*
 * Provide the codeguard source supported operation used by this module and its client
 * applications.
 */
int umi_codeguard_source_supported(const char *p,int cpp,int hdr){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return 0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ends(p,".c"))return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cpp&&(ends(p,".cc")||ends(p,".cpp")||ends(p,".cxx")))return 1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(hdr&&(ends(p,".h")||ends(p,".hpp")||ends(p,".hh")))return 1;return 0;}
/*
 * Provide the codeguard source measure operation used by this module and its client
 * applications.
 */
/* Keep partial measurements private until all input and close operations succeed; reject paths and counters that cannot be represented. The previous implementation remains for review. */
#if 0
UmiStatus umi_codeguard_source_measure(const char *p,UmiCodeGuardSourceInfo *o){FILE *f;char line[8192];size_t bytes=0U,lines=0U,incs=0U,funcs=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;f=fopen(p,"rb");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(f==NULL)return UMI_STATUS_IO_ERROR;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(fgets(line,sizeof(line),f)!=NULL){size_t n=strlen(line);bytes+=n;++lines;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strstr(line,"#include")!=NULL)++incs;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strchr(line,'(')!=NULL&&strchr(line,')')!=NULL&&strchr(line,'{')!=NULL)++funcs;}fclose(f);(void)snprintf(o->path,sizeof(o->path),"%s",p);o->bytes=bytes;o->lines=lines;o->include_count=incs;o->function_hint_count=funcs;return UMI_STATUS_OK;}
#endif
UmiStatus umi_codeguard_source_measure(const char *path, UmiCodeGuardSourceInfo *out_info)
{
    if (path == NULL || out_info == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardSourceInfo candidate = {0};
    size_t path_length = 0U;
    while (path_length < sizeof candidate.path && path[path_length] != '\0') ++path_length;
    if (path_length == sizeof candidate.path) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(candidate.path, path, path_length + 1U);

    FILE *file = fopen(candidate.path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    char line[8192];
    UmiStatus status = UMI_STATUS_OK;
    while (fgets(line, sizeof line, file) != NULL) {
        size_t bytes = strlen(line);
        int has_include = strstr(line, "#include") != NULL;
        int has_function = strchr(line, '(') != NULL &&
            strchr(line, ')') != NULL && strchr(line, '{') != NULL;
        if (bytes > SIZE_MAX - candidate.bytes || candidate.lines == SIZE_MAX ||
            (has_include && candidate.include_count == SIZE_MAX) ||
            (has_function && candidate.function_hint_count == SIZE_MAX)) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        candidate.bytes += bytes;
        ++candidate.lines;
        if (has_include) ++candidate.include_count;
        if (has_function) ++candidate.function_hint_count;
    }
    if (ferror(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (fclose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    /* Publish one complete measurement only after read and close succeed.
     * Preserve caller output on failure, including a path borrowed from it. */
    if (status == UMI_STATUS_OK) *out_info = candidate;
    return status;
}

/* Reuse the original textual heuristics, but count the explicit admitted
 * segment size. Candidate storage keeps partial input private on failure. */
static UmiStatus umi_codeguard_measure_line(const char *line, size_t bytes,
    size_t number, void *context)
{
    (void)number;
    UmiCodeGuardSourceInfo *candidate = context;
    int has_include = strstr(line, "#include") != NULL;
    int has_function = strchr(line, '(') != NULL &&
        strchr(line, ')') != NULL && strchr(line, '{') != NULL;
    if (bytes > SIZE_MAX - candidate->bytes || candidate->lines == SIZE_MAX ||
        (has_include && candidate->include_count == SIZE_MAX) ||
        (has_function && candidate->function_hint_count == SIZE_MAX))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate->bytes += bytes;
    ++candidate->lines;
    if (has_include) ++candidate->include_count;
    if (has_function) ++candidate->function_hint_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_codeguard_source_measure_bounded(const char *path,
    uint64_t maximum_bytes, UmiCodeGuardSourceInfo *out_info,
    const UmiCancellationToken *cancellation)
{
    if (path == NULL || out_info == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardSourceInfo candidate = {0};
    size_t length = 0U;
    while (length < sizeof candidate.path && path[length] != '\0') ++length;
    if (length == sizeof candidate.path) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve aliasing support when path is borrowed from out_info. */
    memcpy(candidate.path, path, length + 1U);
    UmiStatus status = umi_codeguard_source_visit_lines(candidate.path,
        maximum_bytes, umi_codeguard_measure_line, &candidate, cancellation);
    if (status == UMI_STATUS_OK) *out_info = candidate;
    return status;
}
