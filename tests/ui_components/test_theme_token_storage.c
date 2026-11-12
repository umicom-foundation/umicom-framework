/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_theme_token_storage.c
 *
 * PURPOSE:
 *   Verify complete semantic token admission, bounded queries, independent outputs and full-capacity replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/ui/theme.h"
#include <stdio.h>
#include <string.h>

/* Checks remain active in optimized owner qualification builds. Each scenario
 * releases its model on both success and failure. */
#define CHECK(expression) do { if (!(expression)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #expression); \
    goto cleanup; \
} } while (0)

static int admission(void)
{
    int result = 1;
    UmiUiThemeModel *model = NULL;
    UmiUiThemeSnapshot token = {0}, found = {0}, saved = {0}, invalid;
    CHECK(umi_ui_theme_model_create(&model) == UMI_STATUS_OK);
    (void)memset(token.token_id, 'i', sizeof(token.token_id) - 1U);
    (void)memset(token.value, 'v', sizeof(token.value) - 1U);
    (void)memset(token.description, 'd', sizeof(token.description) - 1U);
    CHECK(umi_ui_theme_model_upsert(model, &token) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_find(model, token.token_id, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.value, token.value) == 0);
    CHECK(strcmp(found.description, token.description) == 0);
    saved = found;
    const uint64_t revision = umi_ui_theme_model_revision(model);
    for (unsigned field = 0U; field < 3U; ++field) {
        invalid = token;
        if (field == 0U) (void)memset(invalid.token_id, 'x', sizeof(invalid.token_id));
        if (field == 1U) (void)memset(invalid.value, 'x', sizeof(invalid.value));
        if (field == 2U) (void)memset(invalid.description, 'x', sizeof(invalid.description));
        CHECK(umi_ui_theme_model_upsert(model, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_theme_model_count(model) == 1U);
        CHECK(umi_ui_theme_model_revision(model) == revision);
        CHECK(umi_ui_theme_model_at(model, 0U, &found) == UMI_STATUS_OK);
        CHECK(memcmp(&found, &saved, sizeof(found)) == 0);
    }
    invalid = token;
    invalid.token_id[0] = '\0';
    CHECK(umi_ui_theme_model_upsert(model, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid.token_id[0] = ' ';
    CHECK(umi_ui_theme_model_upsert(model, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_upsert(model, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_upsert(NULL, &token) == UMI_STATUS_INVALID_ARGUMENT);
    /* The model exposes no borrowed item pointer. Forged aliases must be
     * refused before reading or writing its private allocation. */
    CHECK(umi_ui_theme_model_upsert(model, (const UmiUiThemeSnapshot *)model)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_revision(model) == revision);
    token.value[0] = '\0';
    token.description[0] = '\0';
    CHECK(umi_ui_theme_model_upsert(model, &token) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_count(model) == 1U);
    CHECK(umi_ui_theme_model_find(model, token.token_id, &found) == UMI_STATUS_OK);
    CHECK(found.value[0] == '\0' && found.description[0] == '\0');
    result = 0;
cleanup:
    umi_ui_theme_model_destroy(model);
    return result;
}

static int queries(void)
{
    int result = 1;
    UmiUiThemeModel *model = NULL;
    UmiUiThemeSnapshot token = {0}, output, before;
    char unterminated[UMI_UI_ID_CAPACITY];
    (void)memset(unterminated, 'q', sizeof(unterminated));
    (void)snprintf(token.token_id, sizeof(token.token_id), "%s", "text.code.font");
    (void)snprintf(token.value, sizeof(token.value), "%s", "Cascadia Code, Consolas");
    CHECK(umi_ui_theme_model_create(&model) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_upsert(model, &token) == UMI_STATUS_OK);
    const uint64_t revision = umi_ui_theme_model_revision(model);
    (void)memset(&output, 0x5a, sizeof(output));
    (void)memcpy(&before, &output, sizeof(before));
    const char *invalid[] = {NULL, "", "invalid token", unterminated};
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        CHECK(umi_ui_theme_model_find(model, invalid[i], &output)
              == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&output, &before, sizeof(output)) == 0);
        CHECK(umi_ui_theme_model_remove(model, invalid[i]) == UMI_STATUS_INVALID_ARGUMENT);
    }
    CHECK(umi_ui_theme_model_find(model, "missing", &output) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_ui_theme_model_at(model, SIZE_MAX, &output) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_ui_theme_model_at(model, 1U, &output) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_ui_theme_model_find(NULL, token.token_id, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_at(NULL, 0U, &output) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&output, &before, sizeof(output)) == 0);
    CHECK(umi_ui_theme_model_find(model, token.token_id, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_at(model, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_find(model, token.token_id, (UmiUiThemeSnapshot *)model)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_at(model, 0U, (UmiUiThemeSnapshot *)model)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ui_theme_model_remove(model, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_ui_theme_model_count(model) == 1U);
    CHECK(umi_ui_theme_model_revision(model) == revision);
    (void)snprintf(output.token_id, sizeof(output.token_id), "%s", token.token_id);
    CHECK(umi_ui_theme_model_find(model, output.token_id, &output) == UMI_STATUS_OK);
    CHECK(strcmp(output.value, token.value) == 0);
    CHECK(umi_ui_theme_model_remove(model, output.token_id) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_count(model) == 0U);
    CHECK(umi_ui_theme_model_revision(model) == revision + 1U);
    CHECK(umi_ui_theme_model_upsert(model, &output) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_at(model, 0U, &output) == UMI_STATUS_OK);
    CHECK(strcmp(output.token_id, token.token_id) == 0);
    result = 0;
cleanup:
    umi_ui_theme_model_destroy(model);
    return result;
}

static int capacity_and_order(void)
{
    int result = 1;
    UmiUiThemeModel *model = NULL;
    UmiUiThemeSnapshot token = {0}, found = {0};
    CHECK(umi_ui_theme_model_create(&model) == UMI_STATUS_OK);
    for (size_t i = 0U; i < UMI_UI_THEME_MAX; ++i) {
        (void)snprintf(token.token_id, sizeof(token.token_id), "token.%zu", i);
        CHECK(umi_ui_theme_model_upsert(model, &token) == UMI_STATUS_OK);
    }
    const uint64_t full_revision = umi_ui_theme_model_revision(model);
    (void)snprintf(token.token_id, sizeof(token.token_id), "%s", "new.token");
    CHECK(umi_ui_theme_model_upsert(model, &token) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_ui_theme_model_count(model) == UMI_UI_THEME_MAX);
    CHECK(umi_ui_theme_model_revision(model) == full_revision);
    (void)snprintf(token.token_id, sizeof(token.token_id), "%s", "token.0");
    (void)snprintf(token.value, sizeof(token.value), "%s", "replacement");
    CHECK(umi_ui_theme_model_upsert(model, &token) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_count(model) == UMI_UI_THEME_MAX);
    CHECK(umi_ui_theme_model_find(model, "token.0", &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.value, "replacement") == 0);
    CHECK(umi_ui_theme_model_remove(model, "token.0") == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_at(model, 0U, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.token_id, "token.1") == 0);
    (void)snprintf(token.token_id, sizeof(token.token_id), "%s", "new.token");
    CHECK(umi_ui_theme_model_upsert(model, &token) == UMI_STATUS_OK);
    CHECK(umi_ui_theme_model_at(model, UMI_UI_THEME_MAX - 1U, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.token_id, "new.token") == 0);
    CHECK(umi_ui_theme_model_find(model, "token.0", &found) == UMI_STATUS_NOT_FOUND);
    result = 0;
cleanup:
    umi_ui_theme_model_destroy(model);
    return result;
}

int main(void)
{
    if (admission() != 0 || queries() != 0 || capacity_and_order() != 0) return 1;
    return 0;
}
