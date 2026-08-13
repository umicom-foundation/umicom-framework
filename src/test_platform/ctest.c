/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/ctest.c
 *
 * PURPOSE:
 *   Parse CTest JSON v1 discovery records into portable test-platform data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/ctest.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/process.h"

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static const char *skip_space(const char *cursor)
{
    while (cursor != NULL && *cursor != '\0' &&
           isspace((unsigned char)*cursor)) {
        ++cursor;
    }
    return cursor;
}

static const char *skip_string(const char *cursor)
{
    int escaped = 0;
    if (cursor == NULL || *cursor != '"') return cursor;
    ++cursor;
    while (*cursor != '\0') {
        if (escaped) {
            escaped = 0;
        } else if (*cursor == '\\') {
            escaped = 1;
        } else if (*cursor == '"') {
            return cursor + 1;
        }
        ++cursor;
    }
    return cursor;
}

static const char *skip_value(const char *cursor)
{
    char open;
    char close;
    size_t depth = 0U;
    cursor = skip_space(cursor);
    if (cursor == NULL || *cursor == '\0') return cursor;
    if (*cursor == '"') return skip_string(cursor);
    if (*cursor != '{' && *cursor != '[') {
        while (*cursor != '\0' && *cursor != ',' && *cursor != '}' &&
               *cursor != ']') {
            ++cursor;
        }
        return cursor;
    }
    open = *cursor;
    close = open == '{' ? '}' : ']';
    while (*cursor != '\0') {
        if (*cursor == '"') {
            cursor = skip_string(cursor);
            continue;
        }
        if (*cursor == open) ++depth;
        if (*cursor == close) {
            if (depth == 0U) return cursor;
            --depth;
            if (depth == 0U) return cursor + 1;
        }
        ++cursor;
    }
    return cursor;
}

