/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_event_log.c
 *
 * PURPOSE:
 *   Verify bounded chronological extension audit history.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "plugin_v2_fixture.h"

int main(void)
{
    UmiPluginEventLog *log = NULL;
    UmiPluginEvent event;
    assert(umi_plugin_event_log_create(2U, &log) == UMI_STATUS_OK);
    assert(umi_plugin_event_log_append(log, 10U, UMI_PLUGIN_EVENT_DISCOVERED,
        UMI_STATUS_OK, "one", "discovered") == UMI_STATUS_OK);
    assert(umi_plugin_event_log_append(log, 20U, UMI_PLUGIN_EVENT_ENABLED,
        UMI_STATUS_OK, "one", "enabled") == UMI_STATUS_OK);
    assert(umi_plugin_event_log_append(log, 30U, UMI_PLUGIN_EVENT_DISABLED,
        UMI_STATUS_OK, "one", "disabled") == UMI_STATUS_OK);
    assert(umi_plugin_event_log_count(log) == 2U);
    assert(umi_plugin_event_log_at(log, 0U, &event) == UMI_STATUS_OK);
    assert(event.sequence == 2U);
    assert(strcmp(umi_plugin_event_kind_text(event.kind), "enabled") == 0);
    umi_plugin_event_log_destroy(log);
    return 0;
}
