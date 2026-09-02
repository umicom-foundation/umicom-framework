/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/workspace_trust.h
 *
 * PURPOSE:
 *   Track explicit workspace trust decisions independently from user-interface
 *   state and build-system configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_WORKSPACE_TRUST_H
#define UMICOM_SECURITY_WORKSPACE_TRUST_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKSPACE_TRUST_MAX 256U
/**
 * List the named workspace trust level values accepted by this public contract.
 */
typedef enum UmiWorkspaceTrustLevel { UMI_WORKSPACE_UNTRUSTED = 0, UMI_WORKSPACE_RESTRICTED = 1, UMI_WORKSPACE_TRUSTED = 2 } UmiWorkspaceTrustLevel;
/**
 * Represent the workspace trust record data shared with callers of this public contract.
 */
typedef struct UmiWorkspaceTrustRecord {
    char path[UMI_PATH_CAPACITY];
    UmiWorkspaceTrustLevel level;
    char decided_by[128];
    uint64_t decided_at_ns;
} UmiWorkspaceTrustRecord;
/**
 * Represent the workspace trust store data shared with callers of this public contract.
 */
typedef struct UmiWorkspaceTrustStore UmiWorkspaceTrustStore;
/**
 * Initialise workspace trust store from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_workspace_trust_store_create(UmiWorkspaceTrustStore **out_store);
/**
 * Release or reset state held by workspace trust store so the same storage can be reused
 * safely.
 */
void umi_workspace_trust_store_destroy(UmiWorkspaceTrustStore *store);
/**
 * Copy workspace trust store into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_workspace_trust_store_set(UmiWorkspaceTrustStore *store,
                                        const char *path,
                                        UmiWorkspaceTrustLevel level,
                                        const char *decided_by,
                                        uint64_t decided_at_ns);
/**
 * Provide the workspace trust store get operation used by this module and its client
 * applications.
 */
UmiStatus umi_workspace_trust_store_get(const UmiWorkspaceTrustStore *store,
                                        const char *path,
                                        UmiWorkspaceTrustRecord *out_record);
/**
 * Return the number of records represented by workspace trust store without changing their
 * state.
 */
size_t umi_workspace_trust_store_count(const UmiWorkspaceTrustStore *store);
#ifdef __cplusplus
}
#endif

#endif
