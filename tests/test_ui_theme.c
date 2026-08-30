/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_theme.c
 *
 * PURPOSE:
 *   Verify semantic theme-token registration and replacement.
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
    UmiUiThemeModel *theme = NULL;
    UmiUiThemeSnapshot token = {0};
    UmiUiThemeSnapshot found;
    assert(umi_ui_theme_model_create(&theme) == UMI_STATUS_OK);
    (void)snprintf(token.token_id, sizeof(token.token_id), "%s", "colour.background");
    (void)snprintf(token.value, sizeof(token.value), "%s", "#07111b");
    assert(umi_ui_theme_model_upsert(theme, &token) == UMI_STATUS_OK);
    assert(umi_ui_theme_model_find(theme, token.token_id, &found) == UMI_STATUS_OK);
    assert(strcmp(found.value, "#07111b") == 0);
    umi_ui_theme_model_destroy(theme);
    return EXIT_SUCCESS;
}
