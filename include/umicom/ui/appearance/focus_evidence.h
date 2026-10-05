/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/focus_evidence.h
 *
 * PURPOSE:
 *   Record keyboard reachability and visible-focus evidence for a semantic interactive element.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_FOCUS_EVIDENCE_H
#define UMICOM_UI_APPEARANCE_FOCUS_EVIDENCE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance focus evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceFocusEvidence {
    char element_id[UMI_APPEARANCE_ID_CAPACITY];
    bool keyboard_reachable;
    bool visible_indicator;
    bool order_defined;
    bool passed;
} UmiAppearanceFocusEvidence;

/* Initialise one focus evidence record with deterministic defaults. */
UmiStatus umi_appearance_focus_evidence_init(UmiAppearanceFocusEvidence *item);
/* Validate the required production invariants for this focus evidence. */
int umi_appearance_focus_evidence_is_valid(const UmiAppearanceFocusEvidence *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_focus_evidence_archive_encode(const UmiAppearanceFocusEvidence *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_focus_evidence_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceFocusEvidence *value);

#ifdef __cplusplus
}
#endif
#endif
