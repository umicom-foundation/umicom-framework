/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_dependency_gate.c
 * PURPOSE: Validate the Framework-owned application dependency gate contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_dependency_gate.h"

int main(void)
{
    UmiDesktopApplicationDependencyGateSnapshot value;
    UmiDesktopApplicationDependencyGateSnapshot copy;
    umi_desktop_application_dependency_gate_init(&value, "application_dependency_gate.primary");
    assert(umi_desktop_application_dependency_gate_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_dependency_gate_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_dependency_gate_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_dependency_gate_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_dependency_gate_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_dependency_gate_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_application_dependency_gate_ready(2U, 2U, false));
    return 0;
}
