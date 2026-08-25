/* Umicom Framework | Project-template filtering | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_TEMPLATE_FILTER_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_TEMPLATE_FILTER_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
