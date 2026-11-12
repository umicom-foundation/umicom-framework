/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_cell_value.c
 *
 * PURPOSE:
 *   Exercise the cell value enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/cell_value.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Extend the original typed-value checks with borrowed text, complete refusal preservation and malformed active storage. Retain the original regression for review. The previous implementation is retained for review. */
#if 0
int main(void){UmiUiEntCellValue a,b;umi_ui_ent_cell_value_set_integer(&a,42);umi_ui_ent_cell_value_set_integer(&b,42);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_cell_value_equal(&a,&b))return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_cell_value_set_text(&a,"abc")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(a.kind!=UMI_UI_ENT_VALUE_TEXT)return 3;puts("ok");return 0;}

#endif
int main(void)
{
    UmiUiEntCellValue left = {0}, right = {0}, before;
    char too_long[UMI_UI_ENT_VALUE_CAPACITY];
    char exact[UMI_UI_ENT_VALUE_CAPACITY];
    (void)memset(too_long, 'x', sizeof(too_long));
    (void)memset(exact, 'e', sizeof(exact));
    exact[sizeof(exact) - 1U] = '\0';
    umi_ui_ent_cell_value_set_integer(&left, 42);
    umi_ui_ent_cell_value_set_integer(&right, 42);
    CHECK(umi_ui_ent_cell_value_equal(&left, &right));
    CHECK(umi_ui_ent_cell_value_set_text(&left, "abc") == UMI_STATUS_OK);
    CHECK(left.kind == UMI_UI_ENT_VALUE_TEXT);
    CHECK(umi_ui_ent_cell_value_set_text(&left, left.text + 1) == UMI_STATUS_OK);
    CHECK(strcmp(left.text, "bc") == 0);
    (void)memcpy(&before, &left, sizeof(left));
    CHECK(umi_ui_ent_cell_value_set_text(&left, too_long) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&left, &before, sizeof(left)) == 0);
    CHECK(umi_ui_ent_cell_value_set_text(&left, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&left, &before, sizeof(left)) == 0);
    CHECK(umi_ui_ent_cell_value_set_text(&left, exact) == UMI_STATUS_OK);
    CHECK(strlen(left.text) == sizeof(exact) - 1U);

    umi_ui_ent_cell_value_set_integer(&left, 72);
    (void)memcpy(&before, &left, sizeof(left));
    CHECK(umi_ui_ent_cell_value_set_text(&left, too_long) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&left, &before, sizeof(left)) == 0);
    CHECK(left.kind == UMI_UI_ENT_VALUE_INTEGER && left.integer_value == 72);

    CHECK(umi_ui_ent_cell_value_set_text(&left, "same") == UMI_STATUS_OK);
    CHECK(umi_ui_ent_cell_value_set_text(&right, "same") == UMI_STATUS_OK);
    CHECK(umi_ui_ent_cell_value_validate(&left) == UMI_STATUS_OK);
    CHECK(umi_ui_ent_cell_value_equal(&left, &right));
    (void)memset(right.text, 'x', sizeof(right.text));
    CHECK(umi_ui_ent_cell_value_validate(&right) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!umi_ui_ent_cell_value_equal(&left, &right));
    CHECK(!umi_ui_ent_cell_value_equal(&right, &left));
    CHECK(!umi_ui_ent_cell_value_equal(&right, &right));
    right.kind = (UmiUiEntValueKind)99;
    CHECK(umi_ui_ent_cell_value_validate(&right) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!umi_ui_ent_cell_value_equal(&right, &right));

    /* Unused text does not invalidate a correctly typed numeric record. */
    right.kind = UMI_UI_ENT_VALUE_INTEGER;
    right.integer_value = 72;
    umi_ui_ent_cell_value_set_integer(&left, 72);
    CHECK(umi_ui_ent_cell_value_validate(&right) == UMI_STATUS_OK);
    CHECK(umi_ui_ent_cell_value_equal(&left, &right));
    umi_ui_ent_cell_value_set_boolean(&left, 8);
    CHECK(left.boolean_value == 1);
    right.kind = UMI_UI_ENT_VALUE_BOOLEAN;
    right.boolean_value = 8;
    CHECK(umi_ui_ent_cell_value_validate(&right) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!umi_ui_ent_cell_value_equal(&right, &right));
    umi_ui_ent_cell_value_set_real(&left, NAN);
    CHECK(umi_ui_ent_cell_value_validate(&left) == UMI_STATUS_OK);
    CHECK(!umi_ui_ent_cell_value_equal(&left, &left));
    CHECK(umi_ui_ent_cell_value_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!umi_ui_ent_cell_value_equal(NULL, &left));
    CHECK(umi_ui_ent_cell_value_set_text(NULL, "x") == UMI_STATUS_INVALID_ARGUMENT);

    char overlapped[16] = "prefix";
    CHECK(umi_ui_ent_copy_text(overlapped, sizeof(overlapped), overlapped + 3) == UMI_STATUS_OK);
    CHECK(strcmp(overlapped, "fix") == 0);
    (void)puts("ok");
    return 0;
}