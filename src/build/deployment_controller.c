/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/deployment_controller.c
 *
 * PURPOSE:
 *   Implement deployment lifecycle supervision and rollback state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/deployment_controller.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildDeploymentController {
    UmiBuildDeploymentPlan plan;
    UmiBuildDeploymentRunState state;
    size_t current_step;
    size_t failed_step;
    UmiStatus last_status;
    int last_exit_code;
    char message[UMI_BUILD_NAME_CAPACITY];
    uint64_t revision;
    int has_plan;
    int stop_requested;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static size_t find_step(const UmiBuildDeploymentController *controller,
                        const char *step_id)
{
    size_t index;
    if (controller == NULL || step_id == NULL) return SIZE_MAX;
    for (index = 0U; index < controller->plan.step_count; ++index) {
        if (strcmp(controller->plan.steps[index].step_id, step_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int normal_steps_complete(const UmiBuildDeploymentController *controller)
{
    size_t index;
    for (index = 0U; index < controller->plan.step_count; ++index) {
        const UmiBuildDeploymentStepSnapshot *step = &controller->plan.steps[index];
        if (step->rollback_step) continue;
        if (step->state != UMI_BUILD_DEPLOYMENT_STEP_SUCCEEDED &&
            step->state != UMI_BUILD_DEPLOYMENT_STEP_SKIPPED) {
            return 0;
        }
    }
    return 1;
}

UmiStatus umi_build_deployment_controller_create(
    UmiBuildDeploymentController **out_controller)
{
    UmiBuildDeploymentController *controller;
    if (out_controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_controller = NULL;
    controller = (UmiBuildDeploymentController *)calloc(1U, sizeof(*controller));
    if (controller == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    controller->state = UMI_BUILD_DEPLOYMENT_IDLE;
    controller->current_step = SIZE_MAX;
    controller->failed_step = SIZE_MAX;
    controller->last_status = UMI_STATUS_OK;
    controller->revision = 1U;
    *out_controller = controller;
    return UMI_STATUS_OK;
}

void umi_build_deployment_controller_destroy(
    UmiBuildDeploymentController *controller)
{
    free(controller);
}

UmiStatus umi_build_deployment_controller_load(
    UmiBuildDeploymentController *controller,
    const UmiBuildDeploymentPlan *plan)
{
    char message[256];
    size_t index;
    UmiStatus status;
    if (controller == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state == UMI_BUILD_DEPLOYMENT_RUNNING ||
        controller->state == UMI_BUILD_DEPLOYMENT_ROLLING_BACK) {
        return UMI_STATUS_BUSY;
    }
    status = umi_build_deployment_plan_validate(plan, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    controller->plan = *plan;
    for (index = 0U; index < controller->plan.step_count; ++index) {
        controller->plan.steps[index].state =
            UMI_BUILD_DEPLOYMENT_STEP_PENDING;
        controller->plan.steps[index].revision += 1U;
    }
    controller->state = UMI_BUILD_DEPLOYMENT_PLANNED;
    controller->current_step = SIZE_MAX;
    controller->failed_step = SIZE_MAX;
    controller->last_status = UMI_STATUS_OK;
    controller->last_exit_code = 0;
    controller->message[0] = '\0';
    controller->has_plan = 1;
    controller->stop_requested = 0;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_begin(
    UmiBuildDeploymentController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!controller->has_plan ||
        controller->state != UMI_BUILD_DEPLOYMENT_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (controller->plan.requires_approval && !controller->plan.approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    controller->state = UMI_BUILD_DEPLOYMENT_RUNNING;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_next_step(
    const UmiBuildDeploymentController *controller,
    UmiBuildDeploymentStepSnapshot *out_step)
{
    size_t index;
    int rollback;
    if (controller == NULL || out_step == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (controller->state != UMI_BUILD_DEPLOYMENT_RUNNING &&
        controller->state != UMI_BUILD_DEPLOYMENT_ROLLING_BACK) {
        return UMI_STATUS_INVALID_STATE;
    }
    rollback = controller->state == UMI_BUILD_DEPLOYMENT_ROLLING_BACK;
    for (index = 0U; index < controller->plan.step_count; ++index) {
        const UmiBuildDeploymentStepSnapshot *step = &controller->plan.steps[index];
        if ((step->rollback_step != 0) != rollback) continue;
        if (step->state == UMI_BUILD_DEPLOYMENT_STEP_PENDING) {
            *out_step = *step;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

static int prior_required_steps_complete(
    const UmiBuildDeploymentController *controller,
    size_t step_index)
{
    size_t index;
    const int rollback = controller->plan.steps[step_index].rollback_step;
    for (index = 0U; index < step_index; ++index) {
        const UmiBuildDeploymentStepSnapshot *step = &controller->plan.steps[index];
        if (step->rollback_step != rollback || !step->required) continue;
        if (step->state != UMI_BUILD_DEPLOYMENT_STEP_SUCCEEDED &&
            step->state != UMI_BUILD_DEPLOYMENT_STEP_SKIPPED) {
            return 0;
        }
    }
    return 1;
}

UmiStatus umi_build_deployment_controller_start_step(
    UmiBuildDeploymentController *controller,
    const char *step_id)
{
    size_t index;
    if (controller == NULL || step_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (controller->state != UMI_BUILD_DEPLOYMENT_RUNNING &&
        controller->state != UMI_BUILD_DEPLOYMENT_ROLLING_BACK) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (controller->current_step != SIZE_MAX) return UMI_STATUS_BUSY;
    index = find_step(controller, step_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (controller->plan.steps[index].state !=
            UMI_BUILD_DEPLOYMENT_STEP_PENDING ||
        !prior_required_steps_complete(controller, index)) {
        return UMI_STATUS_INVALID_STATE;
    }
    if ((controller->plan.steps[index].rollback_step != 0) !=
        (controller->state == UMI_BUILD_DEPLOYMENT_ROLLING_BACK)) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->plan.steps[index].state = UMI_BUILD_DEPLOYMENT_STEP_RUNNING;
    controller->plan.steps[index].revision += 1U;
    controller->current_step = index;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_complete_step(
    UmiBuildDeploymentController *controller,
    const char *step_id,
    int exit_code)
{
    size_t index;
    if (controller == NULL || step_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_step(controller, step_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (controller->current_step != index ||
        controller->plan.steps[index].state !=
            UMI_BUILD_DEPLOYMENT_STEP_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (exit_code != 0) {
        return umi_build_deployment_controller_fail_step(
            controller, step_id, UMI_STATUS_INTERNAL_ERROR,
            exit_code, "Deployment step returned a non-zero exit code.");
    }
    controller->plan.steps[index].state =
        UMI_BUILD_DEPLOYMENT_STEP_SUCCEEDED;
    controller->plan.steps[index].revision += 1U;
    controller->current_step = SIZE_MAX;
    controller->last_status = UMI_STATUS_OK;
    controller->last_exit_code = 0;
    controller->revision += 1U;
    if (controller->state == UMI_BUILD_DEPLOYMENT_RUNNING &&
        controller->stop_requested) {
        controller->state = UMI_BUILD_DEPLOYMENT_CANCELLED;
    } else if (controller->state == UMI_BUILD_DEPLOYMENT_RUNNING &&
               normal_steps_complete(controller)) {
        controller->state = UMI_BUILD_DEPLOYMENT_SUCCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_fail_step(
    UmiBuildDeploymentController *controller,
    const char *step_id,
    UmiStatus status,
    int exit_code,
    const char *message)
{
    size_t index;
    if (controller == NULL || step_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_step(controller, step_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (controller->current_step != index ||
        controller->plan.steps[index].state !=
            UMI_BUILD_DEPLOYMENT_STEP_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) status = UMI_STATUS_INTERNAL_ERROR;
    controller->plan.steps[index].state = UMI_BUILD_DEPLOYMENT_STEP_FAILED;
    controller->plan.steps[index].revision += 1U;
    controller->current_step = SIZE_MAX;
    controller->failed_step = index;
    controller->last_status = status;
    controller->last_exit_code = exit_code;
    copy_text(controller->message, sizeof(controller->message), message);
    controller->state = UMI_BUILD_DEPLOYMENT_FAILED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_request_stop(
    UmiBuildDeploymentController *controller)
{
    size_t index;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_BUILD_DEPLOYMENT_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    controller->stop_requested = 1;
    for (index = 0U; index < controller->plan.step_count; ++index) {
        UmiBuildDeploymentStepSnapshot *step = &controller->plan.steps[index];
        if (!step->rollback_step &&
            step->state == UMI_BUILD_DEPLOYMENT_STEP_PENDING) {
            step->state = UMI_BUILD_DEPLOYMENT_STEP_CANCELLED;
            step->revision += 1U;
        }
    }
    if (controller->current_step == SIZE_MAX) {
        controller->state = UMI_BUILD_DEPLOYMENT_CANCELLED;
    }
    controller->last_status = UMI_STATUS_CANCELLED;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_begin_rollback(
    UmiBuildDeploymentController *controller)
{
    size_t index;
    int found = 0;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!controller->has_plan || !controller->plan.rollback_enabled ||
        (controller->state != UMI_BUILD_DEPLOYMENT_FAILED &&
         controller->state != UMI_BUILD_DEPLOYMENT_CANCELLED)) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < controller->plan.step_count; ++index) {
        if (controller->plan.steps[index].rollback_step) {
            controller->plan.steps[index].state =
                UMI_BUILD_DEPLOYMENT_STEP_PENDING;
            controller->plan.steps[index].revision += 1U;
            found = 1;
        }
    }
    if (!found) return UMI_STATUS_NOT_FOUND;
    controller->state = UMI_BUILD_DEPLOYMENT_ROLLING_BACK;
    controller->current_step = SIZE_MAX;
    controller->stop_requested = 0;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_complete_rollback(
    UmiBuildDeploymentController *controller,
    int exit_code)
{
    size_t index;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->state != UMI_BUILD_DEPLOYMENT_ROLLING_BACK) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (controller->current_step != SIZE_MAX) return UMI_STATUS_BUSY;
    if (exit_code != 0) {
        controller->state = UMI_BUILD_DEPLOYMENT_FAILED;
        controller->last_status = UMI_STATUS_INTERNAL_ERROR;
        controller->last_exit_code = exit_code;
        controller->revision += 1U;
        return UMI_STATUS_INTERNAL_ERROR;
    }
    for (index = 0U; index < controller->plan.step_count; ++index) {
        if (controller->plan.steps[index].rollback_step &&
            controller->plan.steps[index].state ==
                UMI_BUILD_DEPLOYMENT_STEP_PENDING) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
    controller->state = UMI_BUILD_DEPLOYMENT_ROLLED_BACK;
    controller->last_status = UMI_STATUS_OK;
    controller->last_exit_code = 0;
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_snapshot(
    const UmiBuildDeploymentController *controller,
    UmiBuildDeploymentControllerSnapshot *out_snapshot)
{
    size_t index;
    size_t terminal = 0U;
    if (controller == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_DEPLOYMENT_CONTROLLER_API_VERSION;
    out_snapshot->state = controller->state;
    out_snapshot->last_status = controller->last_status;
    out_snapshot->last_exit_code = controller->last_exit_code;
    out_snapshot->revision = controller->revision;
    out_snapshot->stop_requested = controller->stop_requested;
    copy_text(out_snapshot->message, sizeof(out_snapshot->message),
              controller->message);
    if (controller->has_plan) {
        (void)umi_build_deployment_plan_snapshot(
            &controller->plan, &out_snapshot->plan);
    }
    if (controller->current_step != SIZE_MAX) {
        copy_text(out_snapshot->current_step_id,
                  sizeof(out_snapshot->current_step_id),
                  controller->plan.steps[controller->current_step].step_id);
    }
    if (controller->failed_step != SIZE_MAX) {
        copy_text(out_snapshot->failed_step_id,
                  sizeof(out_snapshot->failed_step_id),
                  controller->plan.steps[controller->failed_step].step_id);
    }
    for (index = 0U; index < controller->plan.step_count; ++index) {
        const UmiBuildDeploymentStepState state =
            controller->plan.steps[index].state;
        if (state == UMI_BUILD_DEPLOYMENT_STEP_SUCCEEDED) {
            out_snapshot->completed_step_count += 1U;
            terminal += 1U;
        } else if (state == UMI_BUILD_DEPLOYMENT_STEP_FAILED) {
            out_snapshot->failed_step_count += 1U;
            terminal += 1U;
        } else if (state == UMI_BUILD_DEPLOYMENT_STEP_CANCELLED) {
            out_snapshot->cancelled_step_count += 1U;
            terminal += 1U;
        } else if (state == UMI_BUILD_DEPLOYMENT_STEP_SKIPPED) {
            out_snapshot->skipped_step_count += 1U;
            terminal += 1U;
        }
    }
    if (controller->plan.step_count > 0U) {
        out_snapshot->progress_basis_points =
            (uint32_t)((terminal * 10000U) / controller->plan.step_count);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_controller_plan(
    const UmiBuildDeploymentController *controller,
    UmiBuildDeploymentPlan *out_plan)
{
    if (controller == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!controller->has_plan) return UMI_STATUS_NOT_FOUND;
    *out_plan = controller->plan;
    return UMI_STATUS_OK;
}

const char *umi_build_deployment_run_state_text(
    UmiBuildDeploymentRunState state)
{
    switch (state) {
    case UMI_BUILD_DEPLOYMENT_IDLE: return "Idle";
    case UMI_BUILD_DEPLOYMENT_PLANNED: return "Planned";
    case UMI_BUILD_DEPLOYMENT_RUNNING: return "Running";
    case UMI_BUILD_DEPLOYMENT_SUCCEEDED: return "Succeeded";
    case UMI_BUILD_DEPLOYMENT_FAILED: return "Failed";
    case UMI_BUILD_DEPLOYMENT_CANCELLED: return "Cancelled";
    case UMI_BUILD_DEPLOYMENT_ROLLING_BACK: return "Rolling back";
    case UMI_BUILD_DEPLOYMENT_ROLLED_BACK: return "Rolled back";
    default: return "Unknown";
    }
}
