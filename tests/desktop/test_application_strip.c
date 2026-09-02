/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_application_strip.c
 *
 * PURPOSE:
 *   Verify deterministic taskbar ordering, projection and semantic actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/application_strip.h"

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

/*
 * Exercise make registration and return a clear result when the behaviour no longer
 * matches its contract.
 */
static UmiApplicationRuntimeRegistration make_registration(
    const char *id,
    const char *name,
    const char *group,
    bool pinned)
{
    UmiApplicationRuntimeRegistration value = {0};
    value.structure_size = sizeof(value);
    value.application_id = id;
    value.display_name = name;
    value.executable_name = name;
    value.working_directory = "";
    value.icon_resource_id = "umicom.icon.application.generic";
    value.default_layout_id = "mosaic";
    value.taskbar_group = group;
    value.family = UMI_APPLICATION_FAMILY_PLATFORM;
    value.maturity = UMI_APPLICATION_AVAILABLE;
    value.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    value.installed = true;
    value.compatible = true;
    value.enabled = true;
    value.pinned = pinned;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationRuntimeCatalogue *catalogue = NULL;
    UmiDesktopApplicationStrip *strip = NULL;
    UmiApplicationRuntimeRegistration studio =
        make_registration("org.umicom.studio", "Studio",
                          "development", false);
    UmiApplicationRuntimeRegistration desk =
        make_registration("org.umicom.desktop", "Desk",
                          "system", true);
    UmiDesktopApplicationStripItem item;
    UmiDesktopApplicationStripSnapshot snapshot;

    REQUIRE(umi_application_runtime_catalogue_create(&catalogue) ==
            UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &studio) == UMI_STATUS_OK);
    REQUIRE(umi_application_runtime_catalogue_register(
                catalogue, &desk) == UMI_STATUS_OK);
    REQUIRE(umi_desktop_application_strip_create(
                catalogue, &strip) == UMI_STATUS_OK);
    REQUIRE(umi_desktop_application_strip_at(
                strip, 0U, &item) == UMI_STATUS_OK);
    REQUIRE(strcmp(item.application_id, "org.umicom.desktop") == 0);

    REQUIRE(umi_application_runtime_catalogue_set_state(
                catalogue, "org.umicom.studio",
                UMI_APPLICATION_RUNTIME_ATTENTION,
                "Build finished") == UMI_STATUS_OK);
    REQUIRE(umi_desktop_application_strip_refresh(strip) == UMI_STATUS_OK);
    REQUIRE(umi_desktop_application_strip_find(
                strip, "org.umicom.studio", &item) == UMI_STATUS_OK);
    REQUIRE(item.attention);
    REQUIRE(item.running);

    REQUIRE(umi_desktop_application_strip_snapshot(
                strip, &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.item_count == 2U);
    REQUIRE(snapshot.pinned_count == 1U);
    REQUIRE(snapshot.attention_count == 1U);

    umi_desktop_application_strip_destroy(strip);
    umi_application_runtime_catalogue_destroy(catalogue);
    return 0;
}
