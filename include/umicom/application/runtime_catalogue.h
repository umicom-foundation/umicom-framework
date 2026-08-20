/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime_catalogue.h
 *
 * PURPOSE:
 *   Maintain the authoritative runtime and installation state projected into
 *   Umicom Desk. The catalogue combines stable application identity with
 *   mutable presence, compatibility, pinning, process and failure evidence.
 *
 * ARCHITECTURE:
 *   The Master Controller owns mutation of this catalogue. Frontend adapters,
 *   launchers and application modules consume copied snapshots and must not
 *   retain pointers to internal records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_RUNTIME_CATALOGUE_H
#define UMICOM_APPLICATION_RUNTIME_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/application/definition.h"
#include "umicom/application/presentation.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_RUNTIME_MAX_APPLICATIONS 64U
#define UMI_APPLICATION_RUNTIME_ID_CAPACITY 160U
#define UMI_APPLICATION_RUNTIME_NAME_CAPACITY 192U
#define UMI_APPLICATION_RUNTIME_PATH_CAPACITY 2048U
#define UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY 512U

typedef enum UmiApplicationRuntimeState {
    UMI_APPLICATION_RUNTIME_UNKNOWN = 0,
    UMI_APPLICATION_RUNTIME_UNAVAILABLE = 1,
    UMI_APPLICATION_RUNTIME_STOPPED = 2,
    UMI_APPLICATION_RUNTIME_STARTING = 3,
    UMI_APPLICATION_RUNTIME_RUNNING = 4,
    UMI_APPLICATION_RUNTIME_ATTENTION = 5,
    UMI_APPLICATION_RUNTIME_FAILED = 6,
    UMI_APPLICATION_RUNTIME_STOPPING = 7
} UmiApplicationRuntimeState;

typedef struct UmiApplicationRuntimeRegistration {
    uint32_t structure_size;
    const char *application_id;
    const char *display_name;
    const char *executable_name;
    const char *working_directory;
    const char *icon_resource_id;
    const char *default_layout_id;
    const char *taskbar_group;
    UmiApplicationFamily family;
    UmiApplicationMaturity maturity;
    UmiApplicationEntryKind entry_kind;
    bool installed;
    bool compatible;
    bool enabled;
    bool pinned;
    bool visible_when_unavailable;
} UmiApplicationRuntimeRegistration;

typedef struct UmiApplicationRuntimeRecord {
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char display_name[UMI_APPLICATION_RUNTIME_NAME_CAPACITY];
    char executable_name[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
    char working_directory[UMI_APPLICATION_RUNTIME_PATH_CAPACITY];
    char icon_resource_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char default_layout_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char taskbar_group[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char last_error[UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY];
    UmiApplicationFamily family;
    UmiApplicationMaturity maturity;
    UmiApplicationEntryKind entry_kind;
    UmiApplicationRuntimeState state;
    bool installed;
    bool compatible;
    bool enabled;
    bool visible;
    bool pinned;
    bool running;
    bool active;
    bool attention;
    bool visible_when_unavailable;
    uint64_t process_token;
    uint64_t generation;
    uint64_t revision;
    int last_exit_code;
} UmiApplicationRuntimeRecord;

typedef struct UmiApplicationRuntimeSnapshot {
    char active_application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    size_t application_count;
    size_t visible_application_count;
    size_t pinned_application_count;
    size_t running_application_count;
    size_t failed_application_count;
    uint64_t revision;
} UmiApplicationRuntimeSnapshot;

typedef struct UmiApplicationRuntimeCatalogue UmiApplicationRuntimeCatalogue;

UmiStatus umi_application_runtime_catalogue_create(
    UmiApplicationRuntimeCatalogue **out_catalogue);
void umi_application_runtime_catalogue_destroy(
    UmiApplicationRuntimeCatalogue *catalogue);

UmiStatus umi_application_runtime_registration_validate(
    const UmiApplicationRuntimeRegistration *registration);

UmiStatus umi_application_runtime_catalogue_register(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationRuntimeRegistration *registration);
UmiStatus umi_application_runtime_catalogue_upsert(
    UmiApplicationRuntimeCatalogue *catalogue,
    const UmiApplicationRuntimeRegistration *registration);

/*
 * Seed immutable identity and presentation metadata from the canonical
 * Framework portfolio. Availability is conservative: only applications marked
 * UMI_APPLICATION_AVAILABLE begin as installed. A composition root can then
 * upsert the exact executables it built or installed.
 */
UmiStatus umi_application_runtime_catalogue_seed_portfolio(
    UmiApplicationRuntimeCatalogue *catalogue);

UmiStatus umi_application_runtime_catalogue_set_presence(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    bool installed,
    bool compatible,
    bool enabled);
UmiStatus umi_application_runtime_catalogue_set_state(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    UmiApplicationRuntimeState state,
    const char *message);
UmiStatus umi_application_runtime_catalogue_set_process(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    uint64_t process_token);
UmiStatus umi_application_runtime_catalogue_pin(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    bool pinned);
UmiStatus umi_application_runtime_catalogue_activate(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id);
UmiStatus umi_application_runtime_catalogue_mark_exit(
    UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    int exit_code,
    const char *message);

UmiStatus umi_application_runtime_catalogue_find(
    const UmiApplicationRuntimeCatalogue *catalogue,
    const char *application_id,
    UmiApplicationRuntimeRecord *out_record);
UmiStatus umi_application_runtime_catalogue_at(
    const UmiApplicationRuntimeCatalogue *catalogue,
    size_t index,
    UmiApplicationRuntimeRecord *out_record);
size_t umi_application_runtime_catalogue_count(
    const UmiApplicationRuntimeCatalogue *catalogue);
UmiStatus umi_application_runtime_catalogue_snapshot(
    const UmiApplicationRuntimeCatalogue *catalogue,
    UmiApplicationRuntimeSnapshot *out_snapshot);

const char *umi_application_runtime_state_text(
    UmiApplicationRuntimeState state);

#ifdef __cplusplus
}
#endif

#endif
