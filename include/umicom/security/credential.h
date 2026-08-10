/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/credential.h
 *
 * PURPOSE:
 *   Store credential references and expiry metadata without retaining credential
 *   secret values in Framework memory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiCredentialRecord {
    char credential_id[UMI_CREDENTIAL_ID_CAPACITY];
    char principal_id[UMI_IDENTITY_ID_CAPACITY];
    char provider_id[UMI_CREDENTIAL_PROVIDER_CAPACITY];
    char secret_reference[UMI_CREDENTIAL_REFERENCE_CAPACITY];
    uint64_t expires_at_ns;
    int active;
} UmiCredentialRecord;
typedef struct UmiCredentialRegistry UmiCredentialRegistry;
UmiStatus umi_credential_registry_create(UmiCredentialRegistry **out_registry);
void umi_credential_registry_destroy(UmiCredentialRegistry *registry);
UmiStatus umi_credential_registry_register(UmiCredentialRegistry *registry,
                                           const UmiCredentialRecord *record);
UmiStatus umi_credential_registry_get(const UmiCredentialRegistry *registry,
                                      const char *credential_id,
                                      UmiCredentialRecord *out_record);
UmiStatus umi_credential_registry_revoke(UmiCredentialRegistry *registry,
                                         const char *credential_id);
size_t umi_credential_registry_count(const UmiCredentialRegistry *registry);
#ifdef __cplusplus
}
#endif

#endif
