/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/cmake_cache_entry.c
 *
 * PURPOSE:
 *   Implement the cmake cache entry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/cmake_cache_entry.h"
#include "internal.h"
#include <string.h>
/*
 * Read project workspace cmake cache entry into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_project_workspace_cmake_cache_entry_parse(const char*line,UmiProjectWorkspaceCmakeCacheEntry*out) {
    const char*colon;
    const char*eq;
    size_t n;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(line==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    colon=strchr(line,':');
    eq=strchr(line,'=');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(colon==NULL||eq==NULL||colon>=eq)return UMI_STATUS_PARSE_ERROR;
    (void)memset(out,0,sizeof(*out));
    n=(size_t)(colon-line);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(n==0U||n>=sizeof(out->key))return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out->key,line,n);
    out->key[n]='\0';
    n=(size_t)(eq-colon-1);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(n==0U||n>=sizeof(out->type))return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out->type,colon+1,n);
    out->type[n]='\0';
    return umi_pw_copy(out->value,sizeof(out->value),eq+1);
}
