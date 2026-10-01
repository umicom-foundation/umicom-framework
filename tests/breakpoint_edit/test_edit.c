/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/breakpoint_edit/test_edit.c
 * PURPOSE: Exercise actual property changes, ownership and stale row rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; Open(&f);
    UmiDebugBreakpointEdit *edit = NULL; OK(UmiDebugBreakpointEditCapture(f.workspace, 0U, &edit));
    UmiDebugBreakpointSnapshot before = Read(&f); UmiDebugBreakpointChange change;
    UmiDebugBreakpointSettings settings; OK(UmiDebugBreakpointSettingsInit(&settings, 1, "count > 3", "count={count}"));
    if (strcmp(name, "apply") == 0 || strcmp(name, "clear") == 0 || strcmp(name, "disable") == 0) {
        if (strcmp(name, "disable") == 0) settings.enabled = 0;
        OK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change));
        UmiDebugBreakpointSnapshot after = Read(&f);
        CHECK(change.changed && !change.removed && !after.verified && after.revision == change.after.revision);
        CHECK(strcmp(after.id, before.id) == 0 && strcmp(after.uri, before.uri) == 0 && after.line == before.line && after.column == before.column);
        CHECK(strcmp(after.condition, settings.condition) == 0 && strcmp(after.log_message, settings.logMessage) == 0 && after.enabled == settings.enabled);
        CHECK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change) == UMI_STATUS_BUSY && !change.changed);
        if (strcmp(name, "clear") == 0) {
            UmiDebugBreakpointEditDestroy(edit); edit = NULL;
            OK(UmiDebugBreakpointEditCapture(f.workspace, 0U, &edit));
            OK(UmiDebugBreakpointSettingsInit(&settings, 1, "", ""));
            OK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change));
            CHECK(Read(&f).condition[0] == '\0' && Read(&f).log_message[0] == '\0');
        }
    } else if (strcmp(name, "noop") == 0) {
        OK(UmiDebugBreakpointSettingsInit(&settings, 1, "", ""));
        uint64_t generation = umi_debug_breakpoint_registry_revision(f.registry);
        OK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change));
        CHECK(!change.changed && Read(&f).verified && umi_debug_breakpoint_registry_revision(f.registry) == generation);
        OK(UmiDebugBreakpointEditValidate(f.workspace, edit));
    } else if (strcmp(name, "remove") == 0) {
        OK(UmiDebugBreakpointEditRemove(f.workspace, edit, &change));
        CHECK(change.changed && change.removed && strcmp(change.before.id, "first") == 0 && change.after.id[0] == '\0');
        CHECK(umi_debug_breakpoint_registry_count(f.registry) == 0U);
        CHECK(UmiDebugBreakpointEditRemove(f.workspace, edit, &change) == UMI_STATUS_BUSY);
    } else if (strcmp(name, "ownership") == 0) {
        Close(&f); UmiDebugBreakpointSnapshot copied;
        OK(UmiDebugBreakpointEditRead(edit, &copied)); CHECK(strcmp(copied.uri, before.uri) == 0 && copied.line == 13);
    } else if (strcmp(name, "bounds") == 0) {
        char text[513]; memset(text, 'x', sizeof text); text[512] = '\0';
        UmiDebugBreakpointSettings retained = settings;
        CHECK(UmiDebugBreakpointSettingsInit(&settings, 1, text, "") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&settings, &retained, sizeof settings) == 0);
        text[511] = '\0'; OK(UmiDebugBreakpointSettingsInit(&settings, 1, text, text));
        OK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change));
        CHECK(strlen(Read(&f).condition) == 511U);
    } else if (strcmp(name, "invalid") == 0) {
        CHECK(UmiDebugBreakpointSettingsInit(&settings, 2, "", "") == UMI_STATUS_INVALID_ARGUMENT);
        memset(settings.condition, 'x', sizeof settings.condition);
        CHECK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change) == UMI_STATUS_CAPACITY_EXCEEDED && !change.changed);
        CHECK(Read(&f).revision == before.revision && Read(&f).verified);
        UmiDebugBreakpointEdit *missing = NULL; CHECK(UmiDebugBreakpointEditCapture(f.workspace, 1U, &missing) == UMI_STATUS_NOT_FOUND && missing == NULL);
    } else if (strcmp(name, "unrelated") == 0) {
        UmiDebugWatchSnapshot watch = {0}; strcpy(watch.id, "watch");
        OK(umi_debug_watch_registry_upsert(umi_debug_service_watch(f.service), &watch));
        OK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change));
    } else {
        if (strcmp(name, "changed") == 0) { before.line = 99; OK(umi_debug_breakpoint_registry_upsert(f.registry, &before)); }
        else if (strcmp(name, "reuse") == 0) {
            OK(umi_debug_breakpoint_registry_remove(f.registry, before.id)); OK(umi_debug_breakpoint_registry_upsert(f.registry, &before));
        } else if (strcmp(name, "owner") == 0) {
            umi_debug_workspace_destroy(f.workspace); f.workspace = NULL;
            OK(umi_debug_workspace_create(f.service, f.controller, &f.workspace));
        } else if (strcmp(name, "session") == 0) {
            UmiDebugSessionSnapshot session = {0}; strcpy(session.id, "other");
            OK(umi_debug_session_registry_upsert(umi_debug_service_session(f.service), &session));
        } else if (strcmp(name, "configuration") == 0) {
            UmiDebugLaunchConfigurationSnapshot config = {0}; strcpy(config.id, "other");
            OK(umi_debug_launch_configuration_registry_upsert(umi_debug_service_launch_configuration(f.service), &config));
        } else return 2;
        uint64_t generation = umi_debug_breakpoint_registry_revision(f.registry);
        CHECK(UmiDebugBreakpointEditApply(f.workspace, edit, &settings, &change) == UMI_STATUS_BUSY && !change.changed);
        CHECK(umi_debug_breakpoint_registry_revision(f.registry) == generation);
    }
    UmiDebugBreakpointEditDestroy(edit); Close(&f); return 0;
}
