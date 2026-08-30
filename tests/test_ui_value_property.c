/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_value_property.c
 *
 * PURPOSE:
 *   Verify owned UI values, property replacement, lookup, removal and revision tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
    UmiUiValue value;
    UmiUiValue copy;
    UmiUiPropertyBag *bag = NULL;
    UmiUiPropertySnapshot property;
    uint64_t revision;
    assert(umi_ui_value_set_string(&value, "Umicom") == UMI_STATUS_OK);
    assert(umi_ui_value_copy(&copy, &value) == UMI_STATUS_OK);
    assert(umi_ui_value_equal(&value, &copy));
    assert(umi_ui_property_bag_create(&bag) == UMI_STATUS_OK);
    revision = umi_ui_property_bag_revision(bag);
    assert(umi_ui_property_bag_set(bag, "title", &value) == UMI_STATUS_OK);
    assert(umi_ui_property_bag_revision(bag) > revision);
    assert(umi_ui_property_bag_get(bag, "title", &copy) == UMI_STATUS_OK);
    assert(strcmp(copy.string_value, "Umicom") == 0);
    assert(umi_ui_property_bag_at(bag, 0U, &property) == UMI_STATUS_OK);
    assert(strcmp(property.key, "title") == 0);
    assert(umi_ui_property_bag_remove(bag, "title") == UMI_STATUS_OK);
    assert(umi_ui_property_bag_count(bag) == 0U);
    umi_ui_property_bag_destroy(bag);
    return EXIT_SUCCESS;
}
