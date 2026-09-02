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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/controller.h"

#include <string.h>

/*
 * Initialise repository controller from caller-provided values so later operations receive
 * a known state.
 */
void umi_repository_controller_init(UmiRepositoryController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return;
    (void)memset(controller, 0, sizeof(*controller));
    controller->state = UMI_REPOSITORY_CONTROL_IDLE;
    controller->last_status = UMI_STATUS_OK;
    controller->revision = 1U;
}

/*
 * Provide the repository controller begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_controller_begin(
    UmiRepositoryController *controller, int dry_run)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state == UMI_REPOSITORY_CONTROL_STAGING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_INSPECTING;
    controller->last_status = UMI_STATUS_OK;
    controller->dry_run = dry_run != 0;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository controller mark planned operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_controller_mark_planned(
    UmiRepositoryController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state != UMI_REPOSITORY_CONTROL_INSPECTING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_PLANNED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository controller mark staging operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_controller_mark_staging(
    UmiRepositoryController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->dry_run ||
        controller->state != UMI_REPOSITORY_CONTROL_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_STAGING;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository controller mark verified operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_controller_mark_verified(
    UmiRepositoryController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->state != UMI_REPOSITORY_CONTROL_PLANNED &&
        controller->state != UMI_REPOSITORY_CONTROL_STAGING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->state = UMI_REPOSITORY_CONTROL_VERIFIED;
    controller->last_status = UMI_STATUS_OK;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository controller fail operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_controller_fail(
    UmiRepositoryController *controller, UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || status == UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    controller->state = UMI_REPOSITORY_CONTROL_FAILED;
    controller->last_status = status;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository controller can stage operation used by this module and its client
 * applications.
 */
int umi_repository_controller_can_stage(
    const UmiRepositoryController *controller)
{
    return controller != NULL &&
        !controller->dry_run &&
        controller->state == UMI_REPOSITORY_CONTROL_PLANNED;
}
