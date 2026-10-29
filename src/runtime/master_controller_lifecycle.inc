/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/master_controller_lifecycle.inc
 *
 * PURPOSE:
 *   Retain module cleanup obligations and dependencies across lifecycle failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The outer lifecycle owns the controller exclusively and has set lifecycle_busy.
 * Stop at the first failure: an earlier provider can still be used by the
 * dependent whose quiesce or stop callback has not completed. */
static UmiStatus MasterStopPendingModules(UmiMasterController *controller)
{
    for (size_t index = controller->start_order_count; index > 0U; --index) {
        UmiModuleRuntimeEntry *entry =
            &controller->modules[controller->start_order[index - 1U]];
        UmiStatus status = UMI_STATUS_OK;
        if (!entry->needs_stop) continue;
        umi_master_health(controller, entry->descriptor->module_id,
            UMI_HEALTH_STOPPING, "Stopping module");
        if (entry->quiesce_required && !entry->quiesce_complete) {
            if (entry->descriptor->lifecycle.quiesce != NULL)
                status = entry->descriptor->lifecycle.quiesce(&entry->context);
            if (status != UMI_STATUS_OK) {
                entry->state = UMI_MODULE_FAILED;
                umi_master_health(controller, entry->descriptor->module_id,
                    UMI_HEALTH_FAILED, "Module quiesce failed; dependencies retained");
                return status;
            }
            entry->quiesce_complete = 1;
            entry->state = UMI_MODULE_QUIESCED;
        }
        if (entry->descriptor->lifecycle.stop != NULL)
            status = entry->descriptor->lifecycle.stop(&entry->context);
        if (status != UMI_STATUS_OK) {
            entry->state = UMI_MODULE_FAILED;
            umi_master_health(controller, entry->descriptor->module_id,
                UMI_HEALTH_FAILED, "Module stop failed; dependencies retained");
            return status;
        }
        entry->needs_stop = 0;
        entry->state = UMI_MODULE_STOPPED;
        umi_master_health(controller, entry->descriptor->module_id,
            UMI_HEALTH_STOPPED, "Module stopped");
    }
    controller->cleanup_pending = 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_master_controller_start(UmiMasterController *controller)
{
    UmiStatus status;
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->started || controller->cleanup_pending || controller->release_pending ||
        controller->lifecycle_busy || UmiServiceRegistryIsInCallback(controller->services))
        return UMI_STATUS_INVALID_STATE;
    status = umi_master_controller_validate(controller, NULL);
    controller->lifecycle_busy = 1;
    if (status != UMI_STATUS_OK) {
        umi_master_health(controller, "org.umicom.framework.master",
            UMI_HEALTH_FAILED, "Module dependency validation failed");
        umi_master_report(controller, UMI_DIAGNOSTIC_ERROR,
            "Module dependency validation failed");
        controller->lifecycle_busy = 0;
        return status;
    }
    for (size_t index = 0U; index < controller->module_count; ++index) {
        controller->modules[index].needs_stop = 0;
        controller->modules[index].quiesce_required = 0;
        controller->modules[index].quiesce_complete = 0;
    }
    umi_master_health(controller, "org.umicom.framework.master",
        UMI_HEALTH_STARTING, "Starting Slave Controllers");
    for (size_t index = 0U; index < controller->start_order_count; ++index) {
        UmiModuleRuntimeEntry *entry =
            &controller->modules[controller->start_order[index]];
        status = umi_master_run_phase(controller, entry,
            entry->descriptor->lifecycle.configure, UMI_MODULE_CONFIGURED,
            UMI_HEALTH_STARTING, "Module configured", "Module configure failed");
        if (status != UMI_STATUS_OK) goto rollback;
        /* Once configuration completes, later failure must still stop this
         * configured entry even though its visible state becomes FAILED. */
        entry->needs_stop = 1;
        controller->cleanup_pending = 1;
        status = umi_master_run_phase(controller, entry,
            entry->descriptor->lifecycle.initialise, UMI_MODULE_INITIALISED,
            UMI_HEALTH_STARTING, "Module initialised", "Module initialise failed");
        if (status != UMI_STATUS_OK) goto rollback;
        status = umi_master_run_phase(controller, entry,
            entry->descriptor->lifecycle.start, UMI_MODULE_STARTED,
            UMI_HEALTH_READY, "Module started", "Module start failed");
        if (status != UMI_STATUS_OK) goto rollback;
        entry->quiesce_required = 1;
    }
    controller->started = 1;
    controller->cleanup_pending = 0;
    umi_master_health(controller, "org.umicom.framework.master",
        UMI_HEALTH_READY, "All Slave Controllers started");
    umi_master_report(controller, UMI_DIAGNOSTIC_INFO,
        "All Slave Controllers started");
    controller->lifecycle_busy = 0;
    return UMI_STATUS_OK;

rollback:
    /* Keep the startup error as the return value. A separate retained cleanup
     * obligation prevents retry/start/release from concealing a rollback error. */
    controller->cleanup_pending = 1;
    UmiStatus cleanup_status = MasterStopPendingModules(controller);
    if (cleanup_status != UMI_STATUS_OK) {
        umi_master_health(controller, "org.umicom.framework.master",
            UMI_HEALTH_DEGRADED, "Startup failed; module cleanup remains pending");
        umi_master_report(controller, UMI_DIAGNOSTIC_ERROR,
            "Startup rollback could not stop a module; retain controller and retry stop");
    }
    controller->lifecycle_busy = 0;
    return status;
}

