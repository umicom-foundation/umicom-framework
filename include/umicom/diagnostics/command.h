/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/command.h
 *
 * PURPOSE:
 *   Publish stable command identities for Problems navigation, grouping,
 *   severity policy, auditable suppression, baselines, fixes and SARIF
 *   interchange. Applications contribute placement only.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_COMMAND_H
#define UMICOM_DIAGNOSTICS_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_COMMAND_API_VERSION 1U

typedef enum UmiDiagnosticCommandKind {
    UMI_DIAGNOSTIC_COMMAND_OPEN_PROBLEMS = 1,
    UMI_DIAGNOSTIC_COMMAND_REFRESH = 2,
    UMI_DIAGNOSTIC_COMMAND_CLEAR = 3,
    UMI_DIAGNOSTIC_COMMAND_NEXT = 4,
    UMI_DIAGNOSTIC_COMMAND_PREVIOUS = 5,
    UMI_DIAGNOSTIC_COMMAND_GROUP_FILE = 6,
    UMI_DIAGNOSTIC_COMMAND_GROUP_PROVIDER = 7,
    UMI_DIAGNOSTIC_COMMAND_GROUP_SEVERITY = 8,
    UMI_DIAGNOSTIC_COMMAND_FILTER_ERRORS = 9,
    UMI_DIAGNOSTIC_COMMAND_FILTER_WARNINGS = 10,
    UMI_DIAGNOSTIC_COMMAND_FILTER_NEW = 11,
    UMI_DIAGNOSTIC_COMMAND_FILTER_SUPPRESSED = 12,
    UMI_DIAGNOSTIC_COMMAND_OPEN_DETAILS = 13,
    UMI_DIAGNOSTIC_COMMAND_COPY_MESSAGE = 14,
    UMI_DIAGNOSTIC_COMMAND_COPY_ALL = 15,
    UMI_DIAGNOSTIC_COMMAND_OPEN_FIX = 16,
    UMI_DIAGNOSTIC_COMMAND_APPLY_FIX = 17,
    UMI_DIAGNOSTIC_COMMAND_SUPPRESS = 18,
    UMI_DIAGNOSTIC_COMMAND_UNSUPPRESS = 19,
    UMI_DIAGNOSTIC_COMMAND_MANAGE_SUPPRESSIONS = 20,
    UMI_DIAGNOSTIC_COMMAND_MANAGE_SEVERITY_POLICY = 21,
    UMI_DIAGNOSTIC_COMMAND_CAPTURE_BASELINE = 22,
    UMI_DIAGNOSTIC_COMMAND_COMPARE_BASELINE = 23,
    UMI_DIAGNOSTIC_COMMAND_IMPORT_SARIF = 24,
    UMI_DIAGNOSTIC_COMMAND_EXPORT_SARIF = 25,
    UMI_DIAGNOSTIC_COMMAND_PROVIDER_DETAILS = 26
} UmiDiagnosticCommandKind;

typedef struct UmiDiagnosticCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDiagnosticCommandKind kind;
    const char *id;
    const char *label;
    const char *default_keybinding;
    int requires_problems;
    int requires_selection;
    int requires_fix;
    int requires_confirmation;
    int mutates_policy;
    int mutates_workspace;
} UmiDiagnosticCommandDescriptor;

size_t umi_diagnostic_command_count(void);
const UmiDiagnosticCommandDescriptor *umi_diagnostic_command_at(size_t position);
const UmiDiagnosticCommandDescriptor *umi_diagnostic_command_find(
    const char *command_id);
const UmiDiagnosticCommandDescriptor *umi_diagnostic_command_for_kind(
    UmiDiagnosticCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_COMMAND_H */
