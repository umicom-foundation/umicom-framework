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

typedef struct UmiWebWorkbenchAuthProfile {
    char profile_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    UmiWebWorkbenchAuthKind kind;
    char field_name[UMI_WEB_HEADER_NAME_CAPACITY];
    char public_identity[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    char secret_reference[UMI_WEB_WORKBENCH_ID_CAPACITY];
    bool enabled;
} UmiWebWorkbenchAuthProfile;

typedef struct UmiWebWorkbenchAuthCatalogue {
    UmiWebWorkbenchAuthProfile profiles[UMI_WEB_WORKBENCH_MAX_AUTH_PROFILES];
    size_t profile_count;
    uint64_t revision;
} UmiWebWorkbenchAuthCatalogue;

void umi_web_workbench_auth_profile_init(
    UmiWebWorkbenchAuthProfile *profile,
    const char *profile_id,
    const char *name,
    UmiWebWorkbenchAuthKind kind,
    const char *secret_reference);
UmiStatus umi_web_workbench_auth_profile_validate(
    const UmiWebWorkbenchAuthProfile *profile);
void umi_web_workbench_auth_catalogue_init(
    UmiWebWorkbenchAuthCatalogue *catalogue);
UmiStatus umi_web_workbench_auth_catalogue_upsert(
    UmiWebWorkbenchAuthCatalogue *catalogue,
    const UmiWebWorkbenchAuthProfile *profile);
const UmiWebWorkbenchAuthProfile *umi_web_workbench_auth_catalogue_find(
    const UmiWebWorkbenchAuthCatalogue *catalogue,
    const char *profile_id);
UmiStatus umi_web_workbench_auth_apply(
    const UmiWebWorkbenchAuthProfile *profile,
    const char *transient_secret,
    UmiWebWorkbenchRequest *request);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_AUTH_PROFILE_H */
