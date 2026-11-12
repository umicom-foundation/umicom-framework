/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_accessibility_publication.c
 *
 * PURPOSE:
 *   Verify accessible labels publish both borrowed fields together and preserve previous values on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/ui/components/accessibility.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

int main(void)
{
    UmiUiComponentAccessibility value = {0};
    UmiUiComponentAccessibility before;
    char too_long[UMI_UI_ACCESSIBLE_TEXT_CAPACITY];
    char exact[UMI_UI_ACCESSIBLE_TEXT_CAPACITY];
    (void)memset(too_long, 'x', sizeof(too_long));
    (void)memset(exact, 'e', sizeof(exact));
    exact[sizeof(exact) - 1U] = '\0';
    CHECK(umi_ui_component_accessibility_set(&value, "Save", "Commit the record") == UMI_STATUS_OK);
    (void)memcpy(&before, &value, sizeof(value));
    CHECK(umi_ui_component_accessibility_set(&value, "New name", too_long) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_component_accessibility_set(&value, too_long, "New description") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    CHECK(umi_ui_component_accessibility_set(&value, value.description, value.name) == UMI_STATUS_OK);
    CHECK(strcmp(value.name, "Commit the record") == 0);
    CHECK(strcmp(value.description, "Save") == 0);
    CHECK(umi_ui_component_accessibility_set(&value, value.name + 7, value.description + 1) == UMI_STATUS_OK);
    CHECK(strcmp(value.name, "the record") == 0 && strcmp(value.description, "ave") == 0);
    CHECK(umi_ui_component_accessibility_set(&value, exact, exact) == UMI_STATUS_OK);
    CHECK(strlen(value.name) == sizeof(exact) - 1U);
    CHECK(strlen(value.description) == sizeof(exact) - 1U);
    CHECK(umi_ui_component_accessibility_set(&value, NULL, NULL) == UMI_STATUS_OK);
    CHECK(value.name[0] == '\0' && value.description[0] == '\0');
    CHECK(umi_ui_component_accessibility_set(NULL, "Name", "Description") == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
