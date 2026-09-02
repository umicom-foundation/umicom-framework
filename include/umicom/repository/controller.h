/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/controller.h
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
#ifndef UMICOM_REPOSITORY_CONTROLLER_H
#define UMICOM_REPOSITORY_CONTROLLER_H
#include "umicom/repository/audit.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository controller data shared with callers of this public contract.
 */
typedef struct UmiRepositoryController {
    UmiRepositoryControlState state;
    UmiStatus last_status;
    uint64_t revision;
    int dry_run;
} UmiRepositoryController;
/**
 * Initialise repository controller from caller-provided values so later operations receive
 * a known state.
 */
void umi_repository_controller_init(UmiRepositoryController *controller);
/**
 * Provide the repository controller begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_controller_begin(
    UmiRepositoryController *controller, int dry_run);
/**
 * Provide the repository controller mark planned operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_controller_mark_planned(
    UmiRepositoryController *controller);
/**
 * Provide the repository controller mark staging operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_controller_mark_staging(
    UmiRepositoryController *controller);
/**
 * Provide the repository controller mark verified operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_controller_mark_verified(
    UmiRepositoryController *controller);
/**
 * Provide the repository controller fail operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_controller_fail(
    UmiRepositoryController *controller, UmiStatus status);
/**
 * Provide the repository controller can stage operation used by this module and its client
 * applications.
 */
int umi_repository_controller_can_stage(
    const UmiRepositoryController *controller);
#ifdef __cplusplus
}
#endif
#endif
