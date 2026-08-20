/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime_catalogue.c
 *
 * PURPOSE:
 *   Implement the Master Controller-owned application runtime catalogue used by
 *   Umicom Desk, launchers, installers, diagnostics and future Umicom OS views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/runtime_catalogue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/portfolio.h"

struct UmiApplicationRuntimeCatalogue {
    UmiApplicationRuntimeRecord records[
        UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS];
    size_t count;
    char active_application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    uint64_t revision;
};

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source,
                           bool allow_empty)
{
    size_t length;
    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (source == NULL || source[0] == '\0') {
        if (!allow_empty) return UMI_STATUS_INVALID_ARGUMENT;
        destination[0] = '\0';
        return UMI_STATUS_OK;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static bool state_is_running(UmiApplicationRuntimeState state)
{
    return state == UMI_APPLICATION_RUNTIME_STARTING ||
           state == UMI_APPLICATION_RUNTIME_RUNNING ||
           state == UMI_APPLICATION_RUNTIME_ATTENTION ||
           state == UMI_APPLICATION_RUNTIME_STOPPING;
}

static bool state_is_attention(UmiApplicationRuntimeState state)
{
    return state == UMI_APPLICATION_RUNTIME_ATTENTION ||
           state == UMI_APPLICATION_RUNTIME_FAILED;
}

static UmiApplicationRuntimeRecord *find_mutable(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id)
{
    size_t index;
    if (catalogue == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return NULL;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->records[index].application_id,
                   application_id) == 0) {
            return &catalogue->records[index];
        }
    }
    return NULL;
}

static const UmiApplicationRuntimeRecord *find_const(
    const UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id)
{
    size_t index;
    if (catalogue == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return NULL;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->records[index].application_id,
                   application_id) == 0) {
            return &catalogue->records[index];
        }
    }
    return NULL;
}

static void update_visibility(UmiApplicationRuntimeRecord *record)
{
    if (record == NULL) return;
    record->visible =
        (record->installed && record->compatible && record->enabled) ||
        record->visible_when_unavailable;
    if (!record->installed) {
        record->state = UMI_APPLICATION_RUNTIME_UNAVAILABLE;
        record->running = false;
        record->active = false;
        record->attention = false;
        record->process_token = 0U;
    } else if (!record->compatible || !record->enabled) {
        if (!state_is_running(record->state)) {
            record->state = UMI_APPLICATION_RUNTIME_STOPPED;
        }
        record->active = false;
    } else if (record->state == UMI_APPLICATION_RUNTIME_UNKNOWN ||
               record->state == UMI_APPLICATION_RUNTIME_UNAVAILABLE) {
        record->state = UMI_APPLICATION_RUNTIME_STOPPED;
    }
}

