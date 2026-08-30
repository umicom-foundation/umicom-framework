/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/diagnostic_provider.h
 *
 * PURPOSE:
 *   Define provider-neutral diagnostic producers and immutable diagnostic
 *   findings. Compiler frontends, linkers, linters, language servers, test
 *   runners, security scanners and runtime monitors all publish through this
 *   contract instead of creating application-specific Problems models.
 *
 * OWNERSHIP:
 *   Registries and batches copy descriptors and findings by value. Returned
 *   descriptors and findings are value snapshots; no internal pointer escapes.
 *   Caller-owned strings are copied into bounded storage during upsert.
 *
 * STABILITY:
 *   Public structures carry structure size and API version fields. Providers
 *   may run in-process or out-of-process because no implementation pointer or
 *   toolkit type crosses this boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_DIAGNOSTIC_PROVIDER_H
#define UMICOM_DIAGNOSTICS_DIAGNOSTIC_PROVIDER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_PROVIDER_API_VERSION 1U
#define UMI_DIAGNOSTIC_PROVIDER_ID_CAPACITY 128U
#define UMI_DIAGNOSTIC_PROVIDER_LABEL_CAPACITY 256U
#define UMI_DIAGNOSTIC_TOOL_NAME_CAPACITY 128U
#define UMI_DIAGNOSTIC_TOOL_VERSION_CAPACITY 64U
#define UMI_DIAGNOSTIC_INFORMATION_URI_CAPACITY 1024U
#define UMI_DIAGNOSTIC_RULE_NAME_CAPACITY 256U
#define UMI_DIAGNOSTIC_HELP_URI_CAPACITY 1024U
#define UMI_DIAGNOSTIC_LOGICAL_LOCATION_CAPACITY 256U
#define UMI_DIAGNOSTIC_SNIPPET_CAPACITY 1024U
#define UMI_DIAGNOSTIC_FIX_DESCRIPTION_CAPACITY 512U
#define UMI_DIAGNOSTIC_REPLACEMENT_CAPACITY 2048U
#define UMI_DIAGNOSTIC_SUPPRESSION_JUSTIFICATION_CAPACITY 512U

typedef enum UmiDiagnosticProviderCategory {
    UMI_DIAGNOSTIC_PROVIDER_OTHER = 0,
    UMI_DIAGNOSTIC_PROVIDER_COMPILER = 1,
    UMI_DIAGNOSTIC_PROVIDER_LINKER = 2,
    UMI_DIAGNOSTIC_PROVIDER_LINTER = 3,
    UMI_DIAGNOSTIC_PROVIDER_LANGUAGE_SERVER = 4,
    UMI_DIAGNOSTIC_PROVIDER_TEST_RUNNER = 5,
    UMI_DIAGNOSTIC_PROVIDER_SECURITY_SCANNER = 6,
    UMI_DIAGNOSTIC_PROVIDER_RUNTIME = 7,
    UMI_DIAGNOSTIC_PROVIDER_VALIDATOR = 8
} UmiDiagnosticProviderCategory;


typedef enum UmiDiagnosticProviderBaselineHint {
    UMI_DIAGNOSTIC_PROVIDER_BASELINE_NONE = 0,
    UMI_DIAGNOSTIC_PROVIDER_BASELINE_NEW = 1,
    UMI_DIAGNOSTIC_PROVIDER_BASELINE_UNCHANGED = 2,
    UMI_DIAGNOSTIC_PROVIDER_BASELINE_UPDATED = 3,
    UMI_DIAGNOSTIC_PROVIDER_BASELINE_ABSENT = 4
} UmiDiagnosticProviderBaselineHint;

typedef enum UmiDiagnosticProviderSuppressionKind {
    UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_NONE = 0,
    UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_EXTERNAL = 1,
    UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_IN_SOURCE = 2
} UmiDiagnosticProviderSuppressionKind;

typedef enum UmiDiagnosticProviderSuppressionStatus {
    UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_STATUS_NONE = 0,
    UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_ACCEPTED = 1,
    UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_UNDER_REVIEW = 2,
    UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_REJECTED = 3
} UmiDiagnosticProviderSuppressionStatus;

typedef struct UmiDiagnosticProviderDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DIAGNOSTIC_PROVIDER_ID_CAPACITY];
    char label[UMI_DIAGNOSTIC_PROVIDER_LABEL_CAPACITY];
    char tool_name[UMI_DIAGNOSTIC_TOOL_NAME_CAPACITY];
    char tool_version[UMI_DIAGNOSTIC_TOOL_VERSION_CAPACITY];
    char information_uri[UMI_DIAGNOSTIC_INFORMATION_URI_CAPACITY];
    UmiDiagnosticProviderCategory category;
    UmiDiagnosticKind default_kind;
    int32_t priority;
    int enabled;
    int incremental;
    int supports_fixes;
    int supports_sarif;
} UmiDiagnosticProviderDescriptor;

