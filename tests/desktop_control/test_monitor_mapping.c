/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_monitor_mapping.c
 * PURPOSE: Validate the Framework-owned monitor mapping contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/monitor_mapping.h"

int main(void)
{
    UmiDesktopMonitorMappingSnapshot value;
    UmiDesktopMonitorMappingSnapshot copy;
    umi_desktop_monitor_mapping_init(&value, "monitor_mapping.primary");
    assert(umi_desktop_monitor_mapping_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_mapping_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_monitor_mapping_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_monitor_mapping_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_monitor_mapping_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_monitor_mapping_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_monitor_mapping_confidence(true, false, false) == 100U);
    return 0;
}
