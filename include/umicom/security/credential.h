/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/credential.h
 *
 * PURPOSE:
 *   Store credential references and expiry metadata without retaining credential
 *   secret values in Framework memory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_CREDENTIAL_H
#define UMICOM_SECURITY_CREDENTIAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/security/identity.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CREDENTIAL_ID_CAPACITY 128U
#define UMI_CREDENTIAL_PROVIDER_CAPACITY 96U
#define UMI_CREDENTIAL_REFERENCE_CAPACITY 256U
#define UMI_CREDENTIAL_REGISTRY_MAX 512U
/**
 * Represent the credential record data shared with callers of this public contract.
 */
typedef struct UmiCredentialRecord {
    char credential_id[UMI_CREDENTIAL_ID_CAPACITY];
    char principal_id[UMI_IDENTITY_ID_CAPACITY];
    char provider_id[UMI_CREDENTIAL_PROVIDER_CAPACITY];
    char secret_reference[UMI_CREDENTIAL_REFERENCE_CAPACITY];
    uint64_t expires_at_ns;
    int active;
} UmiCredentialRecord;
/**
 * Represent the credential registry data shared with callers of this public contract.
 */
typedef struct UmiCredentialRegistry UmiCredentialRegistry;
/**
 * Initialise credential registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_credential_registry_create(UmiCredentialRegistry **out_registry);
/**
 * Release or reset state held by credential registry so the same storage can be reused
 * safely.
 */
void umi_credential_registry_destroy(UmiCredentialRegistry *registry);
/**
 * Add credential registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_credential_registry_register(UmiCredentialRegistry *registry,
                                           const UmiCredentialRecord *record);
/**
 * Provide the credential registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_credential_registry_get(const UmiCredentialRegistry *registry,
                                      const char *credential_id,
                                      UmiCredentialRecord *out_record);
/**
 * Provide the credential registry revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_credential_registry_revoke(UmiCredentialRegistry *registry,
                                         const char *credential_id);
/**
 * Return the number of records represented by credential registry without changing their
 * state.
 */
size_t umi_credential_registry_count(const UmiCredentialRegistry *registry);
#ifdef __cplusplus
}
#endif

#endif
