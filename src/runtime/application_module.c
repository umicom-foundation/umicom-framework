/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/application_module.c
 * PURPOSE: Keep native application inspection consistent with configure-time discovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/runtime/application_module.h"

#include <stdlib.h>
#include <string.h>
#include "umicom/platform/input_file.h"

/* Use ASCII rules rather than locale-dependent character classes. Identifiers
 * must mean the same thing on Windows, Linux and cross-compilation hosts. */
static int module_alphanumeric(unsigned char value)
{
    return (value >= 'a' && value <= 'z') ||
           (value >= 'A' && value <= 'Z') ||
           (value >= '0' && value <= '9');
}

static int module_identity(const char *value, int display_name)
{
    size_t length = strlen(value);
    if (length == 0U || length >= UMI_MANIFEST_TEXT_CAPACITY ||
        !module_alphanumeric((unsigned char)value[0]) ||
        value[length - 1U] == '.' || value[length - 1U] == ' ')
        return 0;
    for (size_t index = 0U; index < length; ++index) {
        unsigned char ch = (unsigned char)value[index];
        if (module_alphanumeric(ch) || ch == '_' || ch == '-' || ch == '.')
            continue;
        if (display_name && (ch == ' ' || ch == '(' || ch == ')')) continue;
        return 0;
    }
    return 1;
}

/* Copy only direct identity fields. Rejecting a duplicate is preferable to
 * accepting different values in CMake, native tools and the future wizard. */
