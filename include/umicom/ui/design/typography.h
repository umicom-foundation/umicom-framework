/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/typography.h
 *
 * PURPOSE:
 *   Define validated toolkit-neutral typography specifications for semantic text roles.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_TYPOGRAPHY_H
#define INCLUDE_UMICOM_UI_DESIGN_TYPOGRAPHY_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the design typography data shared with callers of this public contract.
 */
typedef struct UmiDesignTypography { char family[UMI_DESIGN_NAME_CAPACITY]; double size; uint16_t weight; double line_height; double letter_spacing; } UmiDesignTypography;

/* Initialise a typography specification with bounded family text and validated metrics. */
UmiStatus umi_design_typography_init(UmiDesignTypography *spec, const char *family, double size, uint16_t weight, double line_height);
/* Return one when the typography metrics can be rendered consistently. */
int umi_design_typography_valid(const UmiDesignTypography *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_typography_archive_encode(const UmiDesignTypography *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_typography_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignTypography *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_design_typography_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDesignTypography *expected, UmiDesignTypography *value);


#ifdef __cplusplus
}
#endif

#endif
