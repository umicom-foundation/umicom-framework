/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance_persistence.h
 *
 * PURPOSE:
 *   Declare the portable text codec for user-created appearance profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_APPEARANCE_PERSISTENCE_H
#define UMICOM_UI_APPEARANCE_PERSISTENCE_H

#include "umicom/ui/appearance.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_APPEARANCE_TEXT_CAPACITY 4096U

/**
 * Write ui appearance profile in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ui_appearance_profile_encode(
    const UmiUiAppearanceProfile *profile,
    char *out_text,
    size_t capacity);
/**
 * Read ui appearance profile into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ui_appearance_profile_decode(
    const char *text,
    UmiUiAppearanceProfile *out_profile);

#ifdef __cplusplus
}
#endif

#endif
