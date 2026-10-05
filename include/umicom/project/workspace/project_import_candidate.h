/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_import_candidate.h
 *
 * PURPOSE:
 *   Publish the public project import candidate contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_IMPORT_CANDIDATE_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_IMPORT_CANDIDATE_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace project import candidate data shared with callers of
 * this public contract.
 */
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_project_import_candidate_archive_encode(const UmiProjectWorkspaceProjectImportCandidate *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_project_import_candidate_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceProjectImportCandidate *value);

#ifdef __cplusplus
}
#endif
#endif
