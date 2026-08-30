/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_runtime_catalogue.c
 *
 * PURPOSE:
 *   Verify deterministic application registration, presence, activation,
 *   process state and exit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/runtime_catalogue.h"

#include <stdio.h>
#include <string.h>

#define REQUIRE(condition)                                                     \
    do {                                                                       \
        if (!(condition)) {                                                    \
            (void)fprintf(stderr, "[FAIL] %s:%d: %s\n",                       \
                          __FILE__, __LINE__, #condition);                      \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static UmiApplicationRuntimeRegistration registration(
    const char *id,
    const char *name,
    const char *executable,
    bool installed)
{
    UmiApplicationRuntimeRegistration value = {0};
    value.structure_size = sizeof(value);
    value.application_id = id;
    value.display_name = name;
    value.executable_name = executable;
    value.working_directory = "";
    value.icon_resource_id = "umicom.icon.application.generic";
    value.default_layout_id = "mosaic";
    value.taskbar_group = "applications";
    value.family = UMI_APPLICATION_FAMILY_PLATFORM;
    value.maturity = UMI_APPLICATION_AVAILABLE;
    value.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    value.installed = installed;
    value.compatible = installed;
    value.enabled = installed;
    value.pinned = installed;
    return value;
}

int main(void)
{
    UmiApplicationRuntimeCatalogue *catalogue = NULL;
    UmiApplicationRuntimeRegistration desk =
        registration("org.umicom.desktop", "Umicom Desk", "umicom-desk", true);
    UmiApplicationRuntimeRegistration studio =
        registration("org.umicom.studio", "Umicom Studio IDE",
                     "umicom-studio-ide", true);
    UmiApplicationRuntimeRecord record;
    UmiApplicationRuntimeSnapshot snapshot;

    REQUIRE(umi_application_runtime_catalogue_create(&catalogue) ==
            UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &desk) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &studio) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &studio) == UMI_STATUS_ALREADY_EXISTS);
    REQUIRE(umi_application_runtime_catalogue_count(catalogue) == 2U);

    REQUIRE(umi_application_runtime_catalogue_set_state(
                catalogue, "org.umicom.desktop",
                UMI_APPLICATION_RUNTIME_RUNNING, "") == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_set_process(
                catalogue, "org.umicom.desktop", 41U) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_activate(
                catalogue, "org.umicom.desktop") == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_find(
                catalogue, "org.umicom.desktop", &record) == UMI_STATUS_OK);
    REQUIRE(record.running);
    REQUIRE(record.active);
    REQUIRE(record.process_token == 41U);

    REQUIRE(umi_application_runtime_catalogue_mark_exit(
                catalogue, "org.umicom.desktop", 7,
                "simulated failure") == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_find(
                catalogue, "org.umicom.desktop", &record) == UMI_STATUS_OK);
    REQUIRE(record.state == UMI_APPLICATION_RUNTIME_FAILED);
    REQUIRE(!record.running);
    REQUIRE(record.last_exit_code == 7);
    REQUIRE(strcmp(record.last_error, "simulated failure") == 0);

    REQUIRE(umi_application_runtime_catalogue_snapshot(
                catalogue, &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.application_count == 2U);
    REQUIRE(snapshot.failed_application_count == 1U);
    REQUIRE(snapshot.pinned_application_count == 2U);

    umi_application_runtime_catalogue_destroy(catalogue);
    return 0;
}
