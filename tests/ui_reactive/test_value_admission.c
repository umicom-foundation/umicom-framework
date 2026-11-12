/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_value_admission.c
 *
 * PURPOSE:
 *   Verify bounded reactive store publication and shared value admission in archived envelopes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/reactive/observable_store.h"
#include "umicom/ui/reactive/change_event.h"
#include "umicom/ui/reactive/command_parameter.h"
#include "umicom/ui/reactive/expression_operand.h"
#include "umicom/ui/reactive/expression_evaluator.h"
#include "umicom/ui/reactive/converter.h"
#include "umicom/ui/reactive/validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return 1; } } while (0)

/* Store fixtures live on the heap: capacity checks should not depend on a
 * large process stack. Public put operations establish the initial owner. */
static int CheckStore(const char *action, const char *name)
{
    UmiUiReactiveObservableStore *store = malloc(sizeof(*store));
    UmiUiReactiveObservableStore *before = malloc(sizeof(*before));
    UmiUiValue value, output, output_before;
    char key_storage[UMI_UI_REACTIVE_PATH_CAPACITY + 1U];
    const char *key = "alpha";
    UmiStatus expected = UMI_STATUS_OK;
    int put = strcmp(action, "put") == 0;
    CHECK(store != NULL && before != NULL);
    umi_ui_reactive_observable_store_init(store);
    CHECK(umi_ui_value_set_integer(&value, 10) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "alpha", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_integer(&value, 20) == UMI_STATUS_OK);
    CHECK(umi_ui_reactive_observable_store_put(store, "beta", &value) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_integer(&value, 30) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_string(&output, "retained output") == UMI_STATUS_OK);
    const UmiUiValue *source = &value;
    UmiUiReactiveObservableStore *argument = store;
    UmiUiValue *destination = &output;

    if (strcmp(name, "existing") == 0) { /* default */ }
    else if (strcmp(name, "new") == 0) { key = "new"; if (!put) expected = UMI_STATUS_NOT_FOUND; }
    else if (strcmp(name, "null-store") == 0) { argument = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null-key") == 0) { key = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "empty-key") == 0) { key = ""; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "long-key") == 0) {
        memset(key_storage, 'k', sizeof(key_storage)); key = key_storage;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "limit-key") == 0) {
        memset(key_storage, 'k', sizeof(key_storage)); key_storage[UMI_UI_REACTIVE_PATH_CAPACITY - 1U] = '\0';
        key = key_storage;
        if (!put) CHECK(umi_ui_reactive_observable_store_put(store, key, &value) == UMI_STATUS_OK);
    } else if (strcmp(name, "null-value") == 0) { source = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "null-output") == 0) { destination = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "unknown-value") == 0) { value.kind = (UmiUiValueKind)99; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "unterminated-value") == 0) {
        value.kind = UMI_UI_VALUE_STRING; memset(value.string_value, 'x', sizeof(value.string_value));
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "count") == 0) {
        store->count = UMI_UI_REACTIVE_MAX_ITEMS + 1U; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "stored-key") == 0) {
        memset(store->items[1].key, 'x', sizeof(store->items[1].key)); expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "stored-empty-key") == 0) {
        store->items[1].key[0] = '\0'; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "stored-kind") == 0) {
        store->items[1].value.kind = (UmiUiValueKind)99; expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "stored-text") == 0) {
        store->items[1].value.kind = UMI_UI_VALUE_STRING;
        memset(store->items[1].value.string_value, 'x', sizeof(store->items[1].value.string_value));
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "revision") == 0 || strcmp(name, "revision-new") == 0) {
        store->revision = UINT64_MAX;
        if (strcmp(name, "revision-new") == 0) key = "new";
        expected = put ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    } else if (strcmp(name, "last-revision") == 0) store->revision = UINT64_MAX - 1U;
    else if (strcmp(name, "borrowed") == 0) source = &store->items[1].value;
    else if (strcmp(name, "full-new") == 0 || strcmp(name, "full-existing") == 0) {
        for (size_t i = store->count; i < UMI_UI_REACTIVE_MAX_ITEMS; ++i) {
            char item_key[32];
            (void)snprintf(item_key, sizeof(item_key), "item.%zu", i);
            CHECK(umi_ui_reactive_observable_store_put(store, item_key, &value) == UMI_STATUS_OK);
        }
        if (strcmp(name, "full-new") == 0) { key = "new"; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    } else return 1;
    memcpy(before, store, sizeof(*before));
    memcpy(&output_before, &output, sizeof(output));
    UmiStatus actual = put ?
        umi_ui_reactive_observable_store_put(argument, key, source) :
        umi_ui_reactive_observable_store_get(argument, key, destination);
    CHECK(actual == expected);
    if (!put || expected != UMI_STATUS_OK) CHECK(memcmp(store, before, sizeof(*store)) == 0);
    if (!put && expected != UMI_STATUS_OK) CHECK(memcmp(&output, &output_before, sizeof(output)) == 0);
    if (put && expected == UMI_STATUS_OK) {
        CHECK(store->revision == before->revision + 1U);
        const int inserted = strcmp(name, "new") == 0 || strcmp(name, "limit-key") == 0;
        CHECK(store->count == before->count + (inserted ? 1U : 0U));
        const size_t index = inserted ? before->count : 0U;
        CHECK(store->items[index].revision == store->revision);
        CHECK(store->items[index].value.integer_value == (strcmp(name, "borrowed") == 0 ? 20 : 30));
        CHECK(store->items[1].value.integer_value == 20 && store->items[1].revision == before->items[1].revision);
        CHECK(umi_ui_value_set_integer(&value, 90) == UMI_STATUS_OK);
        CHECK(store->items[index].value.integer_value != 90);
        if (strcmp(name, "last-revision") == 0) {
            memcpy(before, store, sizeof(*before));
            CHECK(umi_ui_reactive_observable_store_put(store, "alpha", &value) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(memcmp(store, before, sizeof(*store)) == 0);
        }
    } else if (!put && expected == UMI_STATUS_OK) {
        CHECK(output.kind == UMI_UI_VALUE_INTEGER);
        CHECK(output.integer_value == (strcmp(name, "limit-key") == 0 ? 30 : 10));
    }
    free(before); free(store);
    return 0;
}

static int SelectKind(const char *name, UmiUiValueKind *kind, int *valid)
{
    *valid = 1;
    if (strcmp(name, "none") == 0) *kind = UMI_UI_VALUE_NONE;
    else if (strcmp(name, "boolean") == 0) *kind = UMI_UI_VALUE_BOOLEAN;
    else if (strcmp(name, "integer") == 0) *kind = UMI_UI_VALUE_INTEGER;
    else if (strcmp(name, "real") == 0) *kind = UMI_UI_VALUE_REAL;
    else if (strcmp(name, "string") == 0) *kind = UMI_UI_VALUE_STRING;
    else if (strcmp(name, "unknown") == 0) { *kind = (UmiUiValueKind)99; *valid = 0; }
    else if (strcmp(name, "negative") == 0) { *kind = (UmiUiValueKind)-1; *valid = 0; }
    else return 1;
    return 0;
}

/* Each wrapper exercises its real owner and real archive entry point. The
 * macro shares assertion mechanics, not a replacement serializer or validator. */
#define ENVELOPE_CASE(Name, Type, Prefix, KindField) \
static int Name(const char *name) \
{ \
    Type record; UmiUiValueKind kind; int valid; \
    unsigned char bytes[64], retained[64]; size_t size = 77U; \
    Prefix##_init(&record); \
    CHECK(SelectKind(name, &kind, &valid) == 0); \
    record.KindField = kind; \
    CHECK(umi_ui_value_kind_valid(kind) == valid); \
    CHECK(Prefix##_valid(&record) == valid); \
    if (!valid) { \
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(retained, bytes, sizeof(bytes)); \
        CHECK(Prefix##_archive_encode(&record, bytes, sizeof(bytes), &size) == UMI_STATUS_INVALID_ARGUMENT); \
        CHECK(size == 77U && memcmp(bytes, retained, sizeof(bytes)) == 0); \
    } else { \
        CHECK(Prefix##_archive_encode(&record, NULL, 0U, &size) == UMI_STATUS_OK); \
        CHECK(size > 0U); \
    } \
    return 0; \
}
ENVELOPE_CASE(ChangeBefore, UmiUiReactiveChangeEvent, umi_ui_reactive_change_event, before_value.kind)
ENVELOPE_CASE(ChangeAfter, UmiUiReactiveChangeEvent, umi_ui_reactive_change_event, after_value.kind)
ENVELOPE_CASE(Parameter, UmiUiReactiveCommandParameter, umi_ui_reactive_command_parameter, value.kind)
ENVELOPE_CASE(Operand, UmiUiReactiveExpressionOperand, umi_ui_reactive_expression_operand, value.kind)
ENVELOPE_CASE(Evaluator, UmiUiReactiveExpressionEvaluator, umi_ui_reactive_expression_evaluator, result.kind)
ENVELOPE_CASE(ConverterSource, UmiUiReactiveConverter, umi_ui_reactive_converter, source_kind)
ENVELOPE_CASE(ConverterTarget, UmiUiReactiveConverter, umi_ui_reactive_converter, target_kind)
ENVELOPE_CASE(Validator, UmiUiReactiveValidator, umi_ui_reactive_validator, value_kind)
#undef ENVELOPE_CASE

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "put") == 0 || strcmp(argv[1], "get") == 0)
        return CheckStore(argv[1], argv[2]);
    if (strcmp(argv[1], "change-before") == 0) return ChangeBefore(argv[2]);
    if (strcmp(argv[1], "change-after") == 0) return ChangeAfter(argv[2]);
    if (strcmp(argv[1], "parameter") == 0) return Parameter(argv[2]);
    if (strcmp(argv[1], "operand") == 0) return Operand(argv[2]);
    if (strcmp(argv[1], "evaluator") == 0) return Evaluator(argv[2]);
    if (strcmp(argv[1], "converter-source") == 0) return ConverterSource(argv[2]);
    if (strcmp(argv[1], "converter-target") == 0) return ConverterTarget(argv[2]);
    if (strcmp(argv[1], "validator") == 0) return Validator(argv[2]);
    return 2;
}
