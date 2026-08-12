/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_validation.h
 *
 * PURPOSE:
 *   Validate project-workspace relationships and report actionable structural
 *   problems before configure, build, test, run or debug operations are queued.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Validation is deliberately read-only. It never deletes or rewrites project
 * records. Problems are returned as bounded records so a GUI, CLI or automated
 * agent can display exactly the same project-health information.
 */
#ifndef UMICOM_PROJECT_WORKSPACE_VALIDATION_H
#define UMICOM_PROJECT_WORKSPACE_VALIDATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/project/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_VALIDATION_API_VERSION 1U
#define UMI_PROJECT_WORKSPACE_VALIDATION_ISSUE_CAPACITY 128U

typedef enum UmiProjectWorkspaceIssueSeverity {
    UMI_PROJECT_WORKSPACE_ISSUE_INFO = 0,
    UMI_PROJECT_WORKSPACE_ISSUE_WARNING = 1,
    UMI_PROJECT_WORKSPACE_ISSUE_ERROR = 2
} UmiProjectWorkspaceIssueSeverity;

typedef struct UmiProjectWorkspaceValidationIssue {
    uint32_t struct_size;
    uint32_t api_version;
    UmiProjectWorkspaceIssueSeverity severity;
    char code[96];
    char project_id[128];
    char item_id[128];
    char message[384];
} UmiProjectWorkspaceValidationIssue;

typedef struct UmiProjectWorkspaceValidationReport {
    uint32_t struct_size;
    uint32_t api_version;
    UmiProjectWorkspaceValidationIssue
        issues[UMI_PROJECT_WORKSPACE_VALIDATION_ISSUE_CAPACITY];
    size_t issue_count;
    size_t error_count;
    size_t warning_count;
    size_t unresolved_required_dependency_count;
    uint64_t workspace_revision;
    int valid;
} UmiProjectWorkspaceValidationReport;

UmiStatus umi_project_workspace_validate(
    const UmiProjectWorkspace *workspace,
    UmiProjectWorkspaceValidationReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
