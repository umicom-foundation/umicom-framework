/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_template_filter.h
 *
 * PURPOSE:
 *   Publish the public project template filter contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_TEMPLATE_FILTER_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_TEMPLATE_FILTER_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace template candidate data shared with callers of this
 * public contract.
 */
    typedef struct UmiProjectWorkspaceTemplateCandidate {
        const char*id;
        const char*language;
        const char*frontend;
        uint64_t features;
    }
    UmiProjectWorkspaceTemplateCandidate;
    int umi_project_workspace_project_template_filter_score(const UmiProjectWorkspaceTemplateCandidate*item,const char*language,const char*frontend,uint64_t required_features);
#ifdef __cplusplus
}
#endif
#endif
