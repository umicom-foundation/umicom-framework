/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/identity.h
 *
 * PURPOSE:
 *   Define stable identities and a bounded registry for human users, services,
 *   plug-ins, workers and agents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * List the named identity kind values accepted by this public contract.
 */
typedef enum UmiIdentityKind {
    UMI_IDENTITY_HUMAN = 1,
    UMI_IDENTITY_SERVICE = 2,
    UMI_IDENTITY_PLUGIN = 3,
    UMI_IDENTITY_WORKER = 4,
    UMI_IDENTITY_AGENT = 5
} UmiIdentityKind;
/**
 * Represent the identity record data shared with callers of this public contract.
 */
typedef struct UmiIdentityRecord {
    char identity_id[UMI_IDENTITY_ID_CAPACITY];
    char display_name[UMI_IDENTITY_DISPLAY_CAPACITY];
    UmiIdentityKind kind;
    int enabled;
    uint64_t revision;
} UmiIdentityRecord;
/**
 * Represent the identity registry data shared with callers of this public contract.
 */
typedef struct UmiIdentityRegistry UmiIdentityRegistry;
/**
 * Initialise identity registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_identity_registry_create(UmiIdentityRegistry **out_registry);
/**
 * Release or reset state held by identity registry so the same storage can be reused
 * safely.
 */
void umi_identity_registry_destroy(UmiIdentityRegistry *registry);
/**
 * Add identity registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_identity_registry_register(UmiIdentityRegistry *registry,
                                         const UmiIdentityRecord *identity);
/**
 * Provide the identity registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_identity_registry_get(const UmiIdentityRegistry *registry,
                                    const char *identity_id,
                                    UmiIdentityRecord *out_identity);
/**
 * Provide the identity registry set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_identity_registry_set_enabled(UmiIdentityRegistry *registry,
                                            const char *identity_id,
                                            int enabled);
/**
 * Return the number of records represented by identity registry without changing their
 * state.
 */
size_t umi_identity_registry_count(const UmiIdentityRegistry *registry);
/**
 * Find identity registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_identity_registry_at(const UmiIdentityRegistry *registry,
                                   size_t index,
                                   UmiIdentityRecord *out_identity);
#ifdef __cplusplus
}
#endif

#endif
