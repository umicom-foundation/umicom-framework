/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/configuration_library.c
 * PURPOSE: Publish complete named settings and their catalogue in one local transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/configuration_library.h"
#include "profile_store_internal.h"
#include "umicom/platform/path.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus UmiBuildConfigurationNameValidate(const char *name)
{
    if (name == NULL || name[0] == '\0' || name[0] == ' ')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_BUILD_CONFIGURATION_NAME_CAPACITY && name[length] != '\0')
    {
        unsigned char byte = (unsigned char)name[length];
        if (!((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') ||
              (byte >= '0' && byte <= '9') || byte == ' ' || byte == '.' || byte == '-' ||
              byte == '_'))
            return UMI_STATUS_INVALID_ARGUMENT;
        ++length;
    }
    if (length == UMI_BUILD_CONFIGURATION_NAME_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return name[length - 1U] == ' ' ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
static UmiStatus ConfigurationIdentity(const char *source, char *root, char *prefix)
{
    if (source == NULL || !umi_path_is_absolute(source))
        return UMI_STATUS_INVALID_ARGUMENT;
    char base[128];
    UmiStatus status = UmiBuildProfileStorageIdentity(source, root, base);
    if (status != UMI_STATUS_OK)
        return status;
    int length = snprintf(prefix, 128U, "%s.configurations", base);
    return length < 0 || length >= 128 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
static UmiStatus ConfigurationKey(const char *prefix, const char *field, char *key)
{
    int length = snprintf(key, 192U, "%s.%s", prefix, field);
    return length < 0 || length >= 192 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
static UmiStatus ConfigurationGet(UmiDataServer *server, const char *prefix, const char *field,
                                  char *out, size_t capacity)
{
    char key[192];
    UmiStatus status = ConfigurationKey(prefix, field, key);
    return status == UMI_STATUS_OK ? umi_data_server_get(server, key, out, capacity) : status;
}
static UmiStatus ConfigurationSet(UmiDataServer *server, const char *prefix, const char *field,
                                  const char *value)
{
    char key[192];
    UmiStatus status = ConfigurationKey(prefix, field, key);
    return status == UMI_STATUS_OK ? umi_data_server_set(server, key, value) : status;
}
static UmiStatus ConfigurationSlot(const char *prefix, size_t slot, char *out)
{
    int length = snprintf(out, 128U, "%s.entry.%zu", prefix, slot);
    return length < 0 || length >= 128 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
/* Decimal metadata has one interpretation across libc implementations. Reject
 * signs, whitespace, overflow and empty values before publishing a catalogue. */
static UmiStatus ConfigurationNumber(UmiDataServer *server, const char *prefix, const char *field,
                                     uint64_t *out)
{
    char text[32];
    UmiStatus status = ConfigurationGet(server, prefix, field, text, sizeof text);
    if (status != UMI_STATUS_OK)
        return status;
    if (text[0] == '\0')
        return UMI_STATUS_PARSE_ERROR;
    uint64_t value = 0U;
    for (size_t index = 0U; text[index] != '\0'; ++index)
    {
        if (text[index] < '0' || text[index] > '9')
            return UMI_STATUS_PARSE_ERROR;
        unsigned digit = (unsigned)(text[index] - '0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10))
            return UMI_STATUS_PARSE_ERROR;
        value = value * UINT64_C(10) + digit;
    }
    *out = value;
    return UMI_STATUS_OK;
}
typedef struct ConfigurationPresence
{
    const char *prefix;
    bool found;
} ConfigurationPresence;
static UmiStatus ConfigurationVisit(const char *key, const char *value, void *context)
{
    (void)value;
    ConfigurationPresence *presence = context;
    size_t length = strlen(presence->prefix);
    if (strncmp(key, presence->prefix, length) == 0 && (key[length] == '.' || key[length] == '\0'))
        presence->found = true;
    return UMI_STATUS_OK;
}
static UmiStatus ConfigurationReadCatalogue(UmiDataServer *server, const char *prefix,
                                            const char *root, UmiBuildConfigurationCatalogue *out)
{
    char schema[32], stored_root[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = ConfigurationGet(server, prefix, "schema", schema, sizeof schema);
    if (status == UMI_STATUS_NOT_FOUND)
    {
        /* A missing marker with leftover fields is damaged storage, not a new
         * library. Never conceal those fields by creating a fresh catalogue. */
        ConfigurationPresence presence = {prefix, false};
        status = umi_data_server_visit(server, ConfigurationVisit, &presence);
        if (status != UMI_STATUS_OK)
            return status;
        if (presence.found)
            return UMI_STATUS_PARSE_ERROR;
        memset(out, 0, sizeof *out);
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK)
        return status;
    if (strcmp(schema, "1") != 0)
        return UMI_STATUS_INVALID_STATE;
    UmiBuildConfigurationCatalogue candidate = {0};
    uint64_t count = 0U;
    status = ConfigurationGet(server, prefix, "source", stored_root, sizeof stored_root);
    if (status == UMI_STATUS_OK &&
        (!umi_path_is_absolute(stored_root) || !umi_path_equal(stored_root, root)))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = ConfigurationNumber(server, prefix, "revision", &candidate.revision);
    if (status == UMI_STATUS_OK)
        status = ConfigurationNumber(server, prefix, "count", &count);
    if (status == UMI_STATUS_OK &&
        (candidate.revision == 0U || count > UMI_BUILD_CONFIGURATION_CAPACITY))
        status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK)
        return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
    candidate.count = (size_t)count;
    for (size_t slot = 0U; slot < UMI_BUILD_CONFIGURATION_CAPACITY; ++slot)
    {
        char entry[128], name[UMI_BUILD_CONFIGURATION_NAME_CAPACITY];
        status = ConfigurationSlot(prefix, slot, entry);
        if (status == UMI_STATUS_OK)
            status = ConfigurationGet(server, entry, "name", name, sizeof name);
        if (slot >= candidate.count)
        {
            if (status != UMI_STATUS_NOT_FOUND)
                return status == UMI_STATUS_OK ? UMI_STATUS_PARSE_ERROR : status;
            continue;
        }
        if (status != UMI_STATUS_OK)
            return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
        if (UmiBuildConfigurationNameValidate(name) != UMI_STATUS_OK)
            return UMI_STATUS_PARSE_ERROR;
        for (size_t previous = 0U; previous < slot; ++previous)
            if (strcmp(candidate.names[previous], name) == 0)
                return UMI_STATUS_PARSE_ERROR;
        strcpy(candidate.names[slot], name);
    }
    *out = candidate;
    return UMI_STATUS_OK;
}
static UmiStatus ConfigurationFinishRead(UmiDataServer *server, UmiStatus status)
{
    UmiStatus finish = umi_data_server_rollback(server);
    return finish == UMI_STATUS_OK ? status : finish;
}
UmiStatus UmiBuildConfigurationCapture(UmiDataServer *server, const char *source,
                                       UmiBuildConfigurationCatalogue *out)
{
    if (server == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    char root[UMI_BUILD_PATH_CAPACITY], prefix[128];
    UmiStatus status = ConfigurationIdentity(source, root, prefix);
    if (status != UMI_STATUS_OK)
        return status;
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK)
        return status;
    UmiBuildConfigurationCatalogue candidate;
    status = ConfigurationReadCatalogue(server, prefix, root, &candidate);
    status = ConfigurationFinishRead(server, status);
    if (status == UMI_STATUS_OK)
        *out = candidate;
    return status;
}
static size_t ConfigurationFind(const UmiBuildConfigurationCatalogue *catalogue, const char *name)
{
    for (size_t slot = 0U; slot < catalogue->count; ++slot)
        if (strcmp(catalogue->names[slot], name) == 0)
            return slot;
    return catalogue->count;
}
UmiStatus UmiBuildConfigurationLoad(UmiDataServer *server, const char *source, const char *name,
                                    uint64_t expected_revision, UmiBuildProfile *out)
{
    if (server == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiBuildConfigurationNameValidate(name);
    char root[UMI_BUILD_PATH_CAPACITY], prefix[128], entry[128];
    if (status == UMI_STATUS_OK)
        status = ConfigurationIdentity(source, root, prefix);
    if (status != UMI_STATUS_OK)
        return status;
    UmiBuildProfile *candidate = malloc(sizeof *candidate);
    if (candidate == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK)
    {
        free(candidate);
        return status;
    }
    UmiBuildConfigurationCatalogue catalogue;
    status = ConfigurationReadCatalogue(server, prefix, root, &catalogue);
    if (status == UMI_STATUS_OK && catalogue.revision != expected_revision)
        status = UMI_STATUS_INVALID_STATE;
    size_t slot = status == UMI_STATUS_OK ? ConfigurationFind(&catalogue, name) : 0U;
    if (status == UMI_STATUS_OK && slot == catalogue.count)
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        status = ConfigurationSlot(prefix, slot, entry);
    uint64_t profile_revision = 0U;
    if (status == UMI_STATUS_OK)
    {
        status = UmiBuildProfileStorageRead(server, entry, root, candidate, &profile_revision);
        if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK && profile_revision > catalogue.revision)
        status = UMI_STATUS_PARSE_ERROR;
    status = ConfigurationFinishRead(server, status);
    if (status == UMI_STATUS_OK)
        *out = *candidate;
    free(candidate);
    return status;
}
UmiStatus UmiBuildConfigurationSave(UmiDataServer *server, const char *name,
                                    const UmiBuildProfile *profile, uint64_t expected_revision,
                                    uint64_t *out_revision)
{
    if (server == NULL || out_revision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiBuildConfigurationNameValidate(name);
    if (status == UMI_STATUS_OK)
        status = umi_build_profile_validate(profile, NULL, 0U);
    char root[UMI_BUILD_PATH_CAPACITY], prefix[128], entry[128], number[32];
    if (status == UMI_STATUS_OK)
        status = ConfigurationIdentity(profile->source_directory, root, prefix);
    if (status != UMI_STATUS_OK)
        return status;
    UmiBuildProfile *candidate = malloc(sizeof *candidate), *previous = malloc(sizeof *previous);
    if (candidate == NULL || previous == NULL)
    {
        free(candidate);
        free(previous);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *candidate = *profile;
    strcpy(candidate->source_directory, root);
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK)
    {
        free(candidate);
        free(previous);
        return status;
    }
    UmiBuildConfigurationCatalogue catalogue;
    status = ConfigurationReadCatalogue(server, prefix, root, &catalogue);
    if (status == UMI_STATUS_OK && catalogue.revision != expected_revision)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && catalogue.revision == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    size_t slot = status == UMI_STATUS_OK ? ConfigurationFind(&catalogue, name) : 0U;
    if (status == UMI_STATUS_OK && slot == UMI_BUILD_CONFIGURATION_CAPACITY)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        status = ConfigurationSlot(prefix, slot, entry);
    uint64_t previous_revision = 0U;
    if (status == UMI_STATUS_OK)
    {
        status = UmiBuildProfileStorageRead(server, entry, root, previous, &previous_revision);
        if (slot == catalogue.count)
            status = status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK
                     : status == UMI_STATUS_OK      ? UMI_STATUS_PARSE_ERROR
                                                    : status;
        else if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_PARSE_ERROR;
        else if (status == UMI_STATUS_OK && previous_revision > catalogue.revision)
            status = UMI_STATUS_PARSE_ERROR;
    }
    uint64_t next_revision = status == UMI_STATUS_OK ? catalogue.revision + 1U : 0U;
    if (status == UMI_STATUS_OK)
        status = UmiBuildProfileStorageWrite(server, entry, candidate, next_revision);
    if (status == UMI_STATUS_OK)
        status = ConfigurationSet(server, entry, "name", name);
    if (status == UMI_STATUS_OK)
        status = ConfigurationSet(server, prefix, "source", root);
    if (status == UMI_STATUS_OK)
    {
        snprintf(number, sizeof number, "%zu",
                 catalogue.count + (slot == catalogue.count ? 1U : 0U));
        status = ConfigurationSet(server, prefix, "count", number);
    }
    if (status == UMI_STATUS_OK)
    {
        snprintf(number, sizeof number, "%" PRIu64, next_revision);
        status = ConfigurationSet(server, prefix, "revision", number);
    }
    if (status == UMI_STATUS_OK)
        status = ConfigurationSet(server, prefix, "schema", "1");
    if (status == UMI_STATUS_OK)
        status = umi_data_server_commit(server);
    if (status == UMI_STATUS_OK)
        *out_revision = next_revision;
    else
        status = ConfigurationFinishRead(server, status);
    free(candidate);
    free(previous);
    return status;
}

/* Lifecycle operations reuse the same catalogue revision and profile codec. */
#include "configuration_lifecycle.inc"
