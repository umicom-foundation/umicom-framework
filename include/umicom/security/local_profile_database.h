/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/local_profile_database.h
 * PURPOSE: Persist local profile verifiers in a user-local database without broker credentials.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_LOCAL_PROFILE_DATABASE_H
#define UMICOM_SECURITY_LOCAL_PROFILE_DATABASE_H
#include "umicom/security/local_profile.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief Open local account persistence at an explicit absolute database path.
     * @param path UTF-8 filename inside an existing private directory owned by the caller.
     * @param application_id The same application namespace used by the native profile store.
     * @param out_store Receives an owned store; release with UmiLocalProfileStoreRelease.
     * @return OK, UNAVAILABLE when SQLite or native cryptography is absent, or a storage error.
     * @details New accounts store a salt and PBKDF2 verifier, never a password. Existing
     * accounts in the same native credential namespace remain readable without being
     * copied or removed. Account removal requires the current password. Keep this
     * database outside source repositories; it contains local usernames and verifiers.
     * Open on a worker when UI responsiveness matters; calls on one store are serialized.
     * The Windows adapter uses CNG. No broker session, credential or 2FA code is stored.
     */
    UmiStatus UmiLocalProfileStoreDatabase(const char *path, const char *application_id,
                                           UmiLocalProfileStore **out_store);
#ifdef __cplusplus
}
#endif
#endif
