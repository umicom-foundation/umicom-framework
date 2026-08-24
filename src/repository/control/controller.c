/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/controller.c
 *
 * PURPOSE:
 *   Coordinate repository operations under the Framework controller policy.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/controller.h"

#include <string.h>

void umi_repository_controller_init(UmiRepositoryController *controller)
{
    if (controller == NULL) return;
    (void)memset(controller, 0, sizeof(*controller));
    controller->state = UMI_REPOSITORY_CONTROL_IDLE;
    controller->last_status = UMI_STATUS_OK;
    controller->revision = 1U;
}

UmiStatus umi_repository_controller_begin(
    UmiRepositoryController *controller, int dry_run)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state == UMI_REPOSITORY_CONTROL_STAGING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_INSPECTING;
    controller->last_status = UMI_STATUS_OK;
    controller->dry_run = dry_run != 0;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_controller_mark_planned(
    UmiRepositoryController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_REPOSITORY_CONTROL_INSPECTING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_PLANNED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_controller_mark_staging(
    UmiRepositoryController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->dry_run ||
        controller->state != UMI_REPOSITORY_CONTROL_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_STAGING;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_controller_mark_verified(
    UmiRepositoryController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_REPOSITORY_CONTROL_PLANNED &&
        controller->state != UMI_REPOSITORY_CONTROL_STAGING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_VERIFIED;
    controller->last_status = UMI_STATUS_OK;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_controller_fail(
    UmiRepositoryController *controller, UmiStatus status)
{
    if (controller == NULL || status == UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    controller->state = UMI_REPOSITORY_CONTROL_FAILED;
    controller->last_status = status;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

int umi_repository_controller_can_stage(
    const UmiRepositoryController *controller)
{
    return controller != NULL &&
        !controller->dry_run &&
        controller->state == UMI_REPOSITORY_CONTROL_PLANNED;
}
