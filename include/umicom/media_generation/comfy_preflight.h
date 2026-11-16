/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_preflight.h
 *
 * PURPOSE:
 *   Report missing ComfyUI classes and required fields against one owned catalogue observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_PREFLIGHT_H
#define UMICOM_MEDIA_GENERATION_COMFY_PREFLIGHT_H
#include "umicom/media_generation/comfy_nodes.h"
#include "umicom/media_generation/comfy_prompt.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_PREFLIGHT_STORED_ISSUES 256U
typedef enum UmiComfyPreflightIssueKind {
    UMI_COMFY_PREFLIGHT_CLASS_MISSING = 1,
    UMI_COMFY_PREFLIGHT_INPUT_MISSING,
    UMI_COMFY_PREFLIGHT_DEPRECATED,
    UMI_COMFY_PREFLIGHT_EXPERIMENTAL,
    UMI_COMFY_PREFLIGHT_API_NODE
} UmiComfyPreflightIssueKind;
typedef struct UmiComfyPreflightIssue {
    UmiComfyPreflightIssueKind kind;
    char node_id[129], class_type[129], input_name[129];
} UmiComfyPreflightIssue;
typedef struct UmiComfyPreflightSummary {
    size_t node_count, known_node_count, unknown_node_count;
    size_t missing_required_count;
    size_t deprecated_count, experimental_count, api_node_count;
    size_t observed_output_count, unknown_output_flag_count;
    size_t issue_count, stored_issue_count, omitted_issue_count;
    /* True only when every class was observed and every declared required
     * field was present. It is not a value, graph, model or readiness verdict. */
    bool all_node_requirements_observed;
    uint64_t connection_revision;
    unsigned char connection_sha256[UMI_SHA256_BYTES];
    unsigned char request_sha256[UMI_SHA256_BYTES];
    unsigned char catalogue_sha256[UMI_SHA256_BYTES];
} UmiComfyPreflightSummary;
typedef struct UmiComfyPreflightReport UmiComfyPreflightReport;
/* Compare the complete prepared workflow with an owned /object_info observation.
 * Both must match current saved settings and revision; a mismatch returns BUSY.
 * The host supplies those settings: this does not read a store or check freshness.
 * Inputs remain valid and immutable for this call; serialize access/destruction.
 *
 * Every workflow node is inspected in source order, including unused nodes.
 * Missing class: record one issue and skip its unknowable input requirements.
 * Known class: check required field presence in catalogue source order, followed
 * by true deprecated, experimental and api_node flags in that fixed order.
 * Missing optional/hidden fields and extra workflow fields are not errors.
 * A present null counts as present; defaults never supply absent required fields.
 * Custom values, arrays/links, types, cycles and output connectivity are not
 * interpreted. Use typed workflow planning for host-declared typed graphs and
 * the real server for its installed schema validation. Never treat a clean
 * report as permission to execute Python/custom nodes or submit network work.
 *
 * Count all findings, retaining the first 256. omitted_issue_count exposes
 * truncation without stopping inspection. Missing requirements are report data
 * with OK, not a parsing error. On cancellation, allocation or identity refusal,
 * *out is unchanged. Initialize *out=NULL. Copies survive owner destruction.
 * No I/O or logging; report storage is cleared when destroyed. */
UmiStatus UmiComfyPreflightCreate(const UmiProviderConnection *current,
    uint64_t current_revision, const UmiComfyPrompt *prompt,
    const UmiComfyNodeCatalogue *catalogue, const UmiCancellationToken *cancel,
    UmiComfyPreflightReport **out);
void UmiComfyPreflightDestroy(UmiComfyPreflightReport *report);
/* Copy accessors preserve outputs on refusal; do not overlap owned storage. */
UmiStatus UmiComfyPreflightInspect(const UmiComfyPreflightReport *report,
    UmiComfyPreflightSummary *out);
UmiStatus UmiComfyPreflightIssueAt(const UmiComfyPreflightReport *report,
    size_t index, UmiComfyPreflightIssue *out);
#ifdef __cplusplus
}
#endif
#endif
