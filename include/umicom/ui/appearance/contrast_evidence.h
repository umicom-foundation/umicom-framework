/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/contrast_evidence.h
 *
 * PURPOSE:
 *   Persist auditable foreground/background token and ratio evidence for conformance reports.
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
#ifndef UMICOM_UI_APPEARANCE_CONTRAST_EVIDENCE_H
#define UMICOM_UI_APPEARANCE_CONTRAST_EVIDENCE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance contrast evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceContrastEvidence {
    char evidence_id[UMI_APPEARANCE_ID_CAPACITY];
    char foreground_token[UMI_APPEARANCE_TOKEN_CAPACITY];
    char background_token[UMI_APPEARANCE_TOKEN_CAPACITY];
    double ratio;
    bool passed;
} UmiAppearanceContrastEvidence;

/* Initialise one contrast evidence record with deterministic defaults. */
UmiStatus umi_appearance_contrast_evidence_init(UmiAppearanceContrastEvidence *item);
/* Validate the required production invariants for this contrast evidence. */
int umi_appearance_contrast_evidence_is_valid(const UmiAppearanceContrastEvidence *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_contrast_evidence_archive_encode(const UmiAppearanceContrastEvidence *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_contrast_evidence_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceContrastEvidence *value);

#ifdef __cplusplus
}
#endif
#endif
