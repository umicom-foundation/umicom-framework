/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/mnemonic_policy.h
 *
 * PURPOSE:
 *   Govern mnemonic visibility and uniqueness without embedding toolkit accelerator syntax.
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
#ifndef UMICOM_UI_APPEARANCE_MNEMONIC_POLICY_H
#define UMICOM_UI_APPEARANCE_MNEMONIC_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceMnemonicPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    bool show_on_keyboard_intent;
    bool unique_within_scope;
    bool localised;
    bool allow_auto_assignment;
} UmiAppearanceMnemonicPolicy;

/* Initialise one mnemonic policy record with deterministic defaults. */
UmiStatus umi_appearance_mnemonic_policy_init(UmiAppearanceMnemonicPolicy *item);
/* Validate the required production invariants for this mnemonic policy. */
int umi_appearance_mnemonic_policy_is_valid(const UmiAppearanceMnemonicPolicy *item);

#ifdef __cplusplus
}
#endif
#endif