typedef struct UmiDiagnosticProviderFinding {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDiagnosticSnapshot diagnostic;
    char provider_id[UMI_DIAGNOSTIC_PROVIDER_ID_CAPACITY];
    char rule_name[UMI_DIAGNOSTIC_RULE_NAME_CAPACITY];
    char help_uri[UMI_DIAGNOSTIC_HELP_URI_CAPACITY];
    char logical_location[UMI_DIAGNOSTIC_LOGICAL_LOCATION_CAPACITY];
    char snippet[UMI_DIAGNOSTIC_SNIPPET_CAPACITY];
    char fix_description[UMI_DIAGNOSTIC_FIX_DESCRIPTION_CAPACITY];
    char replacement_text[UMI_DIAGNOSTIC_REPLACEMENT_CAPACITY];
    char suppression_justification[
        UMI_DIAGNOSTIC_SUPPRESSION_JUSTIFICATION_CAPACITY];
    uint64_t fingerprint;
    uint64_t content_fingerprint;
    uint64_t run_id;
    uint64_t source_revision;
    uint32_t replacement_start_line;
    uint32_t replacement_start_column;
    uint32_t replacement_end_line;
    uint32_t replacement_end_column;
    UmiDiagnosticProviderBaselineHint baseline_hint;
    UmiDiagnosticProviderSuppressionKind suppression_kind;
    UmiDiagnosticProviderSuppressionStatus suppression_status;
    int has_fix;
    int suppressed;
} UmiDiagnosticProviderFinding;

typedef struct UmiDiagnosticProviderRegistrySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t provider_count;
    size_t enabled_count;
    size_t incremental_count;
    size_t fix_provider_count;
    size_t sarif_provider_count;
    uint64_t revision;
} UmiDiagnosticProviderRegistrySnapshot;

typedef struct UmiDiagnosticProviderBatchSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t finding_count;
    size_t fixable_count;
    size_t provider_count;
    size_t by_severity[UMI_DIAGNOSTIC_SEVERITY_COUNT];
    uint64_t minimum_run_id;
    uint64_t maximum_run_id;
    uint64_t revision;
} UmiDiagnosticProviderBatchSnapshot;

typedef struct UmiDiagnosticProviderRegistry UmiDiagnosticProviderRegistry;
typedef struct UmiDiagnosticProviderBatch UmiDiagnosticProviderBatch;

UmiStatus umi_diagnostic_provider_descriptor_validate(
    const UmiDiagnosticProviderDescriptor *descriptor);
UmiStatus umi_diagnostic_provider_registry_create(
    UmiDiagnosticProviderRegistry **out_registry);
void umi_diagnostic_provider_registry_destroy(
    UmiDiagnosticProviderRegistry *registry);
UmiStatus umi_diagnostic_provider_registry_clear(
    UmiDiagnosticProviderRegistry *registry);
UmiStatus umi_diagnostic_provider_registry_upsert(
    UmiDiagnosticProviderRegistry *registry,
    const UmiDiagnosticProviderDescriptor *descriptor);
UmiStatus umi_diagnostic_provider_registry_remove(
    UmiDiagnosticProviderRegistry *registry,
    const char *provider_id);
UmiStatus umi_diagnostic_provider_registry_find(
    const UmiDiagnosticProviderRegistry *registry,
    const char *provider_id,
    UmiDiagnosticProviderDescriptor *out_descriptor);
UmiStatus umi_diagnostic_provider_registry_at(
    const UmiDiagnosticProviderRegistry *registry,
    size_t position,
    UmiDiagnosticProviderDescriptor *out_descriptor);
UmiStatus umi_diagnostic_provider_registry_snapshot(
    const UmiDiagnosticProviderRegistry *registry,
    UmiDiagnosticProviderRegistrySnapshot *out_snapshot);
size_t umi_diagnostic_provider_registry_count(
    const UmiDiagnosticProviderRegistry *registry);
uint64_t umi_diagnostic_provider_registry_revision(
    const UmiDiagnosticProviderRegistry *registry);

UmiStatus umi_diagnostic_provider_finding_init(
    UmiDiagnosticProviderFinding *finding,
    const char *provider_id,
    const UmiDiagnosticSnapshot *diagnostic);
UmiStatus umi_diagnostic_provider_finding_validate(
    const UmiDiagnosticProviderFinding *finding);
uint64_t umi_diagnostic_provider_fingerprint(
    const UmiDiagnosticProviderFinding *finding);
uint64_t umi_diagnostic_provider_content_fingerprint(
    const UmiDiagnosticProviderFinding *finding);

UmiStatus umi_diagnostic_provider_batch_create(
    UmiDiagnosticProviderBatch **out_batch);
void umi_diagnostic_provider_batch_destroy(UmiDiagnosticProviderBatch *batch);
UmiStatus umi_diagnostic_provider_batch_clear(UmiDiagnosticProviderBatch *batch);
UmiStatus umi_diagnostic_provider_batch_upsert(
    UmiDiagnosticProviderBatch *batch,
    const UmiDiagnosticProviderFinding *finding);
UmiStatus umi_diagnostic_provider_batch_remove(
    UmiDiagnosticProviderBatch *batch,
    const char *provider_id,
    const char *diagnostic_id);
UmiStatus umi_diagnostic_provider_batch_find(
    const UmiDiagnosticProviderBatch *batch,
    const char *provider_id,
    const char *diagnostic_id,
    UmiDiagnosticProviderFinding *out_finding);
UmiStatus umi_diagnostic_provider_batch_at(
    const UmiDiagnosticProviderBatch *batch,
    size_t position,
    UmiDiagnosticProviderFinding *out_finding);
UmiStatus umi_diagnostic_provider_batch_snapshot(
    const UmiDiagnosticProviderBatch *batch,
    UmiDiagnosticProviderBatchSnapshot *out_snapshot);
size_t umi_diagnostic_provider_batch_count(
    const UmiDiagnosticProviderBatch *batch);
uint64_t umi_diagnostic_provider_batch_revision(
    const UmiDiagnosticProviderBatch *batch);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_DIAGNOSTIC_PROVIDER_H */
