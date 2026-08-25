#include "umicom/project/workspace/cmake_cache_entry.h"
#include "internal.h"
#include <string.h>
UmiStatus umi_project_workspace_cmake_cache_entry_parse(const char*line,UmiProjectWorkspaceCmakeCacheEntry*out) {
    const char*colon;
    const char*eq;
    size_t n;
    if(line==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    colon=strchr(line,':');
    eq=strchr(line,'=');
    if(colon==NULL||eq==NULL||colon>=eq)return UMI_STATUS_PARSE_ERROR;
    (void)memset(out,0,sizeof(*out));
    n=(size_t)(colon-line);
    if(n==0U||n>=sizeof(out->key))return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out->key,line,n);
    out->key[n]='\0';
    n=(size_t)(eq-colon-1);
    if(n==0U||n>=sizeof(out->type))return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out->type,colon+1,n);
    out->type[n]='\0';
    return umi_pw_copy(out->value,sizeof(out->value),eq+1);
}
