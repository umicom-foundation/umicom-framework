/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_context_binding.c
 * PURPOSE: Validate the Framework-owned context binding contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/context_binding.h"

int main(void)
{
    UmiDesktopContextBindingSnapshot value;
    UmiDesktopContextBindingSnapshot copy;
    umi_desktop_context_binding_init(&value, "context_binding.primary");
    assert(umi_desktop_context_binding_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_context_binding_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_context_binding_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_context_binding_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_context_binding_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_context_binding_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_context_binding_compatible("instrument", "instrument"));
    return 0;
}
