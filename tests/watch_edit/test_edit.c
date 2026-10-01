/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/watch_edit/test_edit.c
 * PURPOSE: Check watch edit ownership, limits and invalidation against canonical state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; Open(&f);
    UmiDebugWatchEdit *edit = NULL; OK(UmiDebugWatchEditCapture(f.workspace, 0U, &edit));
    UmiDebugWatchSnapshot before = Read(&f); UmiDebugWatchChange change;
    UmiDebugWatchSettings settings; OK(UmiDebugWatchSettingsInit(&settings, 1, "savedNotes + 1"));
    if (strcmp(name, "apply") == 0 || strcmp(name, "disable") == 0 || strcmp(name, "reenable") == 0) {
        if (strcmp(name, "apply") != 0) { settings.enabled = 0; strcpy(settings.expression, before.expression); }
        OK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change));
        UmiDebugWatchSnapshot after = Read(&f);
        CHECK(change.changed && !change.removed && !after.valid && after.revision == change.after.revision);
        CHECK(strcmp(after.id, before.id) == 0 && strcmp(after.expression, settings.expression) == 0);
        CHECK(after.value[0] == '\0' && after.type[0] == '\0' && after.session_id[0] == '\0' && after.enabled == settings.enabled);
        CHECK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change) == UMI_STATUS_BUSY && !change.changed);
        if (strcmp(name, "reenable") == 0) {
            UmiDebugWatchEditDestroy(edit); edit = NULL;
            OK(UmiDebugWatchEditCapture(f.workspace, 0U, &edit)); settings.enabled = 1;
            OK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change));
            CHECK(Read(&f).enabled && !Read(&f).valid);
        }
    } else if (strcmp(name, "noop") == 0) {
        OK(UmiDebugWatchSettingsInit(&settings, 1, before.expression));
        uint64_t revision = umi_debug_watch_registry_revision(f.registry);
        OK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change));
        CHECK(!change.changed && Read(&f).valid && strcmp(Read(&f).value, "old") == 0);
        CHECK(umi_debug_watch_registry_revision(f.registry) == revision); OK(UmiDebugWatchEditValidate(f.workspace, edit));
    } else if (strcmp(name, "remove") == 0) {
        OK(UmiDebugWatchEditRemove(f.workspace, edit, &change));
        CHECK(change.changed && change.removed && strcmp(change.before.id, before.id) == 0 && change.after.id[0] == '\0');
        CHECK(umi_debug_watch_registry_count(f.registry) == 0U);
        CHECK(UmiDebugWatchEditRemove(f.workspace, edit, &change) == UMI_STATUS_BUSY);
    } else if (strcmp(name, "ownership") == 0) {
        Close(&f); UmiDebugWatchSnapshot value;
        OK(UmiDebugWatchEditRead(edit, &value)); CHECK(strcmp(value.expression, before.expression) == 0 && strcmp(value.value, "old") == 0);
    } else if (strcmp(name, "bounds") == 0) {
        char text[1025]; memset(text, 'x', sizeof text); text[1024] = '\0'; UmiDebugWatchSettings retained = settings;
        CHECK(UmiDebugWatchSettingsInit(&settings, 1, text) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&settings, &retained, sizeof settings) == 0);
        text[1023] = '\0'; OK(UmiDebugWatchSettingsInit(&settings, 1, text));
        OK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change)); CHECK(strlen(Read(&f).expression) == 1023U);
    } else if (strcmp(name, "invalid") == 0) {
        UmiDebugWatchSettings retained = settings;
        CHECK(UmiDebugWatchSettingsInit(&settings, 1, "") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDebugWatchSettingsInit(&settings, 2, "x") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDebugWatchSettingsInit(&settings, 1, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&settings, &retained, sizeof settings) == 0);
        memset(settings.expression, 'x', sizeof settings.expression);
        CHECK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change) == UMI_STATUS_CAPACITY_EXCEEDED && !change.changed);
        CHECK(Read(&f).revision == before.revision);
        UmiDebugWatchEdit *missing = NULL; CHECK(UmiDebugWatchEditCapture(f.workspace, 1U, &missing) == UMI_STATUS_NOT_FOUND && missing == NULL);
    } else if (strcmp(name, "unrelated") == 0) {
        UmiDebugBreakpointSnapshot bp = {0}; strcpy(bp.id, "breakpoint");
        OK(umi_debug_breakpoint_registry_upsert(umi_debug_service_breakpoint(f.service), &bp));
        UmiDebugVariableSnapshot variable = {0}; strcpy(variable.id, "variable");
        OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(f.service), &variable));
        OK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change));
    } else {
        if (strcmp(name, "changed") == 0) { strcpy(before.value, "new"); OK(umi_debug_watch_registry_upsert(f.registry, &before)); }
        else if (strcmp(name, "reuse") == 0) {
            OK(umi_debug_watch_registry_remove(f.registry, before.id)); OK(umi_debug_watch_registry_upsert(f.registry, &before));
        } else if (strcmp(name, "owner") == 0) {
            umi_debug_workspace_destroy(f.workspace); f.workspace = NULL;
            OK(umi_debug_workspace_create(f.service, f.controller, &f.workspace));
        } else if (strcmp(name, "session") == 0) {
            UmiDebugSessionSnapshot value = {0}; strcpy(value.id, "other");
            OK(umi_debug_session_registry_upsert(umi_debug_service_session(f.service), &value));
        } else if (strcmp(name, "configuration") == 0) {
            UmiDebugLaunchConfigurationSnapshot value = {0}; strcpy(value.id, "other");
            OK(umi_debug_launch_configuration_registry_upsert(umi_debug_service_launch_configuration(f.service), &value));
        } else return 2;
        uint64_t revision = umi_debug_watch_registry_revision(f.registry);
        CHECK(UmiDebugWatchEditApply(f.workspace, edit, &settings, &change) == UMI_STATUS_BUSY && !change.changed);
        CHECK(umi_debug_watch_registry_revision(f.registry) == revision);
    }
    UmiDebugWatchEditDestroy(edit); Close(&f); return 0;
}
