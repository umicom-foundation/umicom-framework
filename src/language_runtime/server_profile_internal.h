/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/server_profile_internal.h
 * PURPOSE: Validate fixed profile text before native launch or transport ownership transfer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SERVER_PROFILE_INTERNAL_H
#define UMICOM_LANGUAGE_RUNTIME_SERVER_PROFILE_INTERNAL_H
#include "umicom/language/server_profile.h"
#include <string.h>
/* Registry values and caller-built values have the same contract. Check all
 * terminators before a parser or native launcher is allowed to use strlen. */
static inline UmiStatus LanguageProfileText(const UmiLanguageServerProfile *profile)
{
    if (profile == NULL || memchr(profile->id, 0, sizeof(profile->id)) == NULL ||
        memchr(profile->display_name, 0, sizeof(profile->display_name)) == NULL ||
        memchr(profile->executable, 0, sizeof(profile->executable)) == NULL ||
        memchr(profile->arguments, 0, sizeof(profile->arguments)) == NULL ||
        memchr(profile->language_ids, 0, sizeof(profile->language_ids)) == NULL || profile->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
#endif