static UmiStatus apply_registration(
    UmiApplicationRuntimeRecord *record,
    const UmiApplicationRuntimeRegistration *registration,
    bool preserve_runtime)
{
    UmiApplicationRuntimeState previous_state =
        UMI_APPLICATION_RUNTIME_UNKNOWN;
    uint64_t previous_process_token = 0U;
    uint64_t previous_generation = 0U;
    uint64_t previous_revision = 0U;
    bool previous_running = false;
    bool previous_active = false;
    bool previous_attention = false;
    int previous_exit_code = 0;
    char previous_error[UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY] = {0};
    UmiStatus status;

    if (record == NULL || registration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (preserve_runtime) {
        previous_state = record->state;
        previous_process_token = record->process_token;
        previous_generation = record->generation;
        previous_revision = record->revision;
        previous_running = record->running;
        previous_active = record->active;
        previous_attention = record->attention;
        previous_exit_code = record->last_exit_code;
        (void)copy_text(previous_error, sizeof(previous_error),
                        record->last_error, true);
    }

    (void)memset(record, 0, sizeof(*record));
    status = copy_text(record->application_id,
                       sizeof(record->application_id),
                       registration->application_id, false);
    if (status == UMI_STATUS_OK) {
        status = copy_text(record->display_name,
                           sizeof(record->display_name),
                           registration->display_name, false);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(record->executable_name,
                           sizeof(record->executable_name),
                           registration->executable_name, true);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(record->working_directory,
                           sizeof(record->working_directory),
                           registration->working_directory, true);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(record->icon_resource_id,
                           sizeof(record->icon_resource_id),
                           registration->icon_resource_id, true);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(record->default_layout_id,
                           sizeof(record->default_layout_id),
                           registration->default_layout_id, true);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(record->taskbar_group,
                           sizeof(record->taskbar_group),
                           registration->taskbar_group, true);
    }
    if (status != UMI_STATUS_OK) return status;

    record->family = registration->family;
    record->maturity = registration->maturity;
    record->entry_kind = registration->entry_kind;
    record->installed = registration->installed;
    record->compatible = registration->compatible;
    record->enabled = registration->enabled;
    record->pinned = registration->pinned;
    record->visible_when_unavailable =
        registration->visible_when_unavailable;
    record->state = preserve_runtime
        ? previous_state
        : (registration->installed
            ? UMI_APPLICATION_RUNTIME_STOPPED
            : UMI_APPLICATION_RUNTIME_UNAVAILABLE);
    record->process_token = previous_process_token;
    record->generation = preserve_runtime ? previous_generation : 1U;
    record->revision = preserve_runtime ? previous_revision + 1U : 1U;
    record->running = preserve_runtime && previous_running;
    record->active = preserve_runtime && previous_active;
    record->attention = preserve_runtime && previous_attention;
    record->last_exit_code = previous_exit_code;
    (void)copy_text(record->last_error, sizeof(record->last_error),
                    previous_error, true);
    update_visibility(record);
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_create(
    UmiApplicationRuntimeCatalogue **out_catalogue)
{
    UmiApplicationRuntimeCatalogue *catalogue;
    if (out_catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    catalogue = (UmiApplicationRuntimeCatalogue *)calloc(
        1U, sizeof(*catalogue));
    if (catalogue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->revision = 1U;
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
}

void umi_application_runtime_catalogue_destroy(
    UmiApplicationRuntimeCatalogue *catalogue)
{
    free(catalogue);
}

UmiStatus umi_application_runtime_registration_validate(
    const UmiApplicationRuntimeRegistration *registration)
{
    if (registration == NULL ||
        registration->structure_size <
            sizeof(UmiApplicationRuntimeRegistration) ||
        registration->application_id == NULL ||
        registration->application_id[0] == '\0' ||
        registration->display_name == NULL ||
        registration->display_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (registration->family < UMI_APPLICATION_FAMILY_PLATFORM ||
        registration->family > UMI_APPLICATION_FAMILY_EDUCATION ||
        registration->maturity < UMI_APPLICATION_AVAILABLE ||
        registration->maturity > UMI_APPLICATION_ROADMAP ||
        registration->entry_kind < UMI_APPLICATION_ENTRY_WORKBENCH ||
        registration->entry_kind > UMI_APPLICATION_ENTRY_SERVICE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (registration->installed &&
        (registration->executable_name == NULL ||
         registration->executable_name[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_register(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationRuntimeRegistration *registration)
{
    UmiStatus status;
    UmiApplicationRuntimeRecord *record;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_runtime_registration_validate(registration);
    if (status != UMI_STATUS_OK) return status;
    if (find_mutable(catalogue, registration->application_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (catalogue->count >= UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    record = &catalogue->records[catalogue->count];
    status = apply_registration(record, registration, false);
    if (status != UMI_STATUS_OK) return status;
    catalogue->count += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_upsert(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationRuntimeRegistration *registration)
{
    UmiApplicationRuntimeRecord *record;
    UmiStatus status;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_runtime_registration_validate(registration);
    if (status != UMI_STATUS_OK) return status;
    record = find_mutable(catalogue, registration->application_id);
    if (record == NULL) {
        return umi_application_runtime_catalogue_register(
            catalogue, registration);
    }
    status = apply_registration(record, registration, true);
    if (status == UMI_STATUS_OK) catalogue->revision += 1U;
    return status;
}

UmiStatus umi_application_runtime_catalogue_seed_portfolio(
    UmiApplicationRuntimeCatalogue *catalogue)
{
    const size_t count = umi_application_portfolio_count();
    size_t index;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    for (index = 0U; index < count; ++index) {
        const UmiApplicationDefinition *definition =
            umi_application_portfolio_at(index);
        const UmiApplicationPresentation *presentation;
        UmiApplicationRuntimeRegistration registration;
        UmiStatus status;
        if (definition == NULL) return UMI_STATUS_INVALID_STATE;
        presentation = umi_application_presentation_find(
            definition->application_id);
        (void)memset(&registration, 0, sizeof(registration));
        registration.structure_size = sizeof(registration);
        registration.application_id = definition->application_id;
        registration.display_name = definition->display_name;
        registration.executable_name = definition->executable_name;
        registration.working_directory = "";
        registration.icon_resource_id = presentation != NULL
            ? presentation->icon_resource_id : "";
        registration.default_layout_id = presentation != NULL
            ? presentation->default_layout_id : "";
        registration.taskbar_group = presentation != NULL
            ? presentation->taskbar_group : "applications";
        registration.family = definition->family;
        registration.maturity = definition->maturity;
        registration.entry_kind = presentation != NULL
            ? presentation->entry_kind
            : UMI_APPLICATION_ENTRY_WORKBENCH;
        registration.installed =
            definition->maturity == UMI_APPLICATION_AVAILABLE;
        registration.compatible = registration.installed;
        registration.enabled = registration.installed;
        registration.pinned = registration.installed &&
            presentation != NULL && presentation->pinned_by_default;
        registration.visible_when_unavailable =
            presentation != NULL &&
            presentation->visible_when_unavailable;
        status = umi_application_runtime_catalogue_upsert(
            catalogue, &registration);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_set_presence(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    bool installed,
    bool compatible,
    bool enabled)
{
    UmiApplicationRuntimeRecord *record =
        find_mutable(catalogue, application_id);
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    record->installed = installed;
    record->compatible = compatible;
    record->enabled = enabled;
    update_visibility(record);
    if (!record->active &&
        strcmp(catalogue->active_application_id,
               record->application_id) == 0) {
        catalogue->active_application_id[0] = '\0';
    }
    record->revision += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_set_state(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    UmiApplicationRuntimeState state,
    const char *message)
{
    UmiApplicationRuntimeRecord *record =
        find_mutable(catalogue, application_id);
    UmiStatus status;
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    if (state < UMI_APPLICATION_RUNTIME_UNKNOWN ||
        state > UMI_APPLICATION_RUNTIME_STOPPING) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!record->installed &&
        state != UMI_APPLICATION_RUNTIME_UNKNOWN &&
        state != UMI_APPLICATION_RUNTIME_UNAVAILABLE) {
        return UMI_STATUS_INVALID_STATE;
    }
    record->state = state;
    record->running = state_is_running(state);
    record->attention = state_is_attention(state);
    if (!record->running) {
        record->active = false;
        if (strcmp(catalogue->active_application_id,
                   record->application_id) == 0) {
            catalogue->active_application_id[0] = '\0';
        }
    }
    status = copy_text(record->last_error, sizeof(record->last_error),
                       message, true);
    if (status != UMI_STATUS_OK) return status;
    record->revision += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_set_process(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    uint64_t process_token)
{
    UmiApplicationRuntimeRecord *record =
        find_mutable(catalogue, application_id);
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    record->process_token = process_token;
    if (process_token != 0U) {
        record->running = true;
        record->state = UMI_APPLICATION_RUNTIME_RUNNING;
        record->generation += 1U;
    }
    record->revision += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_pin(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    bool pinned)
{
    UmiApplicationRuntimeRecord *record =
        find_mutable(catalogue, application_id);
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    record->pinned = pinned;
    record->revision += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_activate(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id)
{
    UmiApplicationRuntimeRecord *record;
    size_t index;
    UmiStatus status;
    if (catalogue == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    record = find_mutable(catalogue, application_id);
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    if (!record->installed || !record->compatible ||
        !record->enabled || !record->visible) {
        return UMI_STATUS_UNAVAILABLE;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        catalogue->records[index].active = false;
    }
    record->active = true;
    record->attention = false;
    status = copy_text(catalogue->active_application_id,
                       sizeof(catalogue->active_application_id),
                       application_id, false);
    if (status != UMI_STATUS_OK) return status;
    record->revision += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_mark_exit(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    int exit_code,
    const char *message)
{
    UmiApplicationRuntimeRecord *record =
        find_mutable(catalogue, application_id);
    UmiStatus status;
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    record->last_exit_code = exit_code;
    record->process_token = 0U;
    record->running = false;
    record->active = false;
    record->attention = exit_code != 0;
    record->state = exit_code == 0
        ? UMI_APPLICATION_RUNTIME_STOPPED
        : UMI_APPLICATION_RUNTIME_FAILED;
    if (strcmp(catalogue->active_application_id,
               record->application_id) == 0) {
        catalogue->active_application_id[0] = '\0';
    }
    status = copy_text(record->last_error, sizeof(record->last_error),
                       message, true);
    if (status != UMI_STATUS_OK) return status;
    record->revision += 1U;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_find(
    const UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    UmiApplicationRuntimeRecord *out_record)
{
    const UmiApplicationRuntimeRecord *record;
    if (out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    record = find_const(catalogue, application_id);
    if (record == NULL) return UMI_STATUS_NOT_FOUND;
    *out_record = *record;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_at(
    const UmiApplicationRuntimeCatalogue *catalogue,
    size_t index,
    UmiApplicationRuntimeRecord *out_record)
{
    if (catalogue == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    *out_record = catalogue->records[index];
    return UMI_STATUS_OK;
}

size_t umi_application_runtime_catalogue_count(
    const UmiApplicationRuntimeCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}

UmiStatus umi_application_runtime_catalogue_snapshot(
    const UmiApplicationRuntimeCatalogue *catalogue,
    UmiApplicationRuntimeSnapshot *out_snapshot)
{
    size_t index;
    if (catalogue == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)copy_text(out_snapshot->active_application_id,
                    sizeof(out_snapshot->active_application_id),
                    catalogue->active_application_id, true);
    out_snapshot->application_count = catalogue->count;
    out_snapshot->revision = catalogue->revision;
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiApplicationRuntimeRecord *record =
            &catalogue->records[index];
        if (record->visible) out_snapshot->visible_application_count += 1U;
        if (record->pinned) out_snapshot->pinned_application_count += 1U;
        if (record->running) out_snapshot->running_application_count += 1U;
        if (record->state == UMI_APPLICATION_RUNTIME_FAILED) {
            out_snapshot->failed_application_count += 1U;
        }
    }
    return UMI_STATUS_OK;
}

const char *umi_application_runtime_state_text(
    UmiApplicationRuntimeState state)
{
    switch (state) {
    case UMI_APPLICATION_RUNTIME_UNKNOWN: return "unknown";
    case UMI_APPLICATION_RUNTIME_UNAVAILABLE: return "unavailable";
    case UMI_APPLICATION_RUNTIME_STOPPED: return "stopped";
    case UMI_APPLICATION_RUNTIME_STARTING: return "starting";
    case UMI_APPLICATION_RUNTIME_RUNNING: return "running";
    case UMI_APPLICATION_RUNTIME_ATTENTION: return "attention";
    case UMI_APPLICATION_RUNTIME_FAILED: return "failed";
    case UMI_APPLICATION_RUNTIME_STOPPING: return "stopping";
    default: return "invalid";
    }
}