UmiStatus umi_master_controller_stop(UmiMasterController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (controller->lifecycle_busy || controller->release_pending ||
        UmiServiceRegistryIsInCallback(controller->services) ||
        (!controller->started && !controller->cleanup_pending))
        return UMI_STATUS_INVALID_STATE;
    controller->lifecycle_busy = 1;
    controller->cleanup_pending = 1;
    umi_master_health(controller, "org.umicom.framework.master",
        UMI_HEALTH_STOPPING, "Stopping Slave Controllers");
    UmiStatus status = MasterStopPendingModules(controller);
    if (status == UMI_STATUS_OK) {
        controller->started = 0;
        umi_master_health(controller, "org.umicom.framework.master",
            UMI_HEALTH_STOPPED, "All Slave Controllers stopped");
        umi_master_report(controller, UMI_DIAGNOSTIC_INFO,
            "All Slave Controllers stopped");
    } else {
        umi_master_health(controller, "org.umicom.framework.master",
            UMI_HEALTH_DEGRADED, "Module shutdown remains pending");
    }
    controller->lifecycle_busy = 0;
    return status;
}

UmiStatus UmiMasterControllerRelease(UmiMasterController **inOutController)
{
    size_t index;
    if (inOutController == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiMasterController *controller = *inOutController;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) {
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (controller->lifecycle_busy || UmiServiceRegistryIsInCallback(controller->services))
        return UMI_STATUS_INVALID_STATE;
    if (controller->started || controller->cleanup_pending) {
        UmiStatus status = umi_master_controller_stop(controller);
        if (status != UMI_STATUS_OK) return status;
    }
    controller->lifecycle_busy = 1;
    controller->release_pending = 1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = controller->module_count; index > 0U; --index) {
        /* Use dependency order when the complete validated order exists.
         * Never-started/incompletely validated compositions retain reverse
         * registration as their existing destruction fallback. */
        size_t slot = controller->start_order_count == controller->module_count
            ? controller->start_order[index - 1U] : index - 1U;
        UmiModuleRuntimeEntry *entry = &controller->modules[slot];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->state != UMI_MODULE_DESTROYED &&
            entry->descriptor != NULL &&
            entry->descriptor->lifecycle.destroy != NULL) {
            entry->descriptor->lifecycle.destroy(&entry->context);
        }
        entry->state = UMI_MODULE_DESTROYED;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry->descriptor != NULL) {
            umi_master_health(controller,
                              entry->descriptor->module_id,
                              UMI_HEALTH_STOPPED,
                              "Module destroyed");
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = controller->authority_count; index > 0U; --index) {
        UmiMasterAuthorityEntry *entry = &controller->authorities[index - 1U];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry->destroy != NULL) {
            entry->destroy(entry->authority);
        }
        entry->authority = NULL;
        entry->destroy = NULL;
    }

    /* Retain the former unchecked child release for comparison. A failed
     * checked release keeps the remaining controller graph and forbids restart. */
#if 0
    umi_service_registry_destroy(controller->services);
#endif
    UmiStatus release_status = UmiServiceRegistryRelease(&controller->services);
    if (release_status != UMI_STATUS_OK) {
        controller->lifecycle_busy = 0;
        return release_status;
    }
    umi_command_registry_destroy(controller->command_registry);
    umi_health_registry_destroy(controller->health);
    umi_policy_engine_destroy(controller->policy);
    umi_capability_registry_destroy(controller->capabilities);
    umi_module_registry_destroy(controller->registry);
    umi_scheduler_destroy(controller->scheduler);
    umi_clock_dispose(&controller->clock);
    umi_config_destroy(controller->config);
    umi_data_server_destroy(controller->data_server);
    umi_query_bus_destroy(controller->queries);
    umi_command_bus_destroy(controller->commands);
    umi_event_bus_destroy(controller->events);
    free(controller);
    *inOutController = NULL;
    return UMI_STATUS_OK;
}

void umi_master_controller_destroy(UmiMasterController *controller)
{
    /* Compatibility owners retain the graph on a failed checked close. */
    (void)UmiMasterControllerRelease(&controller);
}

int UmiMasterControllerNeedsStop(const UmiMasterController *controller)
{
    return controller != NULL && (controller->started || controller->cleanup_pending);
}