static UmiStatus parse_string(const char *value,
                              char *out_text,
                              size_t capacity)
{
    size_t used = 0U;
    int escaped = 0;
    value = skip_space(value);
    if (value == NULL || out_text == NULL || capacity == 0U || *value != '"') {
        return UMI_STATUS_PARSE_ERROR;
    }
    ++value;
    while (*value != '\0') {
        char character = *value++;
        if (!escaped && character == '"') {
            out_text[used] = '\0';
            return UMI_STATUS_OK;
        }
        if (!escaped && character == '\\') {
            escaped = 1;
            continue;
        }
        if (escaped) {
            switch (character) {
                case 'n': character = '\n'; break;
                case 'r': character = '\r'; break;
                case 't': character = '\t'; break;
                case 'b': character = '\b'; break;
                case 'f': character = '\f'; break;
                case '"': break;
                case '\\': break;
                case '/': break;
                default: return UMI_STATUS_PARSE_ERROR;
            }
            escaped = 0;
        }
        if (used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out_text[used++] = character;
    }
    return UMI_STATUS_PARSE_ERROR;
}

static const char *find_object_field(const char *object, const char *field)
{
    const char *cursor = skip_space(object);
    if (cursor == NULL || field == NULL || *cursor != '{') return NULL;
    ++cursor;
    for (;;) {
        char key[128];
        cursor = skip_space(cursor);
        if (*cursor == '}') return NULL;
        if (*cursor != '"' ||
            parse_string(cursor, key, sizeof(key)) != UMI_STATUS_OK) {
            return NULL;
        }
        cursor = skip_string(cursor);
        cursor = skip_space(cursor);
        if (*cursor != ':') return NULL;
        cursor = skip_space(cursor + 1);
        if (strcmp(key, field) == 0) return cursor;
        cursor = skip_value(cursor);
        cursor = skip_space(cursor);
        if (*cursor == ',') {
            ++cursor;
            continue;
        }
        if (*cursor == '}') return NULL;
        return NULL;
    }
}

static UmiStatus copy_value(const char *value,
                            char *out_text,
                            size_t capacity)
{
    const char *end;
    size_t length;
    if (value == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    end = skip_value(value);
    if (end == NULL || end <= value) return UMI_STATUS_PARSE_ERROR;
    length = (size_t)(end - value);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_text, value, length);
    out_text[length] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus join_string_array(const char *value,
                                   const char *separator,
                                   char *out_text,
                                   size_t capacity)
{
    const char *cursor = skip_space(value);
    size_t used = 0U;
    if (cursor == NULL || separator == NULL || out_text == NULL ||
        capacity == 0U || *cursor != '[') {
        return UMI_STATUS_PARSE_ERROR;
    }
    out_text[0] = '\0';
    ++cursor;
    for (;;) {
        char element[1024];
        size_t element_length;
        size_t separator_length;
        cursor = skip_space(cursor);
        if (*cursor == ']') return UMI_STATUS_OK;
        if (parse_string(cursor, element, sizeof(element)) != UMI_STATUS_OK) {
            return UMI_STATUS_PARSE_ERROR;
        }
        element_length = strlen(element);
        separator_length = used == 0U ? 0U : strlen(separator);
        if (used + separator_length + element_length + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (separator_length > 0U) {
            (void)memcpy(out_text + used, separator, separator_length);
            used += separator_length;
        }
        (void)memcpy(out_text + used, element, element_length);
        used += element_length;
        out_text[used] = '\0';
        cursor = skip_space(skip_string(cursor));
        if (*cursor == ',') {
            ++cursor;
            continue;
        }
        if (*cursor == ']') return UMI_STATUS_OK;
        return UMI_STATUS_PARSE_ERROR;
    }
}

static const char *find_property_value(const char *test,
                                       const char *property_name)
{
    const char *properties = find_object_field(test, "properties");
    const char *cursor = skip_space(properties);
    if (cursor == NULL || *cursor != '[') return NULL;
    ++cursor;
    for (;;) {
        char name[128];
        const char *end;
        const char *name_value;
        const char *property_value;
        cursor = skip_space(cursor);
        if (*cursor == ']') return NULL;
        if (*cursor != '{') return NULL;
        end = skip_value(cursor);
        name_value = find_object_field(cursor, "name");
        property_value = find_object_field(cursor, "value");
        if (name_value != NULL && property_value != NULL &&
            parse_string(name_value, name, sizeof(name)) == UMI_STATUS_OK &&
            strcmp(name, property_name) == 0) {
            return property_value;
        }
        cursor = skip_space(end);
        if (*cursor == ',') {
            ++cursor;
            continue;
        }
        if (*cursor == ']') return NULL;
        return NULL;
    }
}

static int parse_boolean(const char *value, int default_value)
{
    value = skip_space(value);
    if (value == NULL) return default_value;
    if (strncmp(value, "true", 4U) == 0) return 1;
    if (strncmp(value, "false", 5U) == 0) return 0;
    return default_value;
}

static uint32_t parse_timeout_ms(const char *value)
{
    char *end = NULL;
    double seconds;
    value = skip_space(value);
    if (value == NULL) return 0U;
    seconds = strtod(value, &end);
    if (end == value || seconds <= 0.0) return 0U;
    if (seconds >= 4294967.0) return UINT32_MAX;
    return (uint32_t)(seconds * 1000.0);
}

static UmiStatus import_test_object(
    const char *test,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformItemRegistry *items,
    UmiTestPlatformCtestImportSummary *summary)
{
    UmiTestPlatformItemSnapshot item;
    const char *name_value = find_object_field(test, "name");
    const char *command_value = find_object_field(test, "command");
    const char *labels_value = find_property_value(test, "LABELS");
    const char *disabled_value = find_property_value(test, "DISABLED");
    const char *timeout_value = find_property_value(test, "TIMEOUT");
    const char *working_value =
        find_property_value(test, "WORKING_DIRECTORY");
    char name[256];
    int written;
    if (name_value == NULL ||
        parse_string(name_value, name, sizeof(name)) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    written = snprintf(item.id, sizeof(item.id), "%s.%s",
                       options->suite_id, name);
    if (written < 0 || (size_t)written >= sizeof(item.id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    copy_text(item.parent_id, sizeof(item.parent_id), options->suite_id);
    copy_text(item.suite_id, sizeof(item.suite_id), options->suite_id);
    copy_text(item.name, sizeof(item.name), name);
    copy_text(item.uri, sizeof(item.uri), options->build_directory);
    copy_text(item.framework, sizeof(item.framework), "ctest");
    copy_text(item.kind, sizeof(item.kind), "test");
    item.enabled = !parse_boolean(disabled_value, 0);
    item.discovered = 1;
    item.timeout_ms = parse_timeout_ms(timeout_value);
    if (command_value != NULL) {
        UmiStatus status = join_string_array(command_value, " ", item.command,
                                             sizeof(item.command));
        if (status != UMI_STATUS_OK) return status;
    }
    if (labels_value != NULL && *skip_space(labels_value) == '[') {
        UmiStatus status = join_string_array(labels_value, ";", item.labels,
                                             sizeof(item.labels));
        if (status != UMI_STATUS_OK) return status;
    }
    if (working_value != NULL) {
        UmiStatus status = parse_string(working_value, item.working_directory,
                                        sizeof(item.working_directory));
        if (status != UMI_STATUS_OK) return status;
    } else {
        copy_text(item.working_directory, sizeof(item.working_directory),
                  options->build_directory);
    }
    if (!item.enabled) summary->disabled_count += 1U;
    if (item.labels[0] != '\0') summary->labelled_count += 1U;
    if (item.timeout_ms > 0U) summary->timed_count += 1U;
    summary->discovered_count += 1U;
    return umi_test_platform_item_registry_upsert(items, &item);
}

UmiStatus umi_test_platform_ctest_parse_json_v1(
    const char *json,
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformItemRegistry *items,
    UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *out_summary)
{
    UmiTestPlatformCtestImportSummary summary;
    UmiTestPlatformItemSnapshot root;
    UmiTestPlatformSuiteSnapshot suite;
    UmiTestPlatformDiscoverySnapshot discovery;
    const char *tests_value;
    const char *cursor;
    UmiStatus status;
    if (json == NULL || options == NULL || items == NULL || suites == NULL ||
        discoveries == NULL || options->suite_id[0] == '\0' ||
        options->project_id[0] == '\0' ||
        options->build_directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    tests_value = find_object_field(json, "tests");
    cursor = skip_space(tests_value);
    if (cursor == NULL || *cursor != '[') return UMI_STATUS_PARSE_ERROR;
    (void)memset(&summary, 0, sizeof(summary));
    (void)memset(&root, 0, sizeof(root));
    root.struct_size = (uint32_t)sizeof(root);
    root.api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    copy_text(root.id, sizeof(root.id), options->suite_id);
    copy_text(root.suite_id, sizeof(root.suite_id), options->suite_id);
    copy_text(root.name, sizeof(root.name), "CTest");
    copy_text(root.uri, sizeof(root.uri), options->build_directory);
    copy_text(root.framework, sizeof(root.framework), "ctest");
    copy_text(root.kind, sizeof(root.kind), "suite");
    root.enabled = 1;
    root.discovered = 1;
    status = umi_test_platform_item_registry_upsert(items, &root);
    if (status != UMI_STATUS_OK) return status;
    ++cursor;
    for (;;) {
        char test_object[16384];
        cursor = skip_space(cursor);
        if (*cursor == ']') break;
        if (*cursor != '{') return UMI_STATUS_PARSE_ERROR;
        status = copy_value(cursor, test_object, sizeof(test_object));
        if (status != UMI_STATUS_OK) return status;
        status = import_test_object(test_object, options, items, &summary);
        if (status != UMI_STATUS_OK) return status;
        cursor = skip_space(skip_value(cursor));
        if (*cursor == ',') {
            ++cursor;
            continue;
        }
        if (*cursor == ']') break;
        return UMI_STATUS_PARSE_ERROR;
    }
    (void)memset(&suite, 0, sizeof(suite));
    suite.struct_size = (uint32_t)sizeof(suite);
    suite.api_version = UMI_TEST_PLATFORM_SUITE_API_VERSION;
    copy_text(suite.id, sizeof(suite.id), options->suite_id);
    copy_text(suite.name, sizeof(suite.name), "CTest");
    copy_text(suite.project_id, sizeof(suite.project_id), options->project_id);
    copy_text(suite.root_item_id, sizeof(suite.root_item_id),
              options->suite_id);
    suite.test_count = summary.discovered_count;
    suite.enabled = 1;
    status = umi_test_platform_suite_registry_upsert(suites, &suite);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&discovery, 0, sizeof(discovery));
    discovery.struct_size = (uint32_t)sizeof(discovery);
    discovery.api_version = UMI_TEST_PLATFORM_DISCOVERY_API_VERSION;
    if (snprintf(discovery.id, sizeof(discovery.id), "discovery.%s",
                 options->suite_id) < 0) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    copy_text(discovery.project_id, sizeof(discovery.project_id),
              options->project_id);
    copy_text(discovery.provider, sizeof(discovery.provider), "ctest-json-v1");
    copy_text(discovery.root_uri, sizeof(discovery.root_uri),
              options->build_directory);
    copy_text(discovery.configuration, sizeof(discovery.configuration),
              options->configuration);
    discovery.discovered_count = summary.discovered_count;
    discovery.state = 1;
    discovery.automatic = 0;
    discovery.metadata_supported = 1;
    status = umi_test_platform_discovery_registry_upsert(discoveries,
                                                        &discovery);
    if (status != UMI_STATUS_OK) return status;
    if (out_summary != NULL) *out_summary = summary;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_ctest_discover(
    const UmiTestPlatformCtestImportOptions *options,
    UmiTestPlatformItemRegistry *items,
    UmiTestPlatformSuiteRegistry *suites,
    UmiTestPlatformDiscoveryRegistry *discoveries,
    UmiTestPlatformCtestImportSummary *out_summary,
    char *out_diagnostics,
    size_t diagnostics_capacity)
{
    const char *arguments[6];
    UmiProcessRequest request;
    UmiProcessResult result;
    UmiStatus status;
    size_t diagnostics_length;
    if (options == NULL || items == NULL || suites == NULL ||
        discoveries == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    arguments[0] = "--test-dir";
    arguments[1] = options->build_directory;
    arguments[2] = "--show-only=json-v1";
    arguments[3] = "-C";
    arguments[4] = options->configuration[0] != '\0'
                       ? options->configuration
                       : "Debug";
    arguments[5] = "--no-tests=error";
    (void)memset(&request, 0, sizeof(request));
    request.program = "ctest";
    request.arguments = arguments;
    request.argument_count = 6U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    status = umi_process_execute(&request, &result);
    if (out_diagnostics != NULL && diagnostics_capacity > 0U) {
        diagnostics_length = strlen(result.output);
        if (diagnostics_length >= diagnostics_capacity) {
            diagnostics_length = diagnostics_capacity - 1U;
        }
        if (diagnostics_length > 0U) {
            (void)memcpy(out_diagnostics, result.output, diagnostics_length);
        }
        out_diagnostics[diagnostics_length] = '\0';
    }
    if (status != UMI_STATUS_OK || result.exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INTERNAL_ERROR;
    }
    return umi_test_platform_ctest_parse_json_v1(
        result.output, options, items, suites, discoveries, out_summary);
}
