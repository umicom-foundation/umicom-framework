/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_command_search.c
 * PURPOSE: Verify shared command discovery, bounded paging and rejected-input state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/command_bar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #value); \
    failed = 1; goto cleanup; } } while (0)

/* Product-like labels exercise the common model without dispatching any IDE,
 * broker, banking, provider or operating-system action. */
static UmiStatus add(UmiWsCommandBarModel *model, const char *id, const char *title,
    const char *description, const char *keywords, uint32_t priority)
{
    return umi_ws_command_bar_model_add(model, id, title, description, id,
        keywords, UMI_WS_COMMAND_SCOPE_COMMAND, priority);
}

int main(int argc, char **argv)
{
    UmiWsCommandBarModel *model = calloc(1U, sizeof(*model));
    UmiWsCommandBarModel *before = calloc(1U, sizeof(*before));
    UmiWsCommandBarPage page = {0};
    UmiWsCommandBarQuery query = {0}, saved;
    char long_text[UMI_UI_TEXT_CAPACITY + 8U];
    const char *name = argc == 2 ? argv[1] : "";
    int failed = 0;
    CHECK(model != NULL && before != NULL);
    umi_ws_command_bar_model_init(model);
    CHECK(add(model, "studio.compile", "Build project", "Compile the selected workspace",
        "debug C compiler", 30U) == UMI_STATUS_OK);
    CHECK(add(model, "studio.help", "Build help", "Read about build project settings",
        "manual", 900U) == UMI_STATUS_OK);
    CHECK(add(model, "media.export", "Export video", "Render selected timeline",
        "movie encoder", 30U) == UMI_STATUS_OK);
    CHECK(add(model, "bank.review", "Review transfer", "Inspect selected account",
        "currency", 30U) == UMI_STATUS_OK);

    if (strcmp(name, "words") == 0) {
        CHECK(umi_ws_command_bar_model_set_query(model, "workspace compiler") == UMI_STATUS_OK);
        CHECK(model->result_count == 1U);
        CHECK(strcmp(umi_ws_command_bar_model_selected(model)->item_id, "studio.compile") == 0);
    } else if (strcmp(name, "rank") == 0) {
        CHECK(umi_ws_command_bar_model_set_query(model, "BUILD PROJECT") == UMI_STATUS_OK);
        CHECK(model->result_count == 2U);
        CHECK(strcmp(umi_ws_command_bar_model_selected(model)->item_id, "studio.compile") == 0);
    } else if (strcmp(name, "scope") == 0) {
        CHECK(umi_ws_command_bar_model_set_query(model, " \t+ build \r\n") == UMI_STATUS_OK);
        CHECK(model->result_count == 0U && model->query.scope == UMI_WS_COMMAND_SCOPE_PANEL);
        CHECK(strcmp(model->query.text, "build") == 0);
    } else if (strcmp(name, "all-words") == 0) {
        CHECK(umi_ws_command_bar_model_set_query(model, "video missing") == UMI_STATUS_OK);
        CHECK(model->result_count == 0U);
    } else if (strcmp(name, "stable-order") == 0) {
        CHECK(umi_ws_command_bar_model_set_query(model, "selected") == UMI_STATUS_OK);
        CHECK(model->result_count == 3U);
        CHECK(strcmp(umi_ws_command_bar_model_result_at(model, 0U)->item_id, "studio.compile") == 0);
        CHECK(strcmp(umi_ws_command_bar_model_result_at(model, 1U)->item_id, "media.export") == 0);
        CHECK(strcmp(umi_ws_command_bar_model_result_at(model, 2U)->item_id, "bank.review") == 0);
    } else if (strcmp(name, "unicode") == 0) {
        CHECK(add(model, "desk.cafe", "Caf\xc3\xa9", "Open workspace", "desktop", 1U) == UMI_STATUS_OK);
        CHECK(umi_ws_command_bar_model_set_query(model, "caf\xc3\xa9 desktop") == UMI_STATUS_OK);
        CHECK(model->result_count == 1U);
    } else if (strcmp(name, "parse-atomic") == 0) {
        memset(long_text, 'x', sizeof(long_text)); long_text[sizeof(long_text)-1U] = '\0';
        CHECK(umi_ws_command_bar_parse("+ Memory", &query) == UMI_STATUS_OK);
        saved = query;
        CHECK(umi_ws_command_bar_parse(long_text, &query) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&saved, &query, sizeof(query)) == 0);
    } else if (strcmp(name, "parse-alias") == 0) {
        (void)snprintf(query.text, sizeof(query.text), "> compile");
        CHECK(umi_ws_command_bar_parse(query.text, &query) == UMI_STATUS_OK);
        CHECK(query.scope == UMI_WS_COMMAND_SCOPE_COMMAND && strcmp(query.text, "compile") == 0);
    } else if (strcmp(name, "query-atomic") == 0) {
        memset(long_text, 'x', sizeof(long_text)); long_text[sizeof(long_text)-1U] = '\0';
        *before = *model;
        CHECK(umi_ws_command_bar_model_set_query(model, long_text) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, model, sizeof(*model)) == 0);
    } else if (strcmp(name, "pages") == 0) {
        CHECK(umi_ws_command_bar_model_select_result(model, 3U) == UMI_STATUS_OK);
        CHECK(umi_ws_command_bar_model_page(model, 3U, &page) == UMI_STATUS_OK);
        CHECK(page.first_result == 3U && page.result_count == 1U &&
            page.total_results == 4U && page.has_previous && !page.has_next);
        CHECK(umi_ws_command_bar_model_page(model, SIZE_MAX, &page) == UMI_STATUS_OK);
        CHECK(page.first_result == 0U && page.result_count == 4U && !page.has_previous);
    } else if (strcmp(name, "empty") == 0) {
        CHECK(umi_ws_command_bar_model_set_query(model, "no-such-command") == UMI_STATUS_OK);
        CHECK(umi_ws_command_bar_model_page(model, 2U, &page) == UMI_STATUS_OK);
        CHECK(page.total_results == 0U && page.result_count == 0U && !page.has_next);
        CHECK(umi_ws_command_bar_model_select_result(model, 0U) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "invalid-index") == 0) {
        *before = *model;
        CHECK(umi_ws_command_bar_model_select_result(model, SIZE_MAX) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(before, model, sizeof(*model)) == 0);
        model->result_indices[0] = model->count;
        CHECK(umi_ws_command_bar_model_result_at(model, 0U) == NULL);
        CHECK(umi_ws_command_bar_model_page(model, 2U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "invalid-count") == 0) {
        model->result_count = SIZE_MAX;
        CHECK(umi_ws_command_bar_model_result_at(model, 0U) == NULL);
        CHECK(umi_ws_command_bar_model_move_selection(model, INT32_MAX) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ws_command_bar_model_page(model, 2U, &page) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "extreme-offset") == 0) {
        CHECK(umi_ws_command_bar_model_move_selection(model, INT32_MAX) == UMI_STATUS_OK);
        CHECK(model->selected_result == 3U);
        CHECK(umi_ws_command_bar_model_move_selection(model, INT32_MIN) == UMI_STATUS_OK);
        CHECK(model->selected_result == 0U);
    } else if (strcmp(name, "page-output") == 0) {
        page.total_results = 123U;
        CHECK(umi_ws_command_bar_model_page(model, 0U, &page) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(page.total_results == 123U);
    } else {
        CHECK(0 && "Choose a registered test case");
    }
cleanup:
    free(before);
    free(model);
    return failed;
}
