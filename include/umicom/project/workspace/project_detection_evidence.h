/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_detection_evidence.h
 * PURPOSE: Framework-owned Project Detection Evidence contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_DETECTION_EVIDENCE_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_DETECTION_EVIDENCE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectDetectionEvidence {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectDetectionEvidence;
    UmiStatus umi_project_workspace_project_detection_evidence_init(UmiProjectWorkspaceProjectDetectionEvidence *value,const char *id);
    UmiStatus umi_project_workspace_project_detection_evidence_validate(const UmiProjectWorkspaceProjectDetectionEvidence *value);
    UmiStatus umi_project_workspace_project_detection_evidence_set_name(UmiProjectWorkspaceProjectDetectionEvidence *value,const char *name);
    UmiStatus umi_project_workspace_project_detection_evidence_set_detail(UmiProjectWorkspaceProjectDetectionEvidence *value,const char *detail);
    UmiStatus umi_project_workspace_project_detection_evidence_set_state(UmiProjectWorkspaceProjectDetectionEvidence *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_detection_evidence_set_metric(UmiProjectWorkspaceProjectDetectionEvidence *value,uint64_t metric);
    bool umi_project_workspace_project_detection_evidence_same_identity(const UmiProjectWorkspaceProjectDetectionEvidence *left,const UmiProjectWorkspaceProjectDetectionEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
