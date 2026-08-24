/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/service.h
 *
 * PURPOSE:
 *   Compose parsing, head resolution input, planning, health and audit services.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_CONTROL_SERVICE_H
#define UMICOM_REPOSITORY_CONTROL_SERVICE_H
#include "umicom/repository/controller.h"
#include "umicom/repository/gitmodules.h"
#include "umicom/repository/head_set.h"
#include "umicom/repository/issue.h"
#include "umicom/repository/lock_plan.h"
#include "umicom/repository/snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryControlService {
    UmiRepositoryController controller;
    UmiRepositoryInventory inventory;
    UmiRepositoryHeadSet heads;
    UmiRepositoryLockPlan plan;
    UmiRepositoryIssueList issues;
    UmiRepositoryAudit audit;
    uint64_t revision;
} UmiRepositoryControlService;
void umi_repository_control_service_init(UmiRepositoryControlService *service);
UmiStatus umi_repository_control_service_load_gitmodules(
    UmiRepositoryControlService *service, const char *text);
UmiStatus umi_repository_control_service_register_head(
    UmiRepositoryControlService *service, const char *path, const char *sha);
UmiStatus umi_repository_control_service_plan(
    UmiRepositoryControlService *service,
    const UmiRepositoryLockPolicy *policy);
UmiStatus umi_repository_control_service_snapshot(
    const UmiRepositoryControlService *service,
    UmiRepositoryControlSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
