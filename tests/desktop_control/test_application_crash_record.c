/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_crash_record.c
 * PURPOSE: Validate the Framework-owned application crash record contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_crash_record.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopApplicationCrashRecordSnapshot value;
    UmiDesktopApplicationCrashRecordSnapshot copy;
    umi_desktop_application_crash_record_init(&value, "application_crash_record.primary");
    assert(umi_desktop_application_crash_record_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_crash_record_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_crash_record_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_crash_record_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_crash_record_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_crash_record_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
