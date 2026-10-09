/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/local_profile_platform_internal.h
 * PURPOSE: Share the existing native verifier and credential compatibility boundary with local
 * persistence adapters. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LOCAL_PROFILE_PLATFORM_INTERNAL_H
#define UMICOM_LOCAL_PROFILE_PLATFORM_INTERNAL_H
#include "umicom/security/local_profile.h"
/* On success the caller owns backend.context and must call backend.destroy.
 * The native backend supplies CNG cryptography and legacy vault compatibility. */
UmiStatus UmiLocalProfilePlatformBackend(const char *application_id, UmiLocalProfileBackend *out);
/* The per-user/application mutex also serializes old vault and database clients.
 * Calls must enter and leave on the same worker thread. */
UmiStatus UmiLocalProfilePlatformEnter(void *context);
void UmiLocalProfilePlatformLeave(void *context);
#endif
