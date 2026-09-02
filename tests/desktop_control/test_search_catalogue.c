/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_search_catalogue.c
 * PURPOSE: Validate the Framework-owned search catalogue contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/search_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesktopSearchCatalogueSnapshot value;
    UmiDesktopSearchCatalogueSnapshot copy;
    umi_desktop_search_catalogue_init(&value, "search_catalogue.primary");
    assert(umi_desktop_search_catalogue_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_search_catalogue_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_search_catalogue_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_search_catalogue_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_search_catalogue_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_search_catalogue_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    return 0;
}
