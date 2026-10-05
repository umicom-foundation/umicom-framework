/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_generator_request.h
 *
 * PURPOSE:
 *   Publish the public application generator request contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATOR_REQUEST_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_GENERATOR_REQUEST_H
#include "umicom/project/workspace/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace application generator request data shared with callers
 * of this public contract.
 */
    typedef struct UmiProjectWorkspaceApplicationGeneratorRequest {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceApplicationGeneratorRequest;
    UmiStatus umi_project_workspace_application_generator_request_init(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *id);
    UmiStatus umi_project_workspace_application_generator_request_validate(const UmiProjectWorkspaceApplicationGeneratorRequest *value);
    UmiStatus umi_project_workspace_application_generator_request_set_name(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *name);
    UmiStatus umi_project_workspace_application_generator_request_set_detail(UmiProjectWorkspaceApplicationGeneratorRequest *value,const char *detail);
    UmiStatus umi_project_workspace_application_generator_request_set_state(UmiProjectWorkspaceApplicationGeneratorRequest *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_application_generator_request_set_metric(UmiProjectWorkspaceApplicationGeneratorRequest *value,uint64_t metric);
    bool umi_project_workspace_application_generator_request_same_identity(const UmiProjectWorkspaceApplicationGeneratorRequest *left,const UmiProjectWorkspaceApplicationGeneratorRequest *right);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_application_generator_request_archive_encode(const UmiProjectWorkspaceApplicationGeneratorRequest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_application_generator_request_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceApplicationGeneratorRequest *value);

#ifdef __cplusplus
}
#endif
#endif
