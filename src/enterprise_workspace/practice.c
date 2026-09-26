/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/practice.c
 * PURPOSE: Reuse canonical policy and role services for the local lesson.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/enterprise_workspace/practice.h"
#include <stdlib.h>
struct UmiEnterprisePracticeAccess {
    UmiPolicyEngine *policy;
    UmiRoleRegistry *roles;
    UmiAuthorisationService *authorisation;
};
static const UmiEnterpriseActor actors[] = {
    {"viewer", "viewer"}, {"author", "editor"}, {"reviewer", "reviewer"},
    {"operator", "operator"}, {"administrator", "administrator"}
};
UmiEnterpriseActor UmiEnterprisePracticeActor(size_t index)
{
    return index < UMI_ENTERPRISE_PRACTICE_ACTORS ? actors[index] : (UmiEnterpriseActor){NULL, NULL};
}
const char *UmiEnterprisePracticeActorLabel(size_t index)
{
    static const char *const labels[] = {"Viewer (practice)", "Author (practice)",
        "Reviewer (practice)", "Operator (practice)", "Administrator (practice)"};
    return index < UMI_ENTERPRISE_PRACTICE_ACTORS ? labels[index] : NULL;
}
UmiStatus UmiEnterprisePracticeAccessCreate(UmiEnterprisePracticeAccess **outAccess)
{
    UmiEnterprisePracticeAccess *access;
    UmiStatus status;
    static const struct { const char *role, *permission; } grants[] = {
        {"editor", "enterprise.dataset.create"}, {"editor", "enterprise.job.prepare"},
        {"editor", "enterprise.job.cancel"}, {"reviewer", "enterprise.job.review"},
        {"operator", "enterprise.job.execute"}, {"administrator", "enterprise.*"}
    };
    if (outAccess == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outAccess = NULL;
    access = calloc(1U, sizeof(*access));
    if (access == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_policy_engine_create(&access->policy);
    if (status == UMI_STATUS_OK) status = umi_role_registry_create(&access->roles);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UMI_ENTERPRISE_PRACTICE_ACTORS; ++i) {
        status = umi_role_registry_define(access->roles, actors[i].role, "Local practice role, not a login");
        if (status == UMI_STATUS_OK) status = umi_role_registry_grant(access->roles, actors[i].role, "enterprise.read");
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(grants)/sizeof(grants[0]); ++i)
        status = umi_role_registry_grant(access->roles, grants[i].role, grants[i].permission);
    if (status == UMI_STATUS_OK) status = umi_authorisation_service_create(access->policy, access->roles, &access->authorisation);
    if (status != UMI_STATUS_OK) { UmiEnterprisePracticeAccessDestroy(access); return status; }
    *outAccess = access; return UMI_STATUS_OK;
}
void UmiEnterprisePracticeAccessDestroy(UmiEnterprisePracticeAccess *access)
{
    if (access == NULL) return;
    umi_authorisation_service_destroy(access->authorisation);
    umi_role_registry_destroy(access->roles);
    umi_policy_engine_destroy(access->policy);
    free(access);
}
UmiAuthorisationService *UmiEnterprisePracticeAuthorisation(UmiEnterprisePracticeAccess *access)
{
    return access != NULL ? access->authorisation : NULL;
}
