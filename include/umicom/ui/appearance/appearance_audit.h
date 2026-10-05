/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_audit.h
 *
 * PURPOSE:
 *   Aggregate appearance accessibility, scaling, typography and renderer-parity findings into one audit result.
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
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_AUDIT_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_AUDIT_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance appearance audit data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAppearanceAudit {
    char audit_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t checks;
    uint32_t warnings;
    uint32_t errors;
    bool passed;
} UmiAppearanceAppearanceAudit;

/* Initialise one appearance audit record with deterministic defaults. */
UmiStatus umi_appearance_audit_init(UmiAppearanceAppearanceAudit *item);
/* Validate the required production invariants for this appearance audit. */
int umi_appearance_audit_is_valid(const UmiAppearanceAppearanceAudit *item);
/* Recalculate the audit pass gate from blocking appearance errors. */
void umi_appearance_audit_evaluate(UmiAppearanceAppearanceAudit *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_audit_archive_encode(const UmiAppearanceAppearanceAudit *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_audit_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceAppearanceAudit *value);

#ifdef __cplusplus
}
#endif
#endif
