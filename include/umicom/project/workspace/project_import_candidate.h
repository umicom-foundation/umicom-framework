/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_import_candidate.h
 * PURPOSE: Framework-owned Project Import Candidate contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_IMPORT_CANDIDATE_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_IMPORT_CANDIDATE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectImportCandidate {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectImportCandidate;
    UmiStatus umi_project_workspace_project_import_candidate_init(UmiProjectWorkspaceProjectImportCandidate *value,const char *id);
    UmiStatus umi_project_workspace_project_import_candidate_validate(const UmiProjectWorkspaceProjectImportCandidate *value);
    UmiStatus umi_project_workspace_project_import_candidate_set_name(UmiProjectWorkspaceProjectImportCandidate *value,const char *name);
    UmiStatus umi_project_workspace_project_import_candidate_set_detail(UmiProjectWorkspaceProjectImportCandidate *value,const char *detail);
    UmiStatus umi_project_workspace_project_import_candidate_set_state(UmiProjectWorkspaceProjectImportCandidate *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_import_candidate_set_metric(UmiProjectWorkspaceProjectImportCandidate *value,uint64_t metric);
    bool umi_project_workspace_project_import_candidate_same_identity(const UmiProjectWorkspaceProjectImportCandidate *left,const UmiProjectWorkspaceProjectImportCandidate *right);
#ifdef __cplusplus
}
#endif
#endif
