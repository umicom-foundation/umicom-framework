/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace/application_frontend_selection.c
 *
 * PURPOSE:
 *   Implement the application frontend selection behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Application Frontend Selection | MIT */
#include "umicom/project/workspace/application_frontend_selection.h"
#include "internal.h"
#include <string.h>
/*
 * Initialise project workspace application frontend selection from caller-provided values
 * so later operations receive a known state.
 */
void umi_project_workspace_application_frontend_selection_init(UmiProjectWorkspaceApplicationFrontendSelection *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value != NULL) {
        (void)memset(value,0,sizeof(*value));
        value->revision=1U;
    }
}
/*
 * Provide the project workspace application frontend selection contains operation used by
 * this module and its client applications.
 */
bool umi_project_workspace_application_frontend_selection_contains(const UmiProjectWorkspaceApplicationFrontendSelection *value, const char *text) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||text==NULL)return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<value->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(value->items[i],text)==0)return true;
    return false;
}
/*
 * Add project workspace application frontend selection only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_project_workspace_application_frontend_selection_add(UmiProjectWorkspaceApplicationFrontendSelection *value, const char *text) {
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||text==NULL||text[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_project_workspace_application_frontend_selection_contains(value,text))return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(value->count>=UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    s=umi_pw_copy(value->items[value->count],sizeof(value->items[value->count]),text);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK)return s;
    value->count+=1U;
    value->revision+=1U;
    return UMI_STATUS_OK;
}
/*
 * Remove project workspace application frontend selection while keeping the remaining
 * records in a valid and discoverable state.
 */
UmiStatus umi_project_workspace_application_frontend_selection_remove(UmiProjectWorkspaceApplicationFrontendSelection *value, const char *text) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<value->count;++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if(strcmp(value->items[i],text)==0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if(i+1U<value->count)(void)memmove(&value->items[i],&value->items[i+1U],(value->count-i-1U)*sizeof(value->items[0]));
            value->count-=1U;
            value->revision+=1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
