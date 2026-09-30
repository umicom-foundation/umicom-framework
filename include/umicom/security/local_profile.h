/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/local_profile.h
 * PURPOSE: Authenticate local profiles without storing passwords or broker credentials.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_LOCAL_PROFILE_H
#define UMICOM_SECURITY_LOCAL_PROFILE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LOCAL_PROFILE_NAME_CAPACITY 49U
#define UMI_LOCAL_PROFILE_PASSWORD_CAPACITY 257U
#define UMI_LOCAL_PROFILE_ITERATIONS 600000U

/* The only persisted credential is a versioned, salted password verifier.
 * The native backend keeps this record in the current OS user's credential
 * vault. It never contains the password or an IBKR authentication token. */
typedef struct UmiLocalProfileRecord {
    uint32_t version, iterations;
    unsigned char salt[32], verifier[32];
} UmiLocalProfileRecord;

/* Injected backends enable platform adapters and isolated tests. Production
 * implementations must use OS cryptographic randomness and PBKDF2-HMAC-SHA256,
 * atomic create-if-absent and compare-before-remove. All calls on a store must
 * be serialised; only Retain/Release are thread-safe. On Create success the
 * store owns context; on failure the caller still owns it. */
typedef struct UmiLocalProfileBackend {
    void *context;
    UmiStatus (*read)(void *, const char *, UmiLocalProfileRecord *);
    UmiStatus (*create)(void *, const char *, const UmiLocalProfileRecord *);
    UmiStatus (*remove)(void *, const char *, const UmiLocalProfileRecord *);
    UmiStatus (*random)(void *, unsigned char *, size_t);
    UmiStatus (*derive)(void *, const char *, size_t, const UmiLocalProfileRecord *, unsigned char[32]);
    void (*destroy)(void *);
} UmiLocalProfileBackend;
typedef struct UmiLocalProfileStore UmiLocalProfileStore;
UmiStatus UmiLocalProfileStoreCreate(const UmiLocalProfileBackend *backend, UmiLocalProfileStore **out);
/* Windows uses Credential Manager and CNG. Other platforms return UNAVAILABLE
 * until a secure adapter is supplied. There is no plaintext fallback. */
UmiStatus UmiLocalProfileStorePlatform(const char *application_id, UmiLocalProfileStore **out);
void UmiLocalProfileStoreRetain(UmiLocalProfileStore *store);
void UmiLocalProfileStoreRelease(UmiLocalProfileStore *store);
/* Names are case-insensitive ASCII: 3..48 letters, digits, '-' or '_', starting
 * with a letter. Output is lower-case and unchanged on validation failure. */
UmiStatus UmiLocalProfileName(const char *name, char out[UMI_LOCAL_PROFILE_NAME_CAPACITY]);
/* Passwords are opaque UTF-8 bytes: 12..256 bytes, without normalization.
 * Callers clear their input buffers after use. These synchronous functions
 * belong on a worker thread when used by a graphical application. */
UmiStatus UmiLocalProfileRegister(UmiLocalProfileStore *store, const char *name, const char *password);
UmiStatus UmiLocalProfileVerify(UmiLocalProfileStore *store, const char *name, const char *password);
/* Removal requires the current password and removes only the verifier.
 * Saved layouts, trading records and broker sessions remain untouched. */
UmiStatus UmiLocalProfileRemove(UmiLocalProfileStore *store, const char *name, const char *password);
#ifdef __cplusplus
}
#endif
#endif
