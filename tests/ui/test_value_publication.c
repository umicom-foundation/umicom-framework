/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_value_publication.c
 *
 * PURPOSE:
 *   Verify transactional UI strings, bounded comparisons and shared property admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/value.h"
#include "umicom/ui/property.h"
#include "umicom/ui/reactive/observable_value.h"
#include "umicom/ui/reactive/computed_value.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) {     fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

/* Construct boundary records through the public layout. Malformed fixtures
 * are deliberate caller inputs, not permission to read outside their storage. */
static int MakeValue(const char *name, UmiUiValue *value, int *valid, int *equal)
{
    umi_ui_value_clear(value);
    *valid = 1; *equal = 1;
    if (strcmp(name, "none") == 0) return 0;
    if (strcmp(name, "boolean") == 0) return umi_ui_value_set_boolean(value, 1) != UMI_STATUS_OK;
    if (strcmp(name, "integer") == 0) return umi_ui_value_set_integer(value, INT64_MIN) != UMI_STATUS_OK;
    if (strcmp(name, "real") == 0) return umi_ui_value_set_real(value, -12.25) != UMI_STATUS_OK;
    if (strcmp(name, "nan") == 0) { *equal = 0; return umi_ui_value_set_real(value, NAN) != UMI_STATUS_OK; }
    if (strcmp(name, "infinity") == 0) return umi_ui_value_set_real(value, INFINITY) != UMI_STATUS_OK;
    if (strcmp(name, "text") == 0) return umi_ui_value_set_string(value, "Umicom") != UMI_STATUS_OK;
    if (strcmp(name, "empty") == 0) return umi_ui_value_set_string(value, "") != UMI_STATUS_OK;
    if (strcmp(name, "last") == 0) {
        value->kind = UMI_UI_VALUE_STRING;
        memset(value->string_value, 'x', sizeof(value->string_value) - 1U);
        return 0;
    }
    if (strcmp(name, "unterminated") == 0) {
        value->kind = UMI_UI_VALUE_STRING;
        memset(value->string_value, 'x', sizeof(value->string_value));
        *valid = 0; *equal = 0; return 0;
    }
    if (strcmp(name, "unknown") == 0 || strcmp(name, "negative-kind") == 0) {
        value->kind = (UmiUiValueKind)(strcmp(name, "unknown") == 0 ? 99 : -1);
        *valid = 0; *equal = 0; return 0;
    }
    if (strcmp(name, "inactive-text") == 0) {
        CHECK(umi_ui_value_set_integer(value, 41) == UMI_STATUS_OK);
        memset(value->string_value, 'x', sizeof(value->string_value));
        return 0;
    }
    if (strcmp(name, "negative-boolean") == 0) {
        value->kind = UMI_UI_VALUE_BOOLEAN; value->boolean_value = -7; return 0;
    }
    return 1;
}
static int CheckRecord(const char *action, const char *name)
{
    UmiUiValue source, before, destination, retained;
    int valid, equal;
    CHECK(MakeValue(name, &source, &valid, &equal) == 0);
    memcpy(&before, &source, sizeof(before));
    CHECK(umi_ui_value_set_string(&destination, "retained") == UMI_STATUS_OK);
    memcpy(&retained, &destination, sizeof(retained));
    if (strcmp(action, "valid") == 0) CHECK(umi_ui_value_valid(&source) == valid);
    else if (strcmp(action, "copy") == 0) {
        CHECK(umi_ui_value_copy(&destination, &source) ==
            (valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
        /* Padding is not part of successful semantic equality. On refusal,
         * every byte belongs to the prior owner and must remain untouched. */
        if (!valid) CHECK(memcmp(&destination, &retained, sizeof(retained)) == 0);
        else {
            CHECK(destination.kind == source.kind);
            CHECK(destination.boolean_value == source.boolean_value);
            CHECK(destination.integer_value == source.integer_value);
            CHECK(isnan(source.real_value) ? isnan(destination.real_value) :
                destination.real_value == source.real_value);
            CHECK(memcmp(destination.string_value, source.string_value, sizeof(source.string_value)) == 0);
        }
    } else if (strcmp(action, "equal") == 0) {
        CHECK(umi_ui_value_equal(&source, &before) == equal);
        CHECK(umi_ui_value_equal(&before, &source) == equal);
        CHECK(umi_ui_value_equal(&source, &source) == equal);
    } else return 1;
    CHECK(memcmp(&source, &before, sizeof(source)) == 0);
    return 0;
}
static int CheckString(const char *name)
{
    UmiUiValue value, before;
    char external[UMI_UI_VALUE_STRING_CAPACITY + 1U];
    const char *input = "replacement";
    const char *expected = input;
    UmiStatus expected_status = UMI_STATUS_OK;
    CHECK(umi_ui_value_set_string(&value, "prefix/Umicom") == UMI_STATUS_OK);
    memset(external, 'x', sizeof(external));
    external[sizeof(external) - 1U] = '\0';
    if (strcmp(name, "empty") == 0) input = expected = "";
    else if (strcmp(name, "replace") == 0) { /* default fixture */ }
    else if (strcmp(name, "limit") == 0) {
        external[UMI_UI_VALUE_STRING_CAPACITY - 1U] = '\0'; input = expected = external;
    } else if (strcmp(name, "overflow") == 0) {
        input = external; expected_status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "unterminated") == 0) {
        memset(external, 'x', sizeof(external)); input = external; expected_status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "null-text") == 0) {
        input = NULL; expected_status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "null-destination") == 0) {
        CHECK(umi_ui_value_set_string(NULL, "text") == UMI_STATUS_INVALID_ARGUMENT); return 0;
    } else if (strcmp(name, "self") == 0) {
        input = value.string_value; expected = "prefix/Umicom";
    } else if (strcmp(name, "suffix") == 0) {
        input = value.string_value + 7U; expected = "Umicom";
    } else if (strcmp(name, "last-byte") == 0) {
        input = value.string_value + sizeof(value.string_value) - 1U; expected = "";
    } else if (strcmp(name, "self-unterminated") == 0) {
        memset(value.string_value, 'x', sizeof(value.string_value));
        input = value.string_value; expected_status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else return 1;
    memcpy(&before, &value, sizeof(before));
    CHECK(umi_ui_value_set_string(&value, input) == expected_status);
    if (expected_status != UMI_STATUS_OK) {
        CHECK(memcmp(&before, &value, sizeof(value)) == 0); return 0;
    }
    CHECK(value.kind == UMI_UI_VALUE_STRING && strcmp(value.string_value, expected) == 0);
    CHECK(value.boolean_value == 0 && value.integer_value == 0 && value.real_value == 0.0);
    for (size_t i = strlen(expected) + 1U; i < sizeof(value.string_value); ++i)
        CHECK(value.string_value[i] == '\0');
    return 0;
}
static int CheckSpecial(const char *name)
{
    UmiUiValue a, b, before;
    int valid, equal;
    if (strcmp(name, "nulls") == 0) {
        CHECK(!umi_ui_value_valid(NULL));
        CHECK(umi_ui_value_set_integer(&a, 9) == UMI_STATUS_OK);
        memcpy(&before, &a, sizeof(a));
        CHECK(umi_ui_value_copy(&a, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&a, &before, sizeof(a)) == 0);
        CHECK(umi_ui_value_copy(NULL, &a) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_ui_value_equal(NULL, &a) && !umi_ui_value_equal(&a, NULL));
        return 0;
    }
    if (strcmp(name, "self-copy") == 0 || strcmp(name, "invalid-self-copy") == 0) {
        CHECK(MakeValue(strcmp(name, "self-copy") == 0 ? "text" : "unterminated", &a, &valid, &equal) == 0);
        memcpy(&before, &a, sizeof(a));
        CHECK(umi_ui_value_copy(&a, &a) == (valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
        CHECK(memcmp(&a, &before, sizeof(a)) == 0); return 0;
    }
    CHECK(umi_ui_value_set_string(&a, "left") == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_string(&b, "right") == UMI_STATUS_OK);
    if (strcmp(name, "different-kind") == 0) CHECK(umi_ui_value_set_integer(&b, 4) == UMI_STATUS_OK);
    else if (strcmp(name, "different-integer") == 0) {
        CHECK(umi_ui_value_set_integer(&a, 1) == UMI_STATUS_OK);
        CHECK(umi_ui_value_set_integer(&b, 2) == UMI_STATUS_OK);
    } else if (strcmp(name, "different-boolean") == 0) {
        CHECK(umi_ui_value_set_boolean(&a, 1) == UMI_STATUS_OK);
        CHECK(umi_ui_value_set_boolean(&b, 0) == UMI_STATUS_OK);
    } else if (strcmp(name, "different-real") == 0) {
        CHECK(umi_ui_value_set_real(&a, 1.5) == UMI_STATUS_OK);
        CHECK(umi_ui_value_set_real(&b, 1.75) == UMI_STATUS_OK);
    } else if (strcmp(name, "different-text") != 0) return 1;
    CHECK(!umi_ui_value_equal(&a, &b) && !umi_ui_value_equal(&b, &a));
    return 0;
}
static int CheckBag(const char *name, const char *record)
{
    UmiUiPropertyBag *bag = NULL;
    UmiUiValue retained, source, observed;
    UmiUiPropertySnapshot before, after;
    int valid, equal;
    CHECK(umi_ui_property_bag_create(&bag) == UMI_STATUS_OK);
    CHECK(umi_ui_value_set_string(&retained, "retained") == UMI_STATUS_OK);
    CHECK(umi_ui_property_bag_set(bag, "existing", &retained) == UMI_STATUS_OK);
    CHECK(umi_ui_property_bag_at(bag, 0U, &before) == UMI_STATUS_OK);
    CHECK(MakeValue(record, &source, &valid, &equal) == 0);
    const uint64_t revision = umi_ui_property_bag_revision(bag);
    const char *key = strcmp(name, "existing") == 0 ? "existing" : "new";
    if (!valid) {
        CHECK(umi_ui_property_bag_set(bag, key, &source) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ui_property_bag_count(bag) == 1U);
        CHECK(umi_ui_property_bag_revision(bag) == revision);
        CHECK(umi_ui_property_bag_at(bag, 0U, &after) == UMI_STATUS_OK);
        CHECK(strcmp(before.key, after.key) == 0 && umi_ui_value_equal(&before.value, &after.value));
        CHECK(umi_ui_property_bag_get(bag, "new", &observed) == UMI_STATUS_NOT_FOUND);
    } else {
        CHECK(umi_ui_property_bag_set(bag, key, &source) == UMI_STATUS_OK);
        CHECK(umi_ui_property_bag_revision(bag) == revision + 1U);
        CHECK(umi_ui_value_set_boolean(&source, 0) == UMI_STATUS_OK);
        CHECK(umi_ui_property_bag_get(bag, key, &observed) == UMI_STATUS_OK);
        if (strcmp(record, "text") == 0) CHECK(strcmp(observed.string_value, "Umicom") == 0);
        else CHECK(observed.kind == UMI_UI_VALUE_INTEGER && observed.integer_value == 41);
    }
    umi_ui_property_bag_destroy(bag);
    return 0;
}
static int CheckReactive(const char *owner, const char *record)
{
    UmiUiValue value;
    int valid, equal;
    CHECK(MakeValue(record, &value, &valid, &equal) == 0);
    /* Archive records also serialize inactive text, so the inactive-text
     * fixture is valid as a UI value but must remain invalid for archives. */
    const int expected = valid && strcmp(record, "inactive-text") != 0;
    unsigned char bytes[64], retained[64];
    size_t written = 77U;
    memset(bytes, 0xa5, sizeof(bytes)); memcpy(retained, bytes, sizeof(bytes));
    if (strcmp(owner, "observable") == 0) {
        UmiUiReactiveObservableValue item;
        umi_ui_reactive_observable_value_init(&item); item.value = value;
        CHECK(umi_ui_reactive_observable_value_valid(&item) == expected);
        if (!expected) {
            CHECK(umi_ui_reactive_observable_value_archive_encode(&item, bytes, sizeof(bytes), &written) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(written == 77U && memcmp(bytes, retained, sizeof(bytes)) == 0);
        }
    } else if (strcmp(owner, "computed") == 0) {
        UmiUiReactiveComputedValue item;
        umi_ui_reactive_computed_value_init(&item); item.value = value;
        CHECK(umi_ui_reactive_computed_value_valid(&item) == expected);
        if (!expected) {
            CHECK(umi_ui_reactive_computed_value_archive_encode(&item, bytes, sizeof(bytes), &written) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(written == 77U && memcmp(bytes, retained, sizeof(bytes)) == 0);
        }
    } else return 1;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc == 3) {
        if (strcmp(argv[1], "string") == 0) return CheckString(argv[2]);
        if (strcmp(argv[1], "special") == 0) return CheckSpecial(argv[2]);
        return CheckRecord(argv[1], argv[2]);
    }
    if (argc == 4 && strcmp(argv[1], "bag") == 0) return CheckBag(argv[2], argv[3]);
    if (argc == 4 && strcmp(argv[1], "reactive") == 0) return CheckReactive(argv[2], argv[3]);
    fprintf(stderr, "Expected a registered value publication case.\n");
    return 2;
}
