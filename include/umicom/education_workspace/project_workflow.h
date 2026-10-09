/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education_workspace/project_workflow.h
 * PURPOSE: Describe and export lesson projects with explicit IDE build and run settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDUCATION_WORKSPACE_PROJECT_WORKFLOW_H
#define UMICOM_EDUCATION_WORKSPACE_PROJECT_WORKFLOW_H
#include "umicom/build/profile.h"
#include "umicom/developer_project/model.h"
#include "umicom/education_workspace/projects.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiEducationProjectWorkflow
    {
        UmiDeveloperProjectModel project;
        UmiBuildProfile build;
        int requires_framework_sdk;
    } UmiEducationProjectWorkflow;
    /** Prepare copied metadata for a known lesson and an explicit export folder.
 * No filesystem access or execution occurs. The destination follows the export
 * contract: an absolute local directory, under 1024 UTF-8 bytes, with no trailing
 * separator. Outputs change only on success. The profile uses Ninja, Debug,
 * build/default and the lesson's actual executable; no trust is granted.
 * Framework lessons additionally require an installed Framework data SDK. */
    UmiStatus UmiEducationProjectWorkflowDescribe(const char *project_id, const char *destination,
                                                  UmiEducationProjectWorkflow *out);
    /** Export canonical lesson bytes exclusively, then return their prepared
 * metadata. Existing locations are never overwritten. On failure, out stays
 * unchanged and out_files_written describes complete writes; partial output
 * remains for review. This does not build, run, adopt or trust the project. */
    UmiStatus UmiEducationProjectExportForDevelopment(const char *project_id,
                                                      const char *destination,
                                                      size_t *out_files_written,
                                                      UmiEducationProjectWorkflow *out);
#ifdef __cplusplus
}
#endif
#endif
