/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance_catalogue.h
 *
 * PURPOSE:
 *   Publish reusable appearance presets and populate an application appearance
 *   model without placing colours or font choices in a client application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_APPEARANCE_CATALOGUE_H
#define UMICOM_UI_APPEARANCE_CATALOGUE_H

#include "umicom/ui/appearance.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the number of standard profiles available to every application. */
size_t umi_ui_appearance_catalogue_count(void);

/** Copy one standard profile by position without exposing mutable storage. */
UmiStatus umi_ui_appearance_catalogue_at(size_t index, UmiUiAppearanceProfile *out_profile);

/** Find a standard profile by its stable identifier. */
UmiStatus umi_ui_appearance_catalogue_find(const char *profile_id,
                                           UmiUiAppearanceProfile *out_profile);

/** Add all standard profiles and one editable custom profile to a model. */
UmiStatus umi_ui_appearance_catalogue_populate(UmiUiAppearanceModel *model);

#ifdef __cplusplus
}
#endif

#endif
