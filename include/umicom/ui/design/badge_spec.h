/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/badge_spec.h
 *
 * PURPOSE:
 *   Define compact status and metadata badges with semantic intent.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_BADGE_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_BADGE_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/base/value_archive.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the design badge spec data shared with callers of this public contract.
 */
typedef struct UmiDesignBadgeSpec {
    char text[UMI_DESIGN_TEXT_CAPACITY];
    UmiDesignSemanticRole role;
    int outlined;
} UmiDesignBadgeSpec;

/* Initialise the semantic badge spec specification. */
UmiStatus umi_design_badge_spec_init(UmiDesignBadgeSpec *spec, const char *text, UmiDesignSemanticRole role, int outlined);
/* Return one when the semantic specification is internally consistent. */
int umi_design_badge_spec_valid(const UmiDesignBadgeSpec *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_badge_spec_archive_encode(const UmiDesignBadgeSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_badge_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignBadgeSpec *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_design_badge_spec_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDesignBadgeSpec *expected, UmiDesignBadgeSpec *value);


#ifdef __cplusplus
}
#endif

#endif
