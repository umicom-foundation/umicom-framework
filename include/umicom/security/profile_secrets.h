/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/profile_secrets.h
 * PURPOSE: Require a fresh local-profile check for each credential operation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_PROFILE_SECRETS_H
#define UMICOM_SECURITY_PROFILE_SECRETS_H
#include "umicom/security/local_profile.h"
#include "umicom/security/platform_secrets.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProfileSecrets UmiProfileSecrets;

/* This service composes the existing profile verifier and secret provider.
 * It never keeps an unlocked flag or caches a password or key. Every Get, Set
 * and Remove verifies the password again before invoking the provider.
 *
 * Create is an adapter boundary for trusted hosts and isolated tests: profiles
 * and provider must already belong to the SAME application/profile namespace.
 * Success retains profiles and takes provider ownership, clearing its struct.
 * Failure leaves both inputs owned by the caller and sets *out to NULL.
 * Serialise calls on this object AND any other users of the retained profile
 * store. Join all callers before Destroy; callbacks must not re-enter it.
 * Platform constructs matching private adapters for the current OS user.
 * Unsupported platforms return UNAVAILABLE without a plaintext fallback. */
UmiStatus UmiProfileSecretsCreate(UmiLocalProfileStore *profiles,
    const char *profile_name, UmiSecretProvider *provider, UmiProfileSecrets **out);
UmiStatus UmiProfileSecretsPlatform(const char *application_id,
    const char *profile_name, UmiProfileSecrets **out);
void UmiProfileSecretsDestroy(UmiProfileSecrets *secrets);

/* Validate the common native namespace without accessing any OS store.
 * Application: 1..95 lower-case ASCII letters, digits, '.', '_' or '-', first
 * character a letter. Profile: the canonical, lower-case local-profile name.
 * Reject mixed case rather than silently selecting different saved settings. */
UmiStatus UmiProfileSecretsScopeValidate(const char *application_id, const char *profile_name);

/* Register creates only a verifier and refuses an existing profile. It does
 * not reset a forgotten password or alter any key. Passwords are opaque
 * 12..256 UTF-8 bytes, as defined by the existing local-profile service. */
UmiStatus UmiProfileSecretsRegister(UmiProfileSecrets *secrets, const char *password);

/* Names are non-secret native aliases (1..96 lower-case ASCII characters).
 * Set explicitly creates OR REPLACES a key of 1..2048 bytes. Remove affects
 * only local storage, never the remote account. Neither operation edits a
 * connection database. Concurrent writers in other objects/processes are not
 * compare-and-swap: hosts must coordinate them or accept the last completed
 * write. Never promise an atomic credential-and-metadata transaction.
 *
 * Get clears the whole output on entry and failure, and rejects empty or
 * unterminated provider results. Input strings and output must not overlap.
 * Caller buffers remain caller-owned; clear passwords and keys after use.
 * This is application policy, not isolation against other same-OS-user
 * processes. The local profile password does not encrypt Credential Manager.
 * A successful Get checks local availability, not remote account validity. */
UmiStatus UmiProfileSecretsGet(UmiProfileSecrets *secrets, const char *password,
    const char *name, char *out_value, size_t capacity);
UmiStatus UmiProfileSecretsSet(UmiProfileSecrets *secrets, const char *password,
    const char *name, const char *value);
UmiStatus UmiProfileSecretsRemove(UmiProfileSecrets *secrets, const char *password,
    const char *name);

#ifdef __cplusplus
}
#endif
#endif
