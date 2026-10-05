/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_context_changes.c
 * PURPOSE: Check typed context transactions without executing any UI command.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/context_changes.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

int main(int argc, char **argv)
{
    UmiUiContextStore *store = NULL;
    UmiUiContextChange changes[3] = {0};
    UmiUiContextSnapshot snapshot;
    const char *scenario = argc == 2 ? argv[1] : "publication";
    uint64_t output = UINT64_MAX;
    CHECK(umi_ui_context_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_ui_context_set_string(store, "old", "before") == UMI_STATUS_OK);
    CHECK(umi_ui_context_set_integer(store, "untouched", 42) == UMI_STATUS_OK);
    uint64_t revision = umi_ui_context_revision(store);
    changes[0].operation = UMI_UI_CONTEXT_CHANGE_SET;
    strcpy(changes[0].value.key, "old");
    changes[0].value.kind = UMI_UI_CONTEXT_STRING;
    strcpy(changes[0].value.string_value, "after");
    changes[1].operation = UMI_UI_CONTEXT_CHANGE_SET;
    strcpy(changes[1].value.key, "flag");
    changes[1].value.kind = UMI_UI_CONTEXT_BOOLEAN;
    changes[1].value.boolean_value = 1;
    changes[2].operation = UMI_UI_CONTEXT_CHANGE_SET;
    strcpy(changes[2].value.key, "number");
    changes[2].value.kind = UMI_UI_CONTEXT_INTEGER;
    changes[2].value.integer_value = INT64_MIN;

    if (strcmp(scenario, "read") == 0 || strcmp(scenario, "read_invalid") == 0) {
        const char *keys[] = {"old", "untouched", "missing", "old"};
        UmiUiContextObservation observations[4] = {0};
        CHECK(UmiUiContextReadKeys(store, keys, 4U, observations, &output) == UMI_STATUS_OK);
        CHECK(output == revision && observations[0].found && observations[1].found);
        CHECK(!observations[2].found && observations[2].value.key[0] == '\0');
        CHECK(observations[1].value.kind == UMI_UI_CONTEXT_INTEGER && observations[1].value.integer_value == 42);
        CHECK(strcmp(observations[3].value.string_value, "before") == 0);
        if (strcmp(scenario, "read_invalid") == 0) {
            keys[3] = NULL;
            CHECK(UmiUiContextReadKeys(store, keys, 4U, observations, &output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(output == revision && strcmp(observations[3].value.string_value, "before") == 0);
            CHECK(UmiUiContextReadKeys(store, keys, UMI_UI_CONTEXT_MAX + 1U,
                observations, &output) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(output == revision);
        } else {
            CHECK(umi_ui_context_set_string(store, "old", "later") == UMI_STATUS_OK);
            CHECK(strcmp(observations[0].value.string_value, "before") == 0);
            CHECK(UmiUiContextReadKeys(store, NULL, 0U, NULL, &output) == UMI_STATUS_OK);
            CHECK(output == revision + 1U);
        }
    } else if (strcmp(scenario, "publication") == 0) {
        CHECK(UmiUiContextApplyChanges(store, revision, changes, 3U, &output) == UMI_STATUS_OK);
        CHECK(output == revision + 1U && umi_ui_context_revision(store) == output);
        CHECK(umi_ui_context_get(store, "number", &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.kind == UMI_UI_CONTEXT_INTEGER && snapshot.integer_value == INT64_MIN);
        CHECK(umi_ui_context_evaluate(store, "flag&&old=after"));
    } else if (strcmp(scenario, "empty") == 0) {
        CHECK(UmiUiContextApplyChanges(store, revision, NULL, 0U, &output) == UMI_STATUS_OK);
        CHECK(output == revision);
    } else if (strcmp(scenario, "replacement") == 0 || strcmp(scenario, "capacity") == 0) {
        char key[64];
        for (size_t index = 0U; umi_ui_context_count(store) < UMI_UI_CONTEXT_MAX; ++index) {
            (void)snprintf(key, sizeof(key), "filled.%zu", index);
            CHECK(umi_ui_context_set_integer(store, key, 5) == UMI_STATUS_OK);
        }
        revision = umi_ui_context_revision(store);
        if (strcmp(scenario, "replacement") == 0) {
            /* Input order must not require temporary spare capacity. */
            changes[0] = changes[1];
            changes[1].operation = UMI_UI_CONTEXT_CHANGE_REMOVE;
            strcpy(changes[1].value.key, "old");
            CHECK(UmiUiContextApplyChanges(store, revision, changes, 2U, &output) == UMI_STATUS_OK);
            CHECK(umi_ui_context_count(store) == UMI_UI_CONTEXT_MAX);
            CHECK(umi_ui_context_get(store, "old", &snapshot) == UMI_STATUS_NOT_FOUND);
            CHECK(umi_ui_context_get(store, "flag", &snapshot) == UMI_STATUS_OK);
        } else {
            CHECK(UmiUiContextApplyChanges(store, revision, changes, 2U, &output) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(output == UINT64_MAX && umi_ui_context_revision(store) == revision);
            CHECK(umi_ui_context_get(store, "old", &snapshot) == UMI_STATUS_OK);
            CHECK(strcmp(snapshot.string_value, "before") == 0);
        }
    } else if (strcmp(scenario, "legacy_text") == 0) {
        char text[UMI_UI_TEXT_CAPACITY + 1U];
        memset(text, 'x', sizeof(text)); text[sizeof(text) - 1U] = '\0';
        CHECK(umi_ui_context_set_string(store, "old", text) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ui_context_set_integer(store, text, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ui_context_set_boolean(store, text, 1) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ui_context_revision(store) == revision);
        CHECK(umi_ui_context_get(store, "old", &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.string_value, "before") == 0);
    } else {
        UmiStatus expected;
        size_t count = 2U;
        if (strcmp(scenario, "duplicate") == 0) {
            changes[1] = changes[0]; expected = UMI_STATUS_ALREADY_EXISTS;
        } else if (strcmp(scenario, "missing") == 0) {
            changes[1].operation = UMI_UI_CONTEXT_CHANGE_REMOVE; expected = UMI_STATUS_NOT_FOUND;
        } else if (strcmp(scenario, "invalid") == 0) {
            changes[1].value.boolean_value = 2; expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(scenario, "unterminated") == 0) {
            memset(changes[1].value.key, 'x', sizeof(changes[1].value.key)); expected = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(scenario, "count") == 0) {
            count = UMI_UI_CONTEXT_MAX + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED;
        } else {
            CHECK(strcmp(scenario, "stale") == 0);
            expected = UMI_STATUS_INVALID_STATE;
        }
        CHECK(UmiUiContextApplyChanges(store, revision - (strcmp(scenario, "stale") == 0 ? 1U : 0U),
            changes, count, &output) == expected);
        CHECK(output == UINT64_MAX && umi_ui_context_revision(store) == revision);
        CHECK(umi_ui_context_get(store, "old", &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.string_value, "before") == 0);
    }
    CHECK(umi_ui_context_get(store, "untouched", &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.kind == UMI_UI_CONTEXT_INTEGER && snapshot.integer_value == 42);
    umi_ui_context_store_destroy(store);
    return 0;
}
