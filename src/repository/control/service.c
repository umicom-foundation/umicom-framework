/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/service.c
 *
 * PURPOSE:
 *   Compose parsing, head resolution input, planning, health and audit services.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/service.h"

#include <string.h>

void umi_repository_control_service_init(UmiRepositoryControlService *service)
{
    if (service == NULL) return;
    (void)memset(service, 0, sizeof(*service));
    umi_repository_controller_init(&service->controller);
    umi_repository_inventory_init(&service->inventory);
    umi_repository_head_set_init(&service->heads);
    umi_repository_issue_list_init(&service->issues);
    umi_repository_audit_init(&service->audit);
    service->revision = 1U;
}

UmiStatus umi_repository_control_service_load_gitmodules(
    UmiRepositoryControlService *service, const char *text)
{
    UmiStatus status;
    if (service == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_controller_begin(&service->controller, 0);
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_gitmodules_parse(text, &service->inventory);
    if (status != UMI_STATUS_OK) {
        (void)umi_repository_controller_fail(&service->controller, status);
        (void)umi_repository_audit_append(
            &service->audit,
            service->controller.state,
            status,
            "",
            "Unable to parse .gitmodules.");
        return status;
    }
    (void)umi_repository_audit_append(
        &service->audit,
        service->controller.state,
        UMI_STATUS_OK,
        "",
        "Submodule inventory loaded.");
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_control_service_register_head(
    UmiRepositoryControlService *service, const char *path, const char *sha)
{
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_head_set_upsert(&service->heads, path, sha);
    if (status == UMI_STATUS_OK) service->revision += 1U;
    return status;
}

UmiStatus umi_repository_control_service_plan(
    UmiRepositoryControlService *service,
    const UmiRepositoryLockPolicy *policy)
{
    UmiStatus status;
    if (service == NULL || policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->controller.dry_run = policy->dry_run;
    status = umi_repository_lock_plan_build(
        &service->inventory, &service->heads, policy, &service->plan);
    if (status != UMI_STATUS_OK) {
        (void)umi_repository_controller_fail(&service->controller, status);
        (void)umi_repository_issue_add(
            &service->issues,
            UMI_REPOSITORY_CONTROL_ERROR,
            "repository.lock.plan",
            "",
            "Unable to build repository lock plan.");
        return status;
    }
    status = umi_repository_controller_mark_planned(&service->controller);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_repository_audit_append(
        &service->audit,
        service->controller.state,
        UMI_STATUS_OK,
        "",
        policy->dry_run
            ? "Repository lock preview planned."
            : "Repository lock mutation planned.");
    service->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_control_service_snapshot(
    const UmiRepositoryControlService *service,
    UmiRepositoryControlSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_repository_snapshot_from_plan(
        service->controller.state,
        service->controller.last_status,
        &service->plan,
        &service->issues,
        out_snapshot);
}
