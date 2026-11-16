/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_numeric_input.c
 *
 * PURPOSE:
 *   Verify exact numeric input observations, opaque-schema preservation and unchanged failure reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_numeric_input.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto cleanup; \
} } while (0)
typedef struct Fixture
{
    const char *name, *schema;
    UmiStatus status;
    int kind;
    unsigned known;
    int64_t integers[4];
    double decimals[4];
    bool inconsistent;
    unsigned known_flags, flags;
    size_t unknown;
} Fixture;
static const Fixture fixtures[] = {
    {"integer-complete", "[\"INT\",{\"default\":9007199254740993,\"min\":-9223372036854775808,\"max\":9223372036854775807,\"step\":1}]", UMI_STATUS_OK, 1, 15U, {INT64_C(9007199254740993), INT64_MIN, INT64_MAX, 1}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"decimal-complete", "[\"FLOAT\",{\"default\":7.5,\"min\":0,\"max\":30.0,\"step\":0.5,\"forceInput\":true}]", UMI_STATUS_OK, 2, 15U, {0, 0, 0, 0}, {7.5, 0.0, 30.0, 0.5}, false, 1U, 1U, 0U},
    {"integer-minimal", "[\"INT\"]", UMI_STATUS_OK, 1, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"decimal-minimal", "[\"FLOAT\",{}]", UMI_STATUS_OK, 2, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"known-zero", "[\"FLOAT\",{\"default\":0}]", UMI_STATUS_OK, 2, 1U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"negative-zero", "[\"FLOAT\",{\"default\":-0.0}]", UMI_STATUS_OK, 2, 1U, {0, 0, 0, 0}, {-0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"exponent", "[\"FLOAT\",{\"default\":1.25e2,\"min\":1e-2}]", UMI_STATUS_OK, 2, 3U, {0, 0, 0, 0}, {125.0, 0.01, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"missing-default", "[\"INT\",{\"min\":0,\"max\":100}]", UMI_STATUS_OK, 1, 6U, {0, 0, 100, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"integer-adjacent", "[\"INT\",{\"default\":9007199254740993,\"min\":9007199254740992,\"max\":9007199254740993}]", UMI_STATUS_OK, 1, 7U, {INT64_C(9007199254740993), INT64_C(9007199254740992), INT64_C(9007199254740993), 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"integer-below", "[\"INT\",{\"default\":-1,\"min\":0}]", UMI_STATUS_OK, 1, 3U, {-1, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, true, 0U, 0U, 0U},
    {"integer-above", "[\"INT\",{\"default\":5,\"max\":4}]", UMI_STATUS_OK, 1, 5U, {5, 0, 4, 0}, {0.0, 0.0, 0.0, 0.0}, true, 0U, 0U, 0U},
    {"integer-reversed", "[\"INT\",{\"min\":2,\"max\":1}]", UMI_STATUS_OK, 1, 6U, {0, 2, 1, 0}, {0.0, 0.0, 0.0, 0.0}, true, 0U, 0U, 0U},
    {"integer-zero-step", "[\"INT\",{\"step\":0}]", UMI_STATUS_OK, 1, 8U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, true, 0U, 0U, 0U},
    {"integer-negative-step", "[\"INT\",{\"step\":-2}]", UMI_STATUS_OK, 1, 8U, {0, 0, 0, -2}, {0.0, 0.0, 0.0, 0.0}, true, 0U, 0U, 0U},
    {"decimal-below", "[\"FLOAT\",{\"default\":0.5,\"min\":1}]", UMI_STATUS_OK, 2, 3U, {0, 0, 0, 0}, {0.5, 1.0, 0.0, 0.0}, true, 0U, 0U, 0U},
    {"decimal-above", "[\"FLOAT\",{\"default\":5.5,\"max\":5}]", UMI_STATUS_OK, 2, 5U, {0, 0, 0, 0}, {5.5, 0.0, 5.0, 0.0}, true, 0U, 0U, 0U},
    {"decimal-reversed", "[\"FLOAT\",{\"min\":2,\"max\":1}]", UMI_STATUS_OK, 2, 6U, {0, 0, 0, 0}, {0.0, 2.0, 1.0, 0.0}, true, 0U, 0U, 0U},
    {"decimal-zero-step", "[\"FLOAT\",{\"step\":-0.0}]", UMI_STATUS_OK, 2, 8U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, true, 0U, 0U, 0U},
    {"decimal-negative-step", "[\"FLOAT\",{\"step\":-0.5}]", UMI_STATUS_OK, 2, 8U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, -0.5}, true, 0U, 0U, 0U},
    {"force-false", "[\"INT\",{\"forceInput\":false}]", UMI_STATUS_OK, 1, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 1U, 0U, 0U},
    {"unknown-options", "[\"INT\",{\"default\":2,\"display\":\"slider\",\"round\":0.01,\"custom\":{\"nested\":true}}]", UMI_STATUS_OK, 1, 1U, {2, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 3U},
    {"optional-group", "[\"INT\",{\"default\":9}]", UMI_STATUS_OK, 1, 1U, {9, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"report-lifetime", "[\"FLOAT\",{\"default\":7.5}]", UMI_STATUS_OK, 2, 1U, {0, 0, 0, 0}, {7.5, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"integer-fraction", "[\"INT\",{\"default\":1.0}]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"integer-exponent", "[\"INT\",{\"step\":1e1}]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"numeric-null", "[\"FLOAT\",{\"min\":null}]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"numeric-boolean", "[\"FLOAT\",{\"max\":true}]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"numeric-string", "[\"FLOAT\",{\"default\":\"7.5\"}]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"flag-number", "[\"INT\",{\"forceInput\":1}]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"options-array", "[\"FLOAT\",[]]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"empty-array", "[]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"type-null", "[null]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"type-number", "[4]", UMI_STATUS_PARSE_ERROR, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"integer-overflow", "[\"INT\",{\"max\":18446744073709551615}]", UMI_STATUS_CAPACITY_EXCEEDED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"decimal-overflow", "[\"FLOAT\",{\"default\":1e9999}]", UMI_STATUS_CAPACITY_EXCEEDED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"decimal-underflow", "[\"FLOAT\",{\"step\":1e-9999}]", UMI_STATUS_CAPACITY_EXCEEDED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"string-type", "[\"STRING\",{\"default\":\"a\"}]", UMI_STATUS_NOT_IMPLEMENTED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"boolean-type", "[\"BOOLEAN\",{\"default\":true}]", UMI_STATUS_NOT_IMPLEMENTED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"tensor-type", "[\"IMAGE\",{\"forceInput\":true}]", UMI_STATUS_NOT_IMPLEMENTED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"choice-type", "[[\"a\",\"b\"]]", UMI_STATUS_NOT_IMPLEMENTED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"object-schema", "{\"custom\":true}", UMI_STATUS_NOT_IMPLEMENTED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U},
    {"extra-elements", "[\"INT\",{},7]", UMI_STATUS_NOT_IMPLEMENTED, 0, 0U, {0, 0, 0, 0}, {0.0, 0.0, 0.0, 0.0}, false, 0U, 0U, 0U}
};
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection connection = {0};
    strcpy(connection.id, "comfy-numeric-fixture");
    strcpy(connection.provider_id, "comfyui");
    strcpy(connection.label, "Numeric observation fixture");
    strcpy(connection.endpoint, "http://127.0.0.1:8188");
    connection.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    connection.timeout_ms = 30000;
    connection.enabled = true;
    return connection;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    int failed = 0;
    const char *mode = argv[1];
    const Fixture *fixture = NULL;
    for (size_t i = 0U; i < sizeof(fixtures) / sizeof(fixtures[0]); ++i)
        if (strcmp(fixtures[i].name, mode) == 0)
            fixture = &fixtures[i];
    bool special = fixture == NULL;
    const char *schema = fixture ? fixture->schema : "[\"INT\",{\"default\":7}]";
    const char *group_name = strcmp(mode, "optional-group") == 0 ? "optional" : "required";
    UmiComfyInputGroup group = strcmp(group_name, "optional") == 0 ?
        UMI_COMFY_INPUT_OPTIONAL : UMI_COMFY_INPUT_REQUIRED;
    UmiComfyNodeCatalogue *catalogue = NULL;
    char json[4096];
    UmiProviderConnection connection = Connection();
    int written = snprintf(json, sizeof(json),
        "{\"N\":{\"input\":{\"%s\":{\"first\":[\"STRING\"],\"setting\":%s}},\"output\":[]}}",
        group_name, schema);
    CHECK(written > 0 && (size_t)written < sizeof(json));
    CHECK(UmiComfyNodeCatalogueCreate(&connection, 17U, json, (size_t)written, NULL,
        &catalogue) == UMI_STATUS_OK);
    UmiComfyNodeInput raw = {0};
    CHECK(UmiComfyNodeCatalogueInputAt(catalogue, 0U, group, 1U, &raw) == UMI_STATUS_OK);
    CHECK(strcmp(raw.name, "setting") == 0 && raw.schema_bytes == strlen(schema) &&
        memcmp(raw.schema_json, schema, raw.schema_bytes) == 0);
    UmiComfyNodeCatalogueReview before_review = {0}, after_review = {0};
    CHECK(UmiComfyNodeCatalogueInspect(catalogue, &before_review) == UMI_STATUS_OK);
    UmiComfyNumericInput output, before;
    memset(&output, 0xA5, sizeof(output));
    memcpy(&before, &output, sizeof(before));
    UmiStatus status;
    if (!special)
    {
        status = UmiComfyNodeCatalogueNumericInput(catalogue, 0U, group, 1U, &output);
        CHECK(status == fixture->status);
        if (status == UMI_STATUS_OK)
        {
            CHECK(strcmp(output.name, "setting") == 0 && (int)output.kind == fixture->kind);
            CHECK(output.known_values == fixture->known &&
                output.known_flags == fixture->known_flags && output.enabled_flags == fixture->flags);
            CHECK(output.inconsistent_values == fixture->inconsistent &&
                output.uninterpreted_options == fixture->unknown);
            for (size_t i = 0U; i < 4U; ++i)
                if ((fixture->known & (1U << i)) != 0U)
                {
                    if (fixture->kind == 1)
                        CHECK(output.values.integers[i] == fixture->integers[i]);
                    else
                        CHECK(output.values.decimals[i] == fixture->decimals[i]);
                }
            if (strcmp(mode, "negative-zero") == 0)
                CHECK(copysign(1.0, output.values.decimals[0]) < 0.0);
        }
        else
            CHECK(memcmp(&output, &before, sizeof(output)) == 0);
    }
    else
    {
        if (strcmp(mode, "null-catalogue") == 0)
            status = UmiComfyNodeCatalogueNumericInput(NULL, 0U, group, 1U, &output);
        else if (strcmp(mode, "null-output") == 0)
            status = UmiComfyNodeCatalogueNumericInput(catalogue, 0U, group, 1U, NULL);
        else if (strcmp(mode, "invalid-group") == 0)
            status = UmiComfyNodeCatalogueNumericInput(catalogue, 0U, (UmiComfyInputGroup)0, 1U, &output);
        else if (strcmp(mode, "missing-node") == 0)
            status = UmiComfyNodeCatalogueNumericInput(catalogue, 1U, group, 1U, &output);
        else if (strcmp(mode, "missing-input") == 0)
            status = UmiComfyNodeCatalogueNumericInput(catalogue, 0U, group, 2U, &output);
        else if (strcmp(mode, "missing-group") == 0)
            status = UmiComfyNodeCatalogueNumericInput(catalogue, 0U, UMI_COMFY_INPUT_OPTIONAL, 0U, &output);
        else
            CHECK(0);
        UmiStatus expected = strncmp(mode, "missing-", 8U) == 0 ?
            UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_ARGUMENT;
        CHECK(status == expected && memcmp(&output, &before, sizeof(output)) == 0);
    }
    CHECK(UmiComfyNodeCatalogueCount(catalogue) == 1U);
    CHECK(UmiComfyNodeCatalogueInputAt(catalogue, 0U, group, 1U, &raw) == UMI_STATUS_OK);
    CHECK(raw.schema_bytes == strlen(schema) && memcmp(raw.schema_json, schema, raw.schema_bytes) == 0);
    CHECK(UmiComfyNodeCatalogueInspect(catalogue, &after_review) == UMI_STATUS_OK);
    CHECK(before_review.connection_revision == after_review.connection_revision);
    CHECK(memcmp(before_review.connection_sha256, after_review.connection_sha256, UMI_SHA256_BYTES) == 0);
    CHECK(memcmp(before_review.response_sha256, after_review.response_sha256, UMI_SHA256_BYTES) == 0);
    if (strcmp(mode, "report-lifetime") == 0)
    {
        UmiComfyNodeCatalogueDestroy(catalogue);
        catalogue = NULL;
        CHECK(strcmp(output.name, "setting") == 0 && output.values.decimals[0] == 7.5);
    }
cleanup:
    UmiComfyNodeCatalogueDestroy(catalogue);
    return failed;
}
