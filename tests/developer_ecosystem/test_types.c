/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_types.c
 * PURPOSE: Verify ecosystem enum and case-insensitive text helpers.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer/ecosystem/types.h"

int main(void)
{
    char text[16];
    assert(umi_ecosystem_copy_text(text, sizeof(text), "Package") == UMI_STATUS_OK);
    assert(strcmp(text, "Package") == 0);
    assert(umi_ecosystem_text_contains("Component Template", "template"));
    assert(!umi_ecosystem_text_contains("Component", "extension"));
    assert(strcmp(umi_ecosystem_source_kind_text(UMI_ECOSYSTEM_SOURCE_OFFLINE),
        "offline") == 0);
    assert(strcmp(umi_ecosystem_package_kind_text(UMI_ECOSYSTEM_PACKAGE_SDK),
        "sdk") == 0);
    assert(strcmp(umi_ecosystem_action_kind_text(UMI_ECOSYSTEM_ACTION_VERIFY),
        "verify") == 0);
    return 0;
}
