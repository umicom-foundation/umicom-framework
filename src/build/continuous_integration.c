/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/continuous_integration.c
 *
 * PURPOSE:
 *   Implement debounced continuous-integration generations and safe module
 *   update publication on top of the affected-scope build planner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/continuous_integration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The service owns its planner while retaining only copied configuration. */
struct UmiBuildContinuousIntegration {
    UmiBuildContinuousConfig config;
    UmiBuildAutomation *planner;
    UmiBuildContinuousSnapshot snapshot;
};

/* Copy bounded text so malformed metadata cannot corrupt an update notice. */
static UmiStatus continuous_copy_text(char *destination,
                                      size_t capacity,
                                      const char *source)
{
    const size_t length = source != NULL ? strlen(source) : 0U;

    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/* Initialise the public snapshot prefix after allocation or batch reset. */
static void continuous_snapshot_init(UmiBuildContinuousSnapshot *snapshot)
{
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->structure_size = (uint32_t)sizeof(*snapshot);
    snapshot->api_version = UMI_BUILD_CONTINUOUS_API_VERSION;
    snapshot->phase = UMI_BUILD_CONTINUOUS_IDLE;
    snapshot->last_status = UMI_STATUS_OK;
    snapshot->revision = 1U;
}

/* Build one copied update from a successful affected-scope plan item. */
static UmiStatus continuous_update_from_item(
    const UmiBuildAutomationPlanItem *item,
    uint64_t generation,
    uint64_t available_at_ms,
    UmiBuildContinuousUpdate *out_update)
{
    UmiStatus status;

    if (item == NULL || out_update == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_update, 0, sizeof(*out_update));
    out_update->structure_size = (uint32_t)sizeof(*out_update);
    out_update->api_version = UMI_BUILD_CONTINUOUS_API_VERSION;
    status = continuous_copy_text(out_update->scope_id,
                                  sizeof(out_update->scope_id),
                                  item->scope_id);
    if (status == UMI_STATUS_OK) {
        status = continuous_copy_text(out_update->display_name,
                                      sizeof(out_update->display_name),
                                      item->display_name);
    }
    if (status == UMI_STATUS_OK) {
        status = continuous_copy_text(out_update->version,
                                      sizeof(out_update->version),
                                      item->version);
    }
    if (status == UMI_STATUS_OK) {
        status = continuous_copy_text(out_update->build_target,
                                      sizeof(out_update->build_target),
                                      item->build_target);
    }
    out_update->generation = generation;
    out_update->available_at_ms = available_at_ms;
    out_update->restart_required = 1;
    return status;
}

/* Append one character to JSON while always preserving a final NUL byte. */
static UmiStatus continuous_json_character(char *out_json,
                                           size_t capacity,
                                           size_t *used,
                                           char value)
{
    if (out_json == NULL || used == NULL || *used + 1U >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_json[(*used)++] = value;
    out_json[*used] = '\0';
    return UMI_STATUS_OK;
}

/* Append JSON string content with the control-character escapes needed here. */
static UmiStatus continuous_json_text(char *out_json,
                                      size_t capacity,
                                      size_t *used,
                                      const char *text)
{
    size_t index;

    if (text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; text[index] != '\0'; ++index) {
        const unsigned char value = (unsigned char)text[index];

        if (value == '"' || value == '\\') {
            if (continuous_json_character(
                    out_json, capacity, used, '\\') != UMI_STATUS_OK ||
                continuous_json_character(
                    out_json, capacity, used, (char)value) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        } else if (value == '\n' || value == '\r' || value == '\t') {
            const char escaped = value == '\n'
                ? 'n'
                : (value == '\r' ? 'r' : 't');

            if (continuous_json_character(
                    out_json, capacity, used, '\\') != UMI_STATUS_OK ||
                continuous_json_character(
                    out_json, capacity, used, escaped) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        } else if (value < 0x20U) {
            return UMI_STATUS_INVALID_ARGUMENT;
        } else if (continuous_json_character(
                       out_json,
                       capacity,
                       used,
                       (char)value) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    return UMI_STATUS_OK;
}

/* Return defaults that group the short burst of writes made by one save. */
UmiBuildContinuousConfig umi_build_continuous_config_default(void)
{
    UmiBuildContinuousConfig config;

    (void)memset(&config, 0, sizeof(config));
    config.structure_size = (uint32_t)sizeof(config);
    config.api_version = UMI_BUILD_CONTINUOUS_API_VERSION;
    config.debounce_ms = 750U;
    return config;
}

/* Allocate the lifecycle state and its owned affected-scope planner. */
UmiStatus umi_build_continuous_create(
    const UmiBuildContinuousConfig *config,
    UmiBuildContinuousIntegration **out_service)
{
    UmiBuildContinuousConfig effective;
    UmiBuildContinuousIntegration *service;
    UmiStatus status;

    if (out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    effective = config != NULL
        ? *config
        : umi_build_continuous_config_default();
    if ((config != NULL &&
         (config->structure_size < sizeof(*config) ||
          config->api_version != UMI_BUILD_CONTINUOUS_API_VERSION)) ||
        (effective.require_update_record && effective.update_sink == NULL) ||
        effective.initial_generation == UINT64_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (effective.debounce_ms == 0U) {
        effective.debounce_ms = 750U;
    }
    service = (UmiBuildContinuousIntegration *)calloc(1U, sizeof(*service));
    if (service == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    service->config = effective;
    continuous_snapshot_init(&service->snapshot);
    service->snapshot.generation = effective.initial_generation;
    status = umi_build_automation_create(&service->planner);
    if (status != UMI_STATUS_OK) {
        free(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

/* Release only storage owned by the continuous service. */
void umi_build_continuous_destroy(UmiBuildContinuousIntegration *service)
{
    if (service == NULL) {
        return;
    }
    umi_build_automation_destroy(service->planner);
    free(service);
}

/* Expose borrowed planning state for initial scope registration and reading. */
UmiBuildAutomation *umi_build_continuous_planner(
    UmiBuildContinuousIntegration *service)
{
    return service != NULL ? service->planner : NULL;
}

/* Queue one unique changed path and restart the quiet-period timer. */
UmiStatus umi_build_continuous_record_change(
    UmiBuildContinuousIntegration *service,
    const char *workspace_relative_path,
    int deleted,
    uint64_t observed_at_ms)
{
    UmiBuildAutomationChange change;
    UmiStatus status;

    if (service == NULL || workspace_relative_path == NULL ||
        workspace_relative_path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (service->snapshot.phase == UMI_BUILD_CONTINUOUS_RUNNING) {
        return UMI_STATUS_BUSY;
    }
    umi_build_automation_change_init(
        &change, workspace_relative_path, deleted);
    status = umi_build_automation_add_change(service->planner, &change);
    if (status != UMI_STATUS_OK) {
        service->snapshot.last_status = status;
        service->snapshot.revision += 1U;
        return status;
    }
    if (service->snapshot.queued_event_count < SIZE_MAX) {
        service->snapshot.queued_event_count += 1U;
    }
    service->snapshot.phase = UMI_BUILD_CONTINUOUS_DEBOUNCING;
    service->snapshot.last_change_at_ms = observed_at_ms;
    service->snapshot.last_status = UMI_STATUS_OK;
    service->snapshot.revision += 1U;
    return UMI_STATUS_OK;
}

/* Report readiness only after the configured quiet period has fully elapsed. */
int umi_build_continuous_ready(
    const UmiBuildContinuousIntegration *service,
    uint64_t now_ms)
{
    if (service == NULL ||
        service->snapshot.phase != UMI_BUILD_CONTINUOUS_DEBOUNCING ||
        now_ms < service->snapshot.last_change_at_ms) {
        return 0;
    }
    return now_ms - service->snapshot.last_change_at_ms >=
        (uint64_t)service->config.debounce_ms;
}

/* Freeze the current evidence as one deterministic build generation. */
UmiStatus umi_build_continuous_begin(
    UmiBuildContinuousIntegration *service,
    uint64_t started_at_ms)
{
    UmiBuildAutomationSnapshot plan_snapshot;
    UmiStatus status;

    if (service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_build_continuous_ready(service, started_at_ms)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* A generation must never wrap because applications use it to decide
     * whether an update is newer than the module they already observed. */
    if (service->snapshot.generation == UINT64_MAX) {
        service->snapshot.phase = UMI_BUILD_CONTINUOUS_FAILED;
        service->snapshot.last_status = UMI_STATUS_CAPACITY_EXCEEDED;
        service->snapshot.revision += 1U;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_build_automation_evaluate(service->planner);
    if (status == UMI_STATUS_OK) {
        status = umi_build_automation_snapshot(
            service->planner, &plan_snapshot);
    }
    if (status != UMI_STATUS_OK) {
        service->snapshot.phase = UMI_BUILD_CONTINUOUS_FAILED;
        service->snapshot.last_status = status;
        service->snapshot.revision += 1U;
        return status;
    }
    service->snapshot.phase = UMI_BUILD_CONTINUOUS_RUNNING;
    service->snapshot.batch_change_count = plan_snapshot.change_count;
    service->snapshot.batch_scope_count = plan_snapshot.selected_count;
    service->snapshot.generation += 1U;
    service->snapshot.last_started_at_ms = started_at_ms;
    service->snapshot.last_status = UMI_STATUS_OK;
    service->snapshot.revision += 1U;
    return UMI_STATUS_OK;
}

/* Return active plan size only while its evaluated generation is retained. */
size_t umi_build_continuous_plan_count(
    const UmiBuildContinuousIntegration *service)
{
    return service != NULL &&
           service->snapshot.phase == UMI_BUILD_CONTINUOUS_RUNNING
        ? umi_build_automation_count(service->planner)
        : 0U;
}

/* Copy an item from the active generation through the planner's safe API. */
UmiStatus umi_build_continuous_plan_item_at(
    const UmiBuildContinuousIntegration *service,
    size_t position,
    UmiBuildAutomationPlanItem *out_item)
{
    if (service == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (service->snapshot.phase != UMI_BUILD_CONTINUOUS_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_build_automation_item_at(
        service->planner, position, out_item);
}

/* Publish successful module generations, then retain counters but clear paths. */
UmiStatus umi_build_continuous_complete(
    UmiBuildContinuousIntegration *service,
    int build_and_tests_succeeded,
    uint64_t completed_at_ms,
    uint64_t available_at_ms)
{
    UmiStatus publication_status = UMI_STATUS_OK;
    size_t index;

    if (service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (service->snapshot.phase != UMI_BUILD_CONTINUOUS_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (build_and_tests_succeeded) {
        const size_t count = umi_build_automation_count(service->planner);

        for (index = 0U; index < count; ++index) {
            UmiBuildAutomationPlanItem item;
            UmiBuildContinuousUpdate update;
            UmiStatus status = umi_build_automation_item_at(
                service->planner, index, &item);

            if (status != UMI_STATUS_OK) {
                publication_status = status;
                break;
            }
            if ((item.actions & UMI_BUILD_AUTOMATION_ACTION_BUILD) == 0U ||
                item.version[0] == '\0') {
                continue;
            }
            status = continuous_update_from_item(
                &item,
                service->snapshot.generation,
                available_at_ms,
                &update);
            if (status == UMI_STATUS_OK &&
                service->config.update_sink != NULL) {
                status = service->config.update_sink(
                    &update, service->config.update_user_data);
            }
            if (status == UMI_STATUS_OK &&
                service->config.update_sink != NULL) {
                service->snapshot.published_updates += 1U;
            } else if (service->config.require_update_record) {
                publication_status = status;
                break;
            }
        }
    }

    service->snapshot.last_completed_at_ms = completed_at_ms;
    service->snapshot.queued_event_count = 0U;
    if (build_and_tests_succeeded && publication_status == UMI_STATUS_OK) {
        service->snapshot.phase = UMI_BUILD_CONTINUOUS_SUCCEEDED;
        service->snapshot.successful_generations += 1U;
        service->snapshot.last_status = UMI_STATUS_OK;
    } else {
        service->snapshot.phase = UMI_BUILD_CONTINUOUS_FAILED;
        service->snapshot.failed_generations += 1U;
        service->snapshot.last_status = publication_status != UMI_STATUS_OK
            ? publication_status
            : UMI_STATUS_INTERNAL_ERROR;
    }
    service->snapshot.revision += 1U;
    /* Successful evidence is complete and can be forgotten. Failed evidence
     * stays in the planner so the next edit retries the whole failed slice. */
    if (service->snapshot.phase == UMI_BUILD_CONTINUOUS_SUCCEEDED) {
        umi_build_automation_clear_changes(service->planner);
    }
    return service->snapshot.last_status;
}

/* Copy state for a UI or command without exposing mutable service memory. */
UmiStatus umi_build_continuous_snapshot(
    const UmiBuildContinuousIntegration *service,
    UmiBuildContinuousSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = service->snapshot;
    return UMI_STATUS_OK;
}

/* Encode controlled update metadata for durable publish/subscribe transport. */
UmiStatus umi_build_continuous_update_format_json(
    const UmiBuildContinuousUpdate *update,
    char *out_json,
    size_t capacity)
{
    static const char PREFIX[] = "{\"scope_id\":\"";
    static const char DISPLAY[] = "\",\"display_name\":\"";
    static const char VERSION[] = "\",\"version\":\"";
    static const char TARGET[] = "\",\"build_target\":\"";
    char suffix[160];
    size_t used = 0U;
    int written;
    UmiStatus status;

    if (update == NULL || out_json == NULL || capacity == 0U ||
        update->structure_size < sizeof(*update) ||
        update->api_version != UMI_BUILD_CONTINUOUS_API_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_json[0] = '\0';
#define UMI_CONTINUOUS_APPEND_LITERAL(value) \
    do { \
        const size_t literal_length = sizeof(value) - 1U; \
        if (used + literal_length >= capacity) { \
            out_json[0] = '\0'; \
            return UMI_STATUS_CAPACITY_EXCEEDED; \
        } \
        (void)memcpy(out_json + used, value, literal_length); \
        used += literal_length; \
        out_json[used] = '\0'; \
    } while (0)

    UMI_CONTINUOUS_APPEND_LITERAL(PREFIX);
    status = continuous_json_text(
        out_json, capacity, &used, update->scope_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UMI_CONTINUOUS_APPEND_LITERAL(DISPLAY);
    status = continuous_json_text(
        out_json, capacity, &used, update->display_name);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UMI_CONTINUOUS_APPEND_LITERAL(VERSION);
    status = continuous_json_text(
        out_json, capacity, &used, update->version);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UMI_CONTINUOUS_APPEND_LITERAL(TARGET);
    status = continuous_json_text(
        out_json, capacity, &used, update->build_target);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    written = snprintf(
        suffix,
        sizeof(suffix),
        "\",\"generation\":%llu,\"available_at_ms\":%llu,"
        "\"restart_required\":%s}",
        (unsigned long long)update->generation,
        (unsigned long long)update->available_at_ms,
        update->restart_required ? "true" : "false");
    if (written < 0 || (size_t)written >= sizeof(suffix) ||
        used + (size_t)written >= capacity) {
        out_json[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_json + used, suffix, (size_t)written + 1U);
#undef UMI_CONTINUOUS_APPEND_LITERAL
    return UMI_STATUS_OK;
}

/* Translate lifecycle values into stable status text. */
const char *umi_build_continuous_phase_text(UmiBuildContinuousPhase phase)
{
    switch (phase) {
        case UMI_BUILD_CONTINUOUS_IDLE: return "idle";
        case UMI_BUILD_CONTINUOUS_DEBOUNCING: return "debouncing";
        case UMI_BUILD_CONTINUOUS_RUNNING: return "running";
        case UMI_BUILD_CONTINUOUS_SUCCEEDED: return "succeeded";
        case UMI_BUILD_CONTINUOUS_FAILED: return "failed";
        default: return "unknown";
    }
}
