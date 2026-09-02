/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_continuous_integration.c
 *
 * PURPOSE:
 *   Verify debounce timing, retained scope registration, generation counters,
 *   update publication and safe JSON output for continuous builds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/build/continuous_integration.h"

/* Store copied notices so tests never depend on callback-owned pointers. */
typedef struct UpdateCapture {
    UmiBuildContinuousUpdate updates[8];
    size_t count;
} UpdateCapture;

/* Capture each published notice and reject writes beyond the test bound. */
static UmiStatus capture_update(const UmiBuildContinuousUpdate *update,
                                void *user_data)
{
    UpdateCapture *capture = (UpdateCapture *)user_data;

    if (update == NULL || capture == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (capture->count >= sizeof(capture->updates) /
                              sizeof(capture->updates[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capture->updates[capture->count++] = *update;
    return UMI_STATUS_OK;
}

/* Copy a short fixture string and prove that it was not truncated. */
static void fixture_text(char *destination,
                         size_t capacity,
                         const char *source)
{
    const int written = snprintf(destination, capacity, "%s", source);

    assert(written >= 0 && (size_t)written < capacity);
}

/* Register representative root, Framework and application ownership scopes. */
static void register_scopes(UmiBuildContinuousIntegration *service)
{
    UmiBuildAutomation *planner = umi_build_continuous_planner(service);
    UmiBuildAutomationScope scope;

    assert(planner != NULL);
    umi_build_automation_scope_init(
        &scope, "workspace", "Umicom workspace", "");
    fixture_text(scope.build_target,
                 sizeof(scope.build_target),
                 "umicom-products");
    scope.flags = UMI_BUILD_AUTOMATION_SCOPE_WORKSPACE_WIDE;
    assert(umi_build_automation_register_scope(planner, &scope) ==
           UMI_STATUS_OK);

    umi_build_automation_scope_init(
        &scope, "framework", "Umicom Framework", "framework");
    fixture_text(scope.version, sizeof(scope.version), "0.9.0");
    fixture_text(scope.build_target,
                 sizeof(scope.build_target),
                 "umicom-products");
    scope.flags = UMI_BUILD_AUTOMATION_SCOPE_SHARED_PROVIDER;
    assert(umi_build_automation_register_scope(planner, &scope) ==
           UMI_STATUS_OK);

    umi_build_automation_scope_init(
        &scope, "studio", "Umicom Studio IDE", "applications/studio");
    fixture_text(scope.version, sizeof(scope.version), "0.23.0");
    fixture_text(scope.build_target,
                 sizeof(scope.build_target),
                 "umicom-product-studio");
    scope.flags = UMI_BUILD_AUTOMATION_SCOPE_CONSUMES_SHARED;
    assert(umi_build_automation_register_scope(planner, &scope) ==
           UMI_STATUS_OK);
}

/* Create one service using a deterministic 100 ms quiet period. */
static UmiBuildContinuousIntegration *create_service(UpdateCapture *capture)
{
    UmiBuildContinuousConfig config =
        umi_build_continuous_config_default();
    UmiBuildContinuousIntegration *service = NULL;

    config.debounce_ms = 100U;
    config.initial_generation = 10U;
    config.update_sink = capture_update;
    config.update_user_data = capture;
    config.require_update_record = 1;
    assert(umi_build_continuous_create(&config, &service) == UMI_STATUS_OK);
    register_scopes(service);
    return service;
}

/* Verify that saves are grouped and a successful application build is announced. */
static void test_debounce_and_success(void)
{
    UpdateCapture capture = {0};
    UmiBuildContinuousIntegration *service = create_service(&capture);
    UmiBuildContinuousSnapshot snapshot;
    UmiBuildAutomationPlanItem item;

    assert(umi_build_continuous_record_change(
               service, "applications/studio/src/main.c", 0, 1000U) ==
           UMI_STATUS_OK);
    assert(umi_build_continuous_record_change(
               service, "applications/studio/src/main.c", 0, 1020U) ==
           UMI_STATUS_OK);
    assert(!umi_build_continuous_ready(service, 1119U));
    assert(umi_build_continuous_ready(service, 1120U));
    assert(umi_build_continuous_begin(service, 1120U) == UMI_STATUS_OK);
    assert(umi_build_continuous_plan_count(service) == 1U);
    assert(umi_build_continuous_plan_item_at(service, 0U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.scope_id, "studio") == 0);
    assert(strcmp(item.version, "0.23.0") == 0);
    assert(umi_build_continuous_complete(service, 1, 1200U, 5000U) ==
           UMI_STATUS_OK);
    assert(capture.count == 1U);
    assert(strcmp(capture.updates[0].scope_id, "studio") == 0);
    assert(capture.updates[0].generation == 11U);
    assert(capture.updates[0].available_at_ms == 5000U);
    assert(capture.updates[0].restart_required != 0);
    assert(umi_build_continuous_snapshot(service, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.phase == UMI_BUILD_CONTINUOUS_SUCCEEDED);
    assert(snapshot.successful_generations == 1U);
    assert(snapshot.batch_change_count == 1U);
    assert(snapshot.queued_event_count == 0U);

    /* Scope registration survives completion, so the next save needs no scan. */
    assert(umi_build_continuous_record_change(
               service, "applications/studio/src/editor.c", 0, 1300U) ==
           UMI_STATUS_OK);
    assert(umi_build_continuous_begin(service, 1400U) == UMI_STATUS_OK);
    assert(umi_build_continuous_plan_count(service) == 1U);
    assert(umi_build_continuous_complete(service, 0, 1450U, 5250U) ==
           UMI_STATUS_INTERNAL_ERROR);
    assert(capture.count == 1U);
    assert(umi_build_continuous_snapshot(service, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.phase == UMI_BUILD_CONTINUOUS_FAILED);
    assert(snapshot.failed_generations == 1U);

    /* Failed evidence remains owned by the service. The next edit retries the
     * failed file together with the new change instead of losing either one. */
    assert(umi_build_continuous_record_change(
               service, "applications/studio/src/panel.c", 0, 1500U) ==
           UMI_STATUS_OK);
    assert(umi_build_continuous_begin(service, 1600U) == UMI_STATUS_OK);
    assert(umi_build_continuous_plan_item_at(service, 0U, &item) ==
           UMI_STATUS_OK);
    assert(item.changed_file_count == 2U);
    assert(umi_build_continuous_complete(service, 1, 1650U, 5450U) ==
           UMI_STATUS_OK);
    assert(capture.count == 2U);
    assert(capture.updates[1].generation == 13U);
    umi_build_continuous_destroy(service);
}

/* Verify bounded JSON escaping for identifiers exposed to application clients. */
static void test_update_json(void)
{
    UmiBuildContinuousUpdate update;
    char json[1024];

    (void)memset(&update, 0, sizeof(update));
    update.structure_size = (uint32_t)sizeof(update);
    update.api_version = UMI_BUILD_CONTINUOUS_API_VERSION;
    fixture_text(update.scope_id, sizeof(update.scope_id), "studio");
    fixture_text(update.display_name,
                 sizeof(update.display_name),
                 "Studio \"Local\"");
    fixture_text(update.version, sizeof(update.version), "0.23.0");
    fixture_text(update.build_target,
                 sizeof(update.build_target),
                 "umicom-product-studio");
    update.generation = 7U;
    update.available_at_ms = 9000U;
    update.restart_required = 1;
    assert(umi_build_continuous_update_format_json(
               &update, json, sizeof(json)) == UMI_STATUS_OK);
    assert(strstr(json, "Studio \\\"Local\\\"") != NULL);
    assert(strstr(json, "\"generation\":7") != NULL);
    assert(strstr(json, "\"restart_required\":true") != NULL);
}

/* Execute independent state-machine and transport-format scenarios. */
int main(void)
{
    test_debounce_and_success();
    test_update_json();
    assert(strcmp(umi_build_continuous_phase_text(
                      UMI_BUILD_CONTINUOUS_DEBOUNCING),
                  "debouncing") == 0);
    return 0;
}
