/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_navigation_history.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration navigation history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/navigation_history.h"
int main(void)
{
    UmiIdeNavigationHistory *history = NULL;
    UmiIdeNavigationTarget first = {0};
    UmiIdeNavigationTarget second = {0};
    UmiIdeNavigationTarget current;

    assert(umi_ide_navigation_history_create(&history) == UMI_STATUS_OK);
    first.domain = UMI_IDE_DOMAIN_EDITOR;
    (void)strcpy(first.label, "one");
    umi_ide_location_init(&first.location);
    assert(umi_ide_location_set_path(
        &first.location, "one.c", 1U, 0U) == UMI_STATUS_OK);

    second = first;
    (void)strcpy(second.label, "two");
    (void)strcpy(second.location.path, "two.c");

    assert(umi_ide_navigation_history_visit(history, &first) == UMI_STATUS_OK);
    assert(umi_ide_navigation_history_visit(history, &second) == UMI_STATUS_OK);
    assert(umi_ide_navigation_history_back(history, &current) == UMI_STATUS_OK);
    assert(strcmp(current.label, "one") == 0);
    assert(umi_ide_navigation_history_forward(history, &current) == UMI_STATUS_OK);
    assert(strcmp(current.label, "two") == 0);
    umi_ide_navigation_history_destroy(history);
    return 0;
}

