/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_application_catalogue_filter.c
 * PURPOSE: Validate the Framework-owned application catalogue filter contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/application_catalogue_filter.h"

int main(void)
{
    UmiDesktopApplicationCatalogueFilterSnapshot value;
    UmiDesktopApplicationCatalogueFilterSnapshot copy;
    umi_desktop_application_catalogue_filter_init(&value, "application_catalogue_filter.primary");
    assert(umi_desktop_application_catalogue_filter_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_application_catalogue_filter_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_application_catalogue_filter_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_application_catalogue_filter_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_application_catalogue_filter_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_application_catalogue_filter_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    assert(umi_desktop_application_catalogue_filter_matches("studio", "Umicom Studio", "studio"));
    return 0;
}
