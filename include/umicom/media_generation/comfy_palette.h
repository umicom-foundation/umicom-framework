/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_palette.h
 *
 * PURPOSE:
 *   Select copied ComfyUI node metadata for native palettes without retaining catalogue storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_PALETTE_H
#define UMICOM_MEDIA_GENERATION_COMFY_PALETTE_H
#include "umicom/media_generation/comfy_nodes.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiComfyNodeOrder {
    UMI_COMFY_NODE_ORDER_SOURCE=0,
    UMI_COMFY_NODE_ORDER_CLASS,
    UMI_COMFY_NODE_ORDER_DISPLAY,
    UMI_COMFY_NODE_ORDER_CATEGORY
} UmiComfyNodeOrder;
typedef struct UmiComfyNodeQuery {
    char search[257];
    /* Empty selects every category. Otherwise match this exact category or
     * its slash-delimited descendants; no leading/trailing/doubled slash. */
    char category[257];
    unsigned required_true_flags, required_false_flags;
    UmiComfyNodeOrder order;
} UmiComfyNodeQuery;
typedef struct UmiComfyNodeSelection UmiComfyNodeSelection;
typedef struct UmiComfyNodeSelectionReview {
    UmiComfyNodeCatalogueReview catalogue;
    size_t source_count, selected_count;
} UmiComfyNodeSelectionReview;
/* Search class, display name and category with ASCII A-Z folding only. Other
 * UTF-8 bytes compare literally; no normalization, fuzzy or locale search.
 * Query arrays must contain terminated valid UTF-8. Both flag masks use only
 * the four published node bits and must not overlap. Every requested true or
 * false bit must be KNOWN; unknown is never silently treated as false.
 *
 * Category hierarchy matching is case-sensitive and slash-boundary aware.
 * Sort by source order or ascending byte order of class/display/category;
 * display/category ties use complete class IDs, which are unique. No globals
 * or locale state are involved. Selection owns copied metadata and catalogue
 * identity, never descriptors or pointers into the original catalogue.
 *
 * Initialize *out=NULL. Failure preserves it. Cancellation is checked between
 * nodes and before/after sorting (not within qsort or allocation). Source and
 * query must remain immutable for this call. No I/O, activation or execution
 * readiness is implied by a match. At most 4096 metadata entries are copied. */
UmiStatus UmiComfyNodeSelect(const UmiComfyNodeCatalogue *source,
    const UmiComfyNodeQuery *query,const UmiCancellationToken *cancel,
    UmiComfyNodeSelection **out);
void UmiComfyNodeSelectionDestroy(UmiComfyNodeSelection *selection);
UmiStatus UmiComfyNodeSelectionInspect(const UmiComfyNodeSelection *selection,
    UmiComfyNodeSelectionReview *out);
UmiStatus UmiComfyNodeSelectionAt(const UmiComfyNodeSelection *selection,
    size_t index,UmiComfyNodeInfo *out);
/* Check exact catalogue bytes, connection fingerprint and revision before
 * resolving descriptors by class ID in a current catalogue. BUSY means the
 * observation changed, including only whitespace. This is not a freshness or
 * authorization check. Both owners must live for the call. Accessors preserve
 * output on refusal; serialize access and destruction and do not overlap it. */
UmiStatus UmiComfyNodeSelectionCheckCatalogue(const UmiComfyNodeSelection *selection,
    const UmiComfyNodeCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
