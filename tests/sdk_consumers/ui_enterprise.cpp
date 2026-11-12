/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_consumers/ui_enterprise.cpp
 *
 * PURPOSE:
 *   Exercise typed enterprise cells and by-value selection ranges through the installed C API from C++.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/ui/enterprise/cell_value.h"
#include "umicom/ui/enterprise/selection_range.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

int main()
{
    UmiUiEntCellValue left = {};
    UmiUiEntCellValue right = {};
    char input[] = "Available";
    CHECK(umi_ui_ent_cell_value_set_text(&left, input) == UMI_STATUS_OK);
    input[0] = 'X';
    CHECK(left.kind == UMI_UI_ENT_VALUE_TEXT);
    CHECK(strcmp(left.text, "Available") == 0);
    CHECK(umi_ui_ent_cell_value_set_text(&right, "Available") == UMI_STATUS_OK);
    CHECK(umi_ui_ent_cell_value_equal(&left, &right));

    umi_ui_ent_cell_value_set_integer(&left, 42);
    umi_ui_ent_cell_value_set_integer(&right, 42);
    CHECK(left.kind == UMI_UI_ENT_VALUE_INTEGER && left.integer_value == 42);
    CHECK(umi_ui_ent_cell_value_equal(&left, &right));
    umi_ui_ent_cell_value_set_real(&right, 42.0);
    CHECK(!umi_ui_ent_cell_value_equal(&left, &right));
    umi_ui_ent_cell_value_set_boolean(&left, 9);
    CHECK(left.kind == UMI_UI_ENT_VALUE_BOOLEAN && left.boolean_value == 1);
    umi_ui_ent_cell_value_clear(&left);
    CHECK(left.kind == UMI_UI_ENT_VALUE_EMPTY);

    /* Passing and returning this C structure by value must retain every bound. */
    UmiUiEntSelectionRange reversed = {9U, 2U, 5U, 1U};
    UmiUiEntSelectionRange normalized = umi_ui_ent_selection_range_normalize(reversed);
    CHECK(normalized.row_first == 2U && normalized.row_last == 9U);
    CHECK(normalized.column_first == 1U && normalized.column_last == 5U);
    CHECK(reversed.row_first == 9U && reversed.column_first == 5U);
    CHECK(umi_ui_ent_selection_range_contains(reversed, 2U, 1U));
    CHECK(umi_ui_ent_selection_range_contains(reversed, 9U, 5U));
    CHECK(!umi_ui_ent_selection_range_contains(reversed, 10U, 5U));
    CHECK(!umi_ui_ent_selection_range_contains(reversed, 2U, 0U));
    (void)puts("Umicom enterprise cell and selection values checked from C++.");
    return 0;
}
