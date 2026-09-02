/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/reflection/test_registry.c
 *
 * PURPOSE:
 *   Verify owned reflection metadata, field-bound validation, stable snapshots
 *   and bounded catalogue queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "umicom/reflection/reflection.h"

typedef struct ReflectionTestRecord {
    uint32_t identifier;
    char label[32];
} ReflectionTestRecord;

static const UmiReflectionFieldDescriptor test_fields[] = {
    UMI_REFLECTION_FIELD(
        ReflectionTestRecord,
        identifier,
        "c.uint32",
        UMI_REFLECTION_VALUE_UNSIGNED_INTEGER,
        UMI_REFLECTION_FIELD_REQUIRED,
        "Stable record identifier."),
    UMI_REFLECTION_ARRAY_FIELD(
        ReflectionTestRecord,
        label,
        "c.char",
        UMI_REFLECTION_VALUE_TEXT,
        UMI_REFLECTION_FIELD_REQUIRED,
        "Human-readable record label.")
};

/* Stop at the first failed expectation and identify its source line. */
static int require_true(int condition, int line)
{
    if (!condition) {
        (void)fprintf(stderr, "reflection registry failure at line %d\n", line);
        return 0;
    }
    return 1;
}

#define REQUIRE(CONDITION)                                                   \
    do {                                                                     \
        if (!require_true((CONDITION), __LINE__)) {                          \
            umi_reflection_registry_destroy(registry);                      \
            return 1;                                                        \
        }                                                                    \
    } while (0)

int main(void)
{
    UmiReflectionRegistry *registry = NULL;
    UmiReflectionTypeDescriptor type;
    UmiReflectionTypeSnapshot snapshot;
    UmiReflectionQuery query;
    UmiReflectionQueryResult result;
    UmiReflectionFieldDescriptor invalid_field;

    REQUIRE(umi_reflection_registry_create(NULL, &registry) == UMI_STATUS_OK);
    (void)memset(&type, 0, sizeof(type));
    type.structure_size = (uint32_t)sizeof(type);
    type.schema_version = 1U;
    type.kind = UMI_REFLECTION_TYPE_STRUCT;
    type.size = sizeof(ReflectionTestRecord);
    type.alignment = _Alignof(ReflectionTestRecord);
    type.fields = test_fields;
    type.field_count = UMI_REFLECTION_ARRAY_COUNT(test_fields);
    (void)snprintf(type.type_id, sizeof(type.type_id), "%s", "test.record");
    (void)snprintf(type.display_name,
                   sizeof(type.display_name),
                   "%s",
                   "Reflection Test Record");
    (void)snprintf(type.module_id,
                   sizeof(type.module_id),
                   "%s",
                   "org.umicom.test");
    (void)snprintf(type.description,
                   sizeof(type.description),
                   "%s",
                   "Small type used to verify owned reflection metadata.");

    REQUIRE(umi_reflection_registry_register_type(registry, &type) ==
            UMI_STATUS_OK);
    REQUIRE(umi_reflection_registry_type_count(registry) == 1U);
    REQUIRE(umi_reflection_registry_get_type(
                registry, "test.record", &snapshot) == UMI_STATUS_OK);
    REQUIRE(snapshot.descriptor.fields == snapshot.fields);
    REQUIRE(snapshot.descriptor.field_count == 2U);
    REQUIRE(snapshot.fields[1].element_count == 32U);

    query = umi_reflection_query_default();
    (void)snprintf(query.module_id,
                   sizeof(query.module_id),
                   "%s",
                   "org.umicom.test");
    (void)snprintf(query.text, sizeof(query.text), "%s", "record");
    REQUIRE(umi_reflection_registry_query_types(registry, &query, &result) ==
            UMI_STATUS_OK);
    REQUIRE(result.count == 1U);
    REQUIRE(strcmp(result.ids[0], "test.record") == 0);

    invalid_field = test_fields[0];
    invalid_field.offset = sizeof(ReflectionTestRecord);
    type.fields = &invalid_field;
    type.field_count = 1U;
    (void)snprintf(type.type_id, sizeof(type.type_id), "%s", "test.invalid");
    REQUIRE(umi_reflection_registry_register_type(registry, &type) ==
            UMI_STATUS_INVALID_ARGUMENT);

    REQUIRE(umi_reflection_registry_unregister_type(
                registry, "test.record") == UMI_STATUS_OK);
    REQUIRE(umi_reflection_registry_type_count(registry) == 0U);
    umi_reflection_registry_destroy(registry);
    return 0;
}
