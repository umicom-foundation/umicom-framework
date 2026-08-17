/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/command.c
 *
 * PURPOSE:
 *   Implement the stable command catalogue for Problems navigation, policy,
 *   suppression, baselines, fixes and SARIF interchange. Command identities
 *   live in Framework so every frontend exposes the same behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/command.h"

#include <string.h>

#define COMMAND(kind_value, command_id, command_label, keybinding, problems,   \
                selection, fix, confirmation, policy, workspace)               \
    {                                                                           \
        (uint32_t)sizeof(UmiDiagnosticCommandDescriptor),                       \
        UMI_DIAGNOSTIC_COMMAND_API_VERSION,                                     \
        kind_value, command_id, command_label, keybinding, problems, selection, \
        fix, confirmation, policy, workspace                                    \
    }

static const UmiDiagnosticCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_DIAGNOSTIC_COMMAND_OPEN_PROBLEMS,
            "diagnostics.problems.open", "Open Problems", "Ctrl+Shift+M",
            0, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_REFRESH,
            "diagnostics.problems.refresh", "Refresh Problems", "F5",
            0, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_CLEAR,
            "diagnostics.problems.clear", "Clear Problems", NULL,
            1, 0, 0, 1, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_NEXT,
            "diagnostics.problems.next", "Next Problem", "F8",
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_PREVIOUS,
            "diagnostics.problems.previous", "Previous Problem", "Shift+F8",
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_GROUP_FILE,
            "diagnostics.problems.group-file", "Group Problems by File", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_GROUP_PROVIDER,
            "diagnostics.problems.group-provider",
            "Group Problems by Provider", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_GROUP_SEVERITY,
            "diagnostics.problems.group-severity",
            "Group Problems by Severity", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_FILTER_ERRORS,
            "diagnostics.problems.filter-errors", "Show Errors", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_FILTER_WARNINGS,
            "diagnostics.problems.filter-warnings", "Show Warnings", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_FILTER_NEW,
            "diagnostics.problems.filter-new", "Show New Problems", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_FILTER_SUPPRESSED,
            "diagnostics.problems.filter-suppressed",
            "Show Suppressed Problems", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_OPEN_DETAILS,
            "diagnostics.problem.open-details", "Open Problem Details", NULL,
            1, 1, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_COPY_MESSAGE,
            "diagnostics.problem.copy-message", "Copy Problem Message", NULL,
            1, 1, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_COPY_ALL,
            "diagnostics.problems.copy-all", "Copy All Problems", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_OPEN_FIX,
            "diagnostics.problem.open-fix", "Preview Problem Fix", "Ctrl+.",
            1, 1, 1, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_APPLY_FIX,
            "diagnostics.problem.apply-fix", "Apply Problem Fix", NULL,
            1, 1, 1, 1, 0, 1),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_SUPPRESS,
            "diagnostics.problem.suppress", "Suppress Problem", NULL,
            1, 1, 0, 1, 1, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_UNSUPPRESS,
            "diagnostics.problem.unsuppress", "Remove Problem Suppression", NULL,
            1, 1, 0, 1, 1, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_MANAGE_SUPPRESSIONS,
            "diagnostics.suppressions.manage", "Manage Suppressions", NULL,
            0, 0, 0, 0, 1, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_MANAGE_SEVERITY_POLICY,
            "diagnostics.severity-policy.manage", "Manage Severity Policy", NULL,
            0, 0, 0, 0, 1, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_CAPTURE_BASELINE,
            "diagnostics.baseline.capture", "Capture Diagnostic Baseline", NULL,
            1, 0, 0, 1, 1, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_COMPARE_BASELINE,
            "diagnostics.baseline.compare", "Compare Diagnostic Baseline", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_IMPORT_SARIF,
            "diagnostics.sarif.import", "Import SARIF", NULL,
            0, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_EXPORT_SARIF,
            "diagnostics.sarif.export", "Export SARIF", NULL,
            1, 0, 0, 0, 0, 0),
    COMMAND(UMI_DIAGNOSTIC_COMMAND_PROVIDER_DETAILS,
            "diagnostics.provider.open-details", "Open Diagnostic Provider", NULL,
            0, 0, 0, 0, 0, 0)
};

#undef COMMAND

size_t umi_diagnostic_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiDiagnosticCommandDescriptor *umi_diagnostic_command_at(size_t position)
{
    return position < umi_diagnostic_command_count() ? &COMMANDS[position]
                                                     : NULL;
}

const UmiDiagnosticCommandDescriptor *umi_diagnostic_command_find(
    const char *command_id)
{
    size_t position;
    if (command_id == NULL) return NULL;
    for (position = 0U; position < umi_diagnostic_command_count(); ++position) {
        if (strcmp(COMMANDS[position].id, command_id) == 0) {
            return &COMMANDS[position];
        }
    }
    return NULL;
}

const UmiDiagnosticCommandDescriptor *umi_diagnostic_command_for_kind(
    UmiDiagnosticCommandKind kind)
{
    size_t position;
    for (position = 0U; position < umi_diagnostic_command_count(); ++position) {
        if (COMMANDS[position].kind == kind) return &COMMANDS[position];
    }
    return NULL;
}