static UmiStatus module_fields(char *text, UmiApplicationModuleDescriptor *result)
{
    static const char *const names[] = {
        "id", "name", "executable", "native_executable", "console_executable", "component"
    };
    char *outputs[] = {result->id, result->name, result->executable,
        result->native_executable, result->console_executable, result->component};
    unsigned seen = 0U;
    int version_seen = 0;
    unsigned frontends = 0U;
    unsigned sections = 0U;
    int section = 0;
    int schema_seen = 0;
    char *cursor = text;

    while (*cursor != '\0') {
        char *line = cursor;
        char *end = strchr(cursor, '\n');
        char *trimmed;
        if (end != NULL) {
            *end = '\0';
            cursor = end + 1;
        } else {
            cursor += strlen(cursor);
        }
        size_t line_length = strlen(line);
        if (line_length != 0U && line[line_length - 1U] == '\r')
            line[--line_length] = '\0';
        trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') ++trimmed;
        if (*trimmed == '\0' || *trimmed == '#') continue;
        if (!schema_seen) {
            if (strcmp(line, "schema: umicom.application.v1") != 0)
                return UMI_STATUS_PARSE_ERROR;
            schema_seen = 1;
            continue;
        }
        if (strncmp(line, "schema:", 7U) == 0) return UMI_STATUS_PARSE_ERROR;

        /* A new top-level key closes the current block. Unknown sections may
         * contain runtime data but cannot contribute build identity fields. */
        if (*line != ' ' && *line != '\t') {
            section = 0;
            if (strncmp(line, "application:", 12U) == 0) {
                if (strcmp(line, "application:") != 0 || (sections & 1U) != 0U)
                    return UMI_STATUS_PARSE_ERROR;
                sections |= 1U;
                section = 1;
            } else if (strncmp(line, "installation:", 13U) == 0) {
                if (strcmp(line, "installation:") != 0 || (sections & 2U) != 0U)
                    return UMI_STATUS_PARSE_ERROR;
                sections |= 2U;
                section = 2;
            } else if (strncmp(line, "frontends:", 10U) == 0) {
                if (strcmp(line, "frontends:") != 0 || (sections & 4U) != 0U)
                    return UMI_STATUS_PARSE_ERROR;
                sections |= 4U;
                section = 3;
            }
            continue;
        }
        /* Use the same two-space frontend list as configure. An inspection
         * must not approve declarations that discovery would ignore. */
        if (section == 3) {
            if (strcmp(line, "  - console") == 0) frontends |= 1U;
            else if (strcmp(line, "  - gtk") == 0 || strcmp(line, "  - gtk4") == 0)
                frontends |= 2U;
            else if (strcmp(line, "  - web") == 0 || strcmp(line, "  - qt") == 0 ||
                     strcmp(line, "  - qt6") == 0 || strcmp(line, "  - wt") == 0 ||
                     strcmp(line, "  - mobile") == 0)
                frontends |= 4U;
        }
        if (section == 1 && strncmp(trimmed, "version:", 8U) == 0) {
            if (version_seen || trimmed != line + 2 || trimmed[8] != ' ' ||
                !module_identity(trimmed + 9, 0)) return UMI_STATUS_PARSE_ERROR;
            version_seen = 1;
        }
        for (size_t field = 0U; field < 6U; ++field) {
            size_t name_length = strlen(names[field]);
            unsigned flag = 1U << field;
            if ((field == 5U ? section != 2 : section != 1) ||
                strncmp(trimmed, names[field], name_length) != 0 ||
                trimmed[name_length] != ':') continue;
            if (trimmed != line + 2 || trimmed[name_length + 1U] != ' ' ||
                (seen & flag) != 0U) return UMI_STATUS_PARSE_ERROR;
            const char *value = trimmed + name_length + 2U;
            if (!module_identity(value, field == 1U)) return UMI_STATUS_PARSE_ERROR;
            memcpy(outputs[field], value, strlen(value) + 1U);
            seen |= flag;
        }
    }
    if (!schema_seen || !version_seen || (seen & 7U) != 7U) return UMI_STATUS_PARSE_ERROR;
    if (frontends == 0U ||
        (result->native_executable[0] != '\0' && (frontends & 2U) == 0U) ||
        (result->console_executable[0] != '\0' && (frontends & 1U) == 0U))
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

/* Defaults reproduce the existing suite spelling. An explicit installer
 * component lets a product keep a stable installer identity when renamed. */
static UmiStatus module_derive(UmiApplicationModuleDescriptor *result)
{
    const char *basename = result->native_executable[0] != '\0'
        ? result->native_executable : result->executable;
    static const char prefix[] = "UMICOM_APPLICATIONS_BUILD_";
    if (strncmp(basename, "umicom-", 7U) == 0) basename += 7U;
    size_t prefix_length = sizeof(prefix) - 1U;
    size_t length = strlen(basename);
    if (length == 0U || length + prefix_length >= sizeof(result->option))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(result->option, prefix, prefix_length);
    for (size_t index = 0U; index < length; ++index) {
        unsigned char ch = (unsigned char)basename[index];
        if (ch >= 'a' && ch <= 'z') ch = (unsigned char)(ch - 'a' + 'A');
        else if (ch == '-' || ch == '.') ch = '_';
        result->option[prefix_length + index] = (char)ch;
    }
    result->option[prefix_length + length] = '\0';
    if (result->component[0] == '\0') {
        size_t used = 0U;
        for (size_t index = 0U; result->name[index] != '\0'; ++index) {
            if (module_alphanumeric((unsigned char)result->name[index]))
                result->component[used++] = result->name[index];
        }
        result->component[used] = '\0';
    }
    for (size_t index = 0U; result->component[index] != '\0'; ++index) {
        unsigned char ch = (unsigned char)result->component[index];
        if (!module_alphanumeric(ch) && ch != '_' && ch != '-')
            return UMI_STATUS_PARSE_ERROR;
    }
    return result->component[0] == '\0' ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
}

UmiStatus umi_application_module_parse(
    const char *text, size_t length, UmiApplicationModuleDescriptor *output)
{
    UmiApplicationModuleDescriptor candidate = {0};
    UmiApplicationManifest *manifest;
    UmiApplicationLaunchSpec launch;
    UmiStatus status;
    char *copy;
    if (output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(output, 0, sizeof(*output));
    if (text == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_APPLICATION_MODULE_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Embedded NULs must not hide a suffix from one of the two parsers. */
    for (size_t index = 0U; index < length; ++index) {
        unsigned char ch = (unsigned char)text[index];
        if ((ch < 32U && ch != '\n' && ch != '\r' && ch != '\t') || ch == 127U)
            return UMI_STATUS_PARSE_ERROR;
        if (ch == '\r' && (index + 1U == length || text[index + 1U] != '\n'))
            return UMI_STATUS_PARSE_ERROR;
    }
    copy = malloc(length + 1U);
    manifest = calloc(1U, sizeof(*manifest));
    if (copy == NULL || manifest == NULL) {
        free(copy);
        free(manifest);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, text, length);
    copy[length] = '\0';
    /* Reuse runtime validation rather than declaring a separate interpretation
     * of frontends, Framework requirements or executable declarations. */
    status = umi_application_manifest_parse_with_launch_spec(copy, manifest, &launch);
    if (status == UMI_STATUS_OK) status = module_fields(copy, &candidate);
    if (status == UMI_STATUS_OK) status = module_derive(&candidate);
    if (status == UMI_STATUS_OK) *output = candidate;
    free(manifest);
    free(copy);
    return status;
}

UmiStatus umi_application_module_load(
    const char *path, UmiApplicationModuleDescriptor *output)
{
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status;
    if (output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(output, 0, sizeof(*output));
    /* Framework owns UTF-8 Windows paths, regular-file checks and read bounds.
     * The caller supplies an absolute path so background inspection never
     * depends on a later change to the process working directory. */
    status = UmiInputFileRead(path, UMI_APPLICATION_MODULE_MAX_BYTES, &bytes, &length);
    if (status == UMI_STATUS_OK)
        status = umi_application_module_parse((const char *)bytes, length, output);
    UmiInputFileFree(bytes);
    return status;
}


/* Descriptors may cross a plugin or UI boundary after parsing. Check the bounds
 * before calling string helpers: an unterminated caller-owned array must never
 * make validation read into the next field. Re-deriving the option also catches
 * edited executable names whose previous build option was accidentally kept. */
UmiStatus umi_application_module_validate(
    const UmiApplicationModuleDescriptor *module)
{
    if (module == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const char *fields[] = {module->id, module->name, module->executable,
        module->native_executable, module->console_executable, module->component};
    for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        if (memchr(fields[i], '\0', UMI_MANIFEST_TEXT_CAPACITY) == NULL)
            return UMI_STATUS_PARSE_ERROR;
        if ((i == 3U || i == 4U) && fields[i][0] == '\0') continue;
        if (!module_identity(fields[i], i == 1U)) return UMI_STATUS_PARSE_ERROR;
    }
    if (memchr(module->option, '\0', sizeof module->option) == NULL)
        return UMI_STATUS_PARSE_ERROR;
    UmiApplicationModuleDescriptor expected = *module;
    UmiStatus status = module_derive(&expected);
    if (status != UMI_STATUS_OK) return status;
    return strcmp(expected.option, module->option) == 0
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

/* Keep the creator, repository inventory and registration preview on one
 * portable folder contract. A platform accepting a device-like name does not
 * make that name suitable for a shared repository. */
UmiStatus umi_application_module_slug_validate(const char *text)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < 64U && text[length] != '\0') ++length;
    if (length == 0U || length == 64U || text[0] < 'a' || text[0] > 'z' ||
        text[length - 1U] == '-') return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 1U; i < length; ++i)
        if (!((text[i] >= 'a' && text[i] <= 'z') ||
            (text[i] >= '0' && text[i] <= '9') || text[i] == '-'))
            return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(text, "con") == 0 || strcmp(text, "prn") == 0 ||
        strcmp(text, "aux") == 0 || strcmp(text, "nul") == 0 ||
        (length == 4U && (memcmp(text, "com", 3U) == 0 || memcmp(text, "lpt", 3U) == 0) &&
         text[3] >= '1' && text[3] <= '9')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
