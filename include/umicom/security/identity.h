/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/identity.h
 *
 * PURPOSE:
 *   Define stable identities and a bounded registry for human users, services,
 *   plug-ins, workers and agents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_IDENTITY_H
#define UMICOM_SECURITY_IDENTITY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_IDENTITY_ID_CAPACITY 128U
#define UMI_IDENTITY_DISPLAY_CAPACITY 160U
#define UMI_IDENTITY_REGISTRY_MAX 512U
typedef enum UmiIdentityKind {
    UMI_IDENTITY_HUMAN = 1,
    UMI_IDENTITY_SERVICE = 2,
    UMI_IDENTITY_PLUGIN = 3,
    UMI_IDENTITY_WORKER = 4,
    UMI_IDENTITY_AGENT = 5
} UmiIdentityKind;
typedef struct UmiIdentityRecord {
    char identity_id[UMI_IDENTITY_ID_CAPACITY];
    char display_name[UMI_IDENTITY_DISPLAY_CAPACITY];
    UmiIdentityKind kind;
    int enabled;
    uint64_t revision;
} UmiIdentityRecord;
typedef struct UmiIdentityRegistry UmiIdentityRegistry;
UmiStatus umi_identity_registry_create(UmiIdentityRegistry **out_registry);
void umi_identity_registry_destroy(UmiIdentityRegistry *registry);
UmiStatus umi_identity_registry_register(UmiIdentityRegistry *registry,
                                         const UmiIdentityRecord *identity);
UmiStatus umi_identity_registry_get(const UmiIdentityRegistry *registry,
                                    const char *identity_id,
                                    UmiIdentityRecord *out_identity);
UmiStatus umi_identity_registry_set_enabled(UmiIdentityRegistry *registry,
                                            const char *identity_id,
                                            int enabled);
size_t umi_identity_registry_count(const UmiIdentityRegistry *registry);
UmiStatus umi_identity_registry_at(const UmiIdentityRegistry *registry,
                                   size_t index,
                                   UmiIdentityRecord *out_identity);
#ifdef __cplusplus
}
#endif

#endif
