/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/breakpoint_inheritance.c
 *
 * PURPOSE:
 *   Resolve bounded parent chains for adaptive variants without accepting inheritance cycles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/breakpoint_inheritance.h"
#include <string.h>

/* Walk parent links with duplicate detection so malformed visual documents cannot recurse forever. */
UmiStatus umi_designer_breakpoint_inheritance_resolve(const UmiDesignerAdaptiveVariantSet *set,
                                                      const char *leaf_variant_id,
                                                      UmiDesignerBreakpointInheritance *out_chain)
{
    const UmiDesignerAdaptiveVariant *current;
    size_t i;
    if (set == NULL || leaf_variant_id == NULL || out_chain == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_chain, 0, sizeof *out_chain);
    current = umi_designer_adaptive_variant_set_find(set, leaf_variant_id);
    while (current != NULL) {
        for (i = 0U; i < out_chain->count; ++i)
            if (strcmp(out_chain->variant_ids[i], current->variant_id) == 0) return UMI_STATUS_INVALID_STATE;
        if (out_chain->count >= UMI_DESIGNER_ADAPTIVE_MAX_VARIANTS) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (umi_designer_adaptive_copy_text(out_chain->variant_ids[out_chain->count],
                                            sizeof out_chain->variant_ids[out_chain->count],
                                            current->variant_id) != UMI_STATUS_OK)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        ++out_chain->count;
        if (current->parent_id[0] == '\0') break;
        current = umi_designer_adaptive_variant_set_find(set, current->parent_id);
        if (current == NULL) return UMI_STATUS_NOT_FOUND;
    }
    return out_chain->count > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
