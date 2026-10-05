/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/list_spec.h
 *
 * PURPOSE:
 *   Define virtualised list selection, estimated size and row-density semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_LIST_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_LIST_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design list spec data shared with callers of this public contract.
 */
typedef struct UmiDesignListSpec {
    size_t estimated_items;
    UmiDesignDensity density;
    int virtualised;
    int multi_select;
} UmiDesignListSpec;

/* Initialise the semantic list spec specification. */
UmiStatus umi_design_list_spec_init(UmiDesignListSpec *spec, size_t estimated_items, UmiDesignDensity density, int virtualised, int multi_select);
/* Return one when the semantic specification is internally consistent. */
int umi_design_list_spec_valid(const UmiDesignListSpec *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_list_spec_archive_encode(const UmiDesignListSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_list_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignListSpec *value);

#ifdef __cplusplus
}
#endif

#endif
