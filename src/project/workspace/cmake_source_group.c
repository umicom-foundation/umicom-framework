/* Umicom Framework | Cmake Source Group | MIT */
#include "umicom/project/workspace/cmake_source_group.h"
#include "internal.h"
#include <string.h>
void umi_project_workspace_cmake_source_group_init(UmiProjectWorkspaceCmakeSourceGroup *value) {
    if (value != NULL) {
        (void)memset(value,0,sizeof(*value));
        value->revision=1U;
    }
}
bool umi_project_workspace_cmake_source_group_contains(const UmiProjectWorkspaceCmakeSourceGroup *value, const char *text) {
    size_t i;
    if(value==NULL||text==NULL)return false;
    for(i=0U;i<value->count;++i)if(strcmp(value->items[i],text)==0)return true;
    return false;
}
UmiStatus umi_project_workspace_cmake_source_group_add(UmiProjectWorkspaceCmakeSourceGroup *value, const char *text) {
    UmiStatus s;
    if(value==NULL||text==NULL||text[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_project_workspace_cmake_source_group_contains(value,text))return UMI_STATUS_ALREADY_EXISTS;
    if(value->count>=UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    s=umi_pw_copy(value->items[value->count],sizeof(value->items[value->count]),text);
    if(s!=UMI_STATUS_OK)return s;
    value->count+=1U;
    value->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_project_workspace_cmake_source_group_remove(UmiProjectWorkspaceCmakeSourceGroup *value, const char *text) {
    size_t i;
    if(value==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<value->count;++i) {
        if(strcmp(value->items[i],text)==0) {
            if(i+1U<value->count)(void)memmove(&value->items[i],&value->items[i+1U],(value->count-i-1U)*sizeof(value->items[0]));
            value->count-=1U;
            value->revision+=1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
