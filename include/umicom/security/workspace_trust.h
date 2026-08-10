/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/workspace_trust.h
 *
 * PURPOSE:
 *   Track explicit workspace trust decisions independently from user-interface
 *   state and build-system configuration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiWorkspaceTrustLevel { UMI_WORKSPACE_UNTRUSTED = 0, UMI_WORKSPACE_RESTRICTED = 1, UMI_WORKSPACE_TRUSTED = 2 } UmiWorkspaceTrustLevel;
typedef struct UmiWorkspaceTrustRecord {
    char path[UMI_PATH_CAPACITY];
    UmiWorkspaceTrustLevel level;
    char decided_by[128];
    uint64_t decided_at_ns;
} UmiWorkspaceTrustRecord;
typedef struct UmiWorkspaceTrustStore UmiWorkspaceTrustStore;
UmiStatus umi_workspace_trust_store_create(UmiWorkspaceTrustStore **out_store);
void umi_workspace_trust_store_destroy(UmiWorkspaceTrustStore *store);
UmiStatus umi_workspace_trust_store_set(UmiWorkspaceTrustStore *store,
                                        const char *path,
                                        UmiWorkspaceTrustLevel level,
                                        const char *decided_by,
                                        uint64_t decided_at_ns);
UmiStatus umi_workspace_trust_store_get(const UmiWorkspaceTrustStore *store,
                                        const char *path,
                                        UmiWorkspaceTrustRecord *out_record);
size_t umi_workspace_trust_store_count(const UmiWorkspaceTrustStore *store);
#ifdef __cplusplus
}
#endif

#endif
