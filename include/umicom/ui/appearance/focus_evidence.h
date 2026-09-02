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

#ifdef __cplusplus
}
#endif
#endif
