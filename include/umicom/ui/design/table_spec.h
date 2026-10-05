/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/table_spec.h
 *
 * PURPOSE:
 *   Define enterprise table column, virtualisation, sorting, filtering and frozen-column semantics.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_TABLE_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_TABLE_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design table spec data shared with callers of this public contract.
 */
typedef struct UmiDesignTableSpec {
    uint16_t columns;
    uint16_t frozen_columns;
    UmiDesignDensity density;
    int virtualised;
    int sortable;
    int filterable;
} UmiDesignTableSpec;

/* Initialise the semantic table spec specification. */
UmiStatus umi_design_table_spec_init(UmiDesignTableSpec *spec, uint16_t columns, uint16_t frozen_columns, UmiDesignDensity density, int virtualised, int sortable, int filterable);
/* Return one when the semantic specification is internally consistent. */
int umi_design_table_spec_valid(const UmiDesignTableSpec *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_table_spec_archive_encode(const UmiDesignTableSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_table_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignTableSpec *value);

#ifdef __cplusplus
}
#endif

#endif
