/* Umicom Framework Tests | Desktop session recovery | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/desktop/session_recovery.h"

int main(void)
{
    UmiApplicationContextHub *first_hub = NULL;
    UmiApplicationContextHub *second_hub = NULL;
    UmiDesktopRuntime *first = NULL;
    UmiDesktopRuntime *second = NULL;
    UmiDesktopSessionRecovery *writer = NULL;
    UmiDesktopSessionRecovery *reader = NULL;
    UmiDesktopSessionRecoverySnapshot snapshot;
    const UmiDesktopWindow *window;
    char *journal = NULL;
    char *payload;
    size_t length = 0U;
    assert(umi_application_context_hub_create(&first_hub) == UMI_STATUS_OK);
    assert(umi_application_context_hub_create(&second_hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(first_hub, &first) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(second_hub, &second) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(first) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(second) == UMI_STATUS_OK);
    assert(umi_desktop_session_recovery_create(first, &writer) ==
           UMI_STATUS_OK);
    assert(umi_desktop_session_recovery_begin(
        writer, "studio-desktop", "org.umicom.studio") == UMI_STATUS_OK);
    assert(umi_desktop_runtime_restore_window_session(
        first, "project-explorer", "primary",
        (UmiDesktopRect){555, 90, 480, 760},
        UMI_DESKTOP_DOCK_FLOATING, true, false) == UMI_STATUS_OK);
    assert(umi_desktop_session_recovery_checkpoint(
        writer, "workspace-change", "Workspace geometry changed") ==
        UMI_STATUS_OK);
    journal = (char *)calloc(
        UMI_DESKTOP_RECOVERY_JOURNAL_TEXT_CAPACITY, sizeof(char));
    assert(journal != NULL);
    assert(umi_desktop_session_recovery_export_latest(
        writer, journal, UMI_DESKTOP_RECOVERY_JOURNAL_TEXT_CAPACITY,
        &length) == UMI_STATUS_OK);
    assert(length > 1000U && strstr(
        journal, "UMICOM-DESKTOP-RECOVERY|1") != NULL);
    assert(umi_desktop_session_recovery_create(second, &reader) ==
           UMI_STATUS_OK);
    payload = strstr(journal, "UMICOM-DESKTOP|1\n");
    assert(payload != NULL);
    payload[0] = 'X';
    assert(umi_desktop_session_recovery_import(reader, journal) ==
           UMI_STATUS_PARSE_ERROR);
    payload[0] = 'U';
    assert(umi_desktop_session_recovery_import(reader, journal) ==
           UMI_STATUS_OK);
    assert(umi_desktop_session_recovery_snapshot(reader, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.recovery_available && !snapshot.clean_shutdown);
    assert(umi_desktop_session_recovery_restore(reader, NULL) ==
           UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(second), "project-explorer");
    assert(window != NULL && window->bounds.x == 555);
    assert(umi_desktop_session_recovery_mark_clean(reader) == UMI_STATUS_OK);
    assert(umi_desktop_session_recovery_snapshot(reader, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.clean_shutdown && !snapshot.recovery_available);
    free(journal);
    umi_desktop_session_recovery_destroy(reader);
    umi_desktop_session_recovery_destroy(writer);
    umi_desktop_runtime_destroy(second);
    umi_desktop_runtime_destroy(first);
    umi_application_context_hub_destroy(second_hub);
    umi_application_context_hub_destroy(first_hub);
    return 0;
}
