/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/platform_secrets.h
 * PURPOSE: Save user-supplied provider keys in the operating system credential store.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_PLATFORM_SECRETS_H
#define UMICOM_SECURITY_PLATFORM_SECRETS_H
#include "umicom/security/secrets.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_SECRET_SCOPE_CAPACITY 97U
#define UMI_PLATFORM_SECRET_NAME_CAPACITY 97U
#define UMI_PLATFORM_SECRET_VALUE_CAPACITY 2049U

/* Create a provider for the current OS user. Windows uses Credential Manager
 * with local-machine persistence, which does not request credential roaming.
 * Unsupported platforms return UNAVAILABLE; there is no plaintext fallback.
 * Application, profile and secret names are non-secret aliases: 1..96 ASCII
 * characters, beginning with a lower-case letter, followed by lower-case
 * letters, digits, '.', '_' or '-'. These rules keep namespaces unambiguous.
 *
 * Initialise out_provider to zero and dispose it before reuse. Failure leaves
 * it unchanged. The provider owns a copied namespace, never a cached key.
 * Use the existing get/set/remove helpers or register it in a secret registry.
 * Set replaces the named value; remove affects only that stored value. Neither
 * operation revokes a key at its remote provider. Values contain 1..2048 bytes
 * before the terminating NUL; bytes are preserved without text conversion.
 * Get clears the supplied output buffer before any lookup, including failures.
 * Calls are synchronous: use a worker for UI callers and join it before dispose.
 * Serialise writes that target the same alias; simultaneous writers are not a
 * compare-and-swap operation. Set must follow explicit user intent to save a key.
 *
 * A profile name separates records, but is not an access-control boundary.
 * Other processes running as the same OS user may read these generic secrets.
 * A local profile password does not encrypt this store. The application must
 * enforce its unlocked-profile policy before resolving any secret reference.
 * Store only references and account metadata in project or database records;
 * clear caller-owned key buffers after use with umi_secret_clear. */
UmiStatus umi_secret_provider_platform(const char *application_id,
    const char *profile_id, UmiSecretProvider *out_provider);

#ifdef __cplusplus
}
#endif
#endif
