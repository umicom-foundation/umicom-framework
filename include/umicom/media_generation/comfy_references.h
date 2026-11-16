/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_references.h
 *
 * PURPOSE:
 *   Inspect copied ComfyUI reference candidates without claiming custom-node execution semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_REFERENCES_H
#define UMICOM_MEDIA_GENERATION_COMFY_REFERENCES_H
#include "umicom/media_generation/comfy_nodes.h"
#include "umicom/media_generation/comfy_prompt.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_REFERENCE_LIMIT 8192U
/* A candidate is a direct input array of length two whose first item is text.
 * It is deliberately broader than a verified link: custom schemas and rawLink
 * handling are not interpreted. No value is rewritten or coerced. */
typedef struct UmiComfyReference {
    char node_id[129], input_name[129], target_id[129], target_class[129];
    char output_type[129];
    int64_t output_slot;
    size_t declared_output_count;
    bool slot_known, target_found, output_count_known, output_observed;
} UmiComfyReference;
typedef struct UmiComfyReferenceSummary {
    size_t node_count, candidate_count, other_array_count;
    size_t unsupported_slot_count, missing_target_count;
    size_t unobserved_output_count, out_of_range_count, observed_output_count;
    uint64_t connection_revision;
    unsigned char connection_sha256[UMI_SHA256_BYTES];
    unsigned char request_sha256[UMI_SHA256_BYTES];
    unsigned char catalogue_sha256[UMI_SHA256_BYTES];
} UmiComfyReferenceSummary;
typedef struct UmiComfyReferenceInventory UmiComfyReferenceInventory;
/* Review every node's direct input fields in source order. Recognize pairs
 * starting with a string; count other direct arrays without inspecting nested
 * values. A slot is known only for nonnegative decimal integer spelling within
 * INT64_MAX, with no minus sign, exponent, float or boolean coercion. Other slot
 * values remain reported candidates with slot_known=false, never slot zero.
 *
 * Resolve the candidate target by exact decoded node ID. When its class exists
 * in the captured catalogue, copy output count and (for an in-range known slot)
 * type text. Unknown class metadata differs from zero declared outputs. Counts
 * are independent: a candidate can have both an unsupported slot and a missing
 * target. unobserved_output_count counts found targets whose class is absent.
 *
 * Require prompt and catalogue to match the supplied connection/revision;
 * BUSY means mismatched observations. These are host-provided bytes, not live
 * server evidence. No input-type compatibility, rawLink handling, cycle check,
 * reachability, generation readiness or permission judgment is made. Even a
 * fully observed pair can be custom-node data rather than a dependency.
 *
 * Limits: existing prompt/tree limits, 8192 candidates, and 128 UTF-8 bytes for
 * candidate input names and target text (empty target remains a missing target).
 * Over-limit text is refused, never truncated. No partial inventory is published.
 * Initialize *out=NULL; every refusal preserves it. Inputs are borrowed only
 * during the call. Result owns copies and survives prompt/catalogue destruction.
 * Serialize use/destruction. No I/O, Python loading, provider call or logging. */
UmiStatus UmiComfyReferenceInventoryCreate(const UmiProviderConnection *current,
    uint64_t current_revision, const UmiComfyPrompt *prompt,
    const UmiComfyNodeCatalogue *catalogue, const UmiCancellationToken *cancel,
    UmiComfyReferenceInventory **out);
void UmiComfyReferenceInventoryDestroy(UmiComfyReferenceInventory *inventory);
/* Copy accessors leave outputs unchanged on refusal. Do not overlap owned
 * storage. At returns NOT_FOUND for an index outside the complete inventory. */
UmiStatus UmiComfyReferenceInventoryInspect(const UmiComfyReferenceInventory *inventory,
    UmiComfyReferenceSummary *out);
UmiStatus UmiComfyReferenceInventoryAt(const UmiComfyReferenceInventory *inventory,
    size_t index, UmiComfyReference *out);
#ifdef __cplusplus
}
#endif
#endif
