/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/tree_spec.h
 *
 * PURPOSE:
 *   Define virtualised hierarchical tree depth, selection and checkbox semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TREE_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_TREE_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design tree spec data shared with callers of this public contract.
 */
typedef struct UmiDesignTreeSpec {
    uint16_t maximum_depth;
    int virtualised;
    int multi_select;
    int checkboxes;
} UmiDesignTreeSpec;

/* Initialise the semantic tree spec specification. */
UmiStatus umi_design_tree_spec_init(UmiDesignTreeSpec *spec, uint16_t maximum_depth, int virtualised, int multi_select, int checkboxes);
/* Return one when the semantic specification is internally consistent. */
int umi_design_tree_spec_valid(const UmiDesignTreeSpec *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_tree_spec_archive_encode(const UmiDesignTreeSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_tree_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignTreeSpec *value);

#ifdef __cplusplus
}
#endif

#endif
