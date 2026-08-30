/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/deployment_controller.h
 *
 * PURPOSE:
 *   Supervise deployment-plan execution, approval, progress, cancellation,
 *   failure evidence and explicit rollback without invoking a provider itself.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_DEPLOYMENT_CONTROLLER_H
#define UMICOM_BUILD_DEPLOYMENT_CONTROLLER_H

#include "umicom/build/deployment_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_DEPLOYMENT_CONTROLLER_API_VERSION 1U

typedef enum UmiBuildDeploymentRunState {
    UMI_BUILD_DEPLOYMENT_IDLE = 0,
    UMI_BUILD_DEPLOYMENT_PLANNED = 1,
    UMI_BUILD_DEPLOYMENT_RUNNING = 2,
    UMI_BUILD_DEPLOYMENT_SUCCEEDED = 3,
    UMI_BUILD_DEPLOYMENT_FAILED = 4,
    UMI_BUILD_DEPLOYMENT_CANCELLED = 5,
    UMI_BUILD_DEPLOYMENT_ROLLING_BACK = 6,
    UMI_BUILD_DEPLOYMENT_ROLLED_BACK = 7
} UmiBuildDeploymentRunState;

typedef struct UmiBuildDeploymentControllerSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiBuildDeploymentRunState state;
    UmiBuildDeploymentPlanSnapshot plan;
    char current_step_id[UMI_BUILD_ID_CAPACITY];
    char failed_step_id[UMI_BUILD_ID_CAPACITY];
    char message[UMI_BUILD_NAME_CAPACITY];
    size_t completed_step_count;
    size_t failed_step_count;
    size_t cancelled_step_count;
    size_t skipped_step_count;
    uint32_t progress_basis_points;
    UmiStatus last_status;
    int last_exit_code;
    uint64_t revision;
    int stop_requested;
} UmiBuildDeploymentControllerSnapshot;

typedef struct UmiBuildDeploymentController
    UmiBuildDeploymentController;

UmiStatus umi_build_deployment_controller_create(
    UmiBuildDeploymentController **out_controller);
void umi_build_deployment_controller_destroy(
    UmiBuildDeploymentController *controller);
UmiStatus umi_build_deployment_controller_load(
    UmiBuildDeploymentController *controller,
    const UmiBuildDeploymentPlan *plan);
UmiStatus umi_build_deployment_controller_begin(
    UmiBuildDeploymentController *controller);
UmiStatus umi_build_deployment_controller_next_step(
    const UmiBuildDeploymentController *controller,
    UmiBuildDeploymentStepSnapshot *out_step);
UmiStatus umi_build_deployment_controller_start_step(
    UmiBuildDeploymentController *controller,
    const char *step_id);
UmiStatus umi_build_deployment_controller_complete_step(
    UmiBuildDeploymentController *controller,
    const char *step_id,
    int exit_code);
UmiStatus umi_build_deployment_controller_fail_step(
    UmiBuildDeploymentController *controller,
    const char *step_id,
    UmiStatus status,
    int exit_code,
    const char *message);
UmiStatus umi_build_deployment_controller_request_stop(
    UmiBuildDeploymentController *controller);
UmiStatus umi_build_deployment_controller_begin_rollback(
    UmiBuildDeploymentController *controller);
UmiStatus umi_build_deployment_controller_complete_rollback(
    UmiBuildDeploymentController *controller,
    int exit_code);
UmiStatus umi_build_deployment_controller_snapshot(
    const UmiBuildDeploymentController *controller,
    UmiBuildDeploymentControllerSnapshot *out_snapshot);
UmiStatus umi_build_deployment_controller_plan(
    const UmiBuildDeploymentController *controller,
    UmiBuildDeploymentPlan *out_plan);
const char *umi_build_deployment_run_state_text(
    UmiBuildDeploymentRunState state);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_DEPLOYMENT_CONTROLLER_H */
