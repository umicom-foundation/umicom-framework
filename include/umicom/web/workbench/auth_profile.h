/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/auth_profile.h
 *
 * PURPOSE:
 *   Describe authentication without retaining credentials. Callers resolve a
 *   secret reference through Umicom Security and pass the transient value only
 *   while preparing an outbound request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_AUTH_PROFILE_H
#define UMICOM_WEB_WORKBENCH_AUTH_PROFILE_H

#include "umicom/web/workbench/request.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench auth profile data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchAuthProfile {
    char profile_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    UmiWebWorkbenchAuthKind kind;
    char field_name[UMI_WEB_HEADER_NAME_CAPACITY];
    char public_identity[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char secret_reference[UMI_WEB_WORKBENCH_ID_CAPACITY];
    bool enabled;
} UmiWebWorkbenchAuthProfile;

/**
 * Represent the web workbench auth catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchAuthCatalogue {
    UmiWebWorkbenchAuthProfile profiles[UMI_WEB_WORKBENCH_MAX_AUTH_PROFILES];
    size_t profile_count;
    uint64_t revision;
} UmiWebWorkbenchAuthCatalogue;

/**
 * Initialise web workbench auth profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_auth_profile_init(
    UmiWebWorkbenchAuthProfile *profile,
    const char *profile_id,
    const char *name,
    UmiWebWorkbenchAuthKind kind,
    const char *secret_reference);
/**
 * Check that web workbench auth profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_web_workbench_auth_profile_validate(
    const UmiWebWorkbenchAuthProfile *profile);
/**
 * Initialise web workbench auth catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_web_workbench_auth_catalogue_init(
    UmiWebWorkbenchAuthCatalogue *catalogue);
/**
 * Provide the web workbench auth catalogue upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_web_workbench_auth_catalogue_upsert(
    UmiWebWorkbenchAuthCatalogue *catalogue,
    const UmiWebWorkbenchAuthProfile *profile);
/**
 * Find web workbench auth catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWebWorkbenchAuthProfile *umi_web_workbench_auth_catalogue_find(
    const UmiWebWorkbenchAuthCatalogue *catalogue,
    const char *profile_id);
/**
 * Perform web workbench auth through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_web_workbench_auth_apply(
    const UmiWebWorkbenchAuthProfile *profile,
    const char *transient_secret,
    UmiWebWorkbenchRequest *request);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_AUTH_PROFILE_H */
