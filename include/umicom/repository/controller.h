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
typedef struct UmiRepositoryController {
    UmiRepositoryControlState state;
    UmiStatus last_status;
    uint64_t revision;
    int dry_run;
} UmiRepositoryController;
void umi_repository_controller_init(UmiRepositoryController *controller);
UmiStatus umi_repository_controller_begin(
    UmiRepositoryController *controller, int dry_run);
UmiStatus umi_repository_controller_mark_planned(
    UmiRepositoryController *controller);
UmiStatus umi_repository_controller_mark_staging(
    UmiRepositoryController *controller);
UmiStatus umi_repository_controller_mark_verified(
    UmiRepositoryController *controller);
UmiStatus umi_repository_controller_fail(
    UmiRepositoryController *controller, UmiStatus status);
int umi_repository_controller_can_stage(
    const UmiRepositoryController *controller);
#ifdef __cplusplus
}
#endif
#endif
