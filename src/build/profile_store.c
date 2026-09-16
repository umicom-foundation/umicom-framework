/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/profile_store.c
 *
 * PURPOSE:
 *   Keep project settings atomic, bounded and independent of GUI objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/profile_store.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/platform/recent_items.h"

/* Each field fits both Data Server backends. Offsets describe only character
 * arrays in the existing profile; the storage format never depends on padding. */
typedef struct ProfileTextField {
    const char *name;
    size_t offset;
    size_t capacity;
} ProfileTextField;
#define PROFILE_FIELD(member) { #member, offsetof(UmiBuildProfile, member), sizeof(((UmiBuildProfile *)0)->member) }
static const ProfileTextField TEXT_FIELDS[] = {
    PROFILE_FIELD(profile_id), PROFILE_FIELD(source_directory),
    PROFILE_FIELD(build_directory), PROFILE_FIELD(generator),
    PROFILE_FIELD(compiler), PROFILE_FIELD(configuration), PROFILE_FIELD(preset),
    PROFILE_FIELD(build_target), PROFILE_FIELD(run_program),
    PROFILE_FIELD(run_argument), PROFILE_FIELD(install_directory)
};
#undef PROFILE_FIELD
static const char *const NUMBER_FIELDS[] = {
    "parallel_jobs", "timeout_ms", "build_testing", "strict_warnings", "revision"
};

/* Reuse the shared URI fingerprint. The full source path is also stored and
 * compared on every read, so a hash collision never substitutes another project.
 * Windows ASCII path case follows the existing path comparison contract. */
static UmiStatus ProfileIdentity(const char *root, char *normalised, char *prefix)
{
    char current[UMI_BUILD_PATH_CAPACITY];
    char identity[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status;
    if (root == NULL || root[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_current_directory(current, sizeof(current));
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(root, current, normalised, UMI_BUILD_PATH_CAPACITY);
    if (status != UMI_STATUS_OK) return status;
    (void)memcpy(identity, normalised, strlen(normalised) + 1U);
#ifdef _WIN32
    for (size_t index = 0U; identity[index] != '\0'; ++index)
        if (identity[index] >= 'A' && identity[index] <= 'Z')
            identity[index] = (char)(identity[index] + ('a' - 'A'));
    const char *scope = "build-profile-windows";
#else
    const char *scope = "build-profile-posix";
#endif
    return umi_platform_recent_item_id_from_uri(scope, identity, prefix, 128U);
}

static UmiStatus ProfileKey(const char *prefix, const char *field, char *key)
{
    int count = snprintf(key, 192U, "%s.%s", prefix, field);
    return count < 0 || count >= 192 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus ReadField(UmiDataServer *server, const char *prefix,
    const char *field, char *value, size_t capacity)
{
    char key[192];
    UmiStatus status = ProfileKey(prefix, field, key);
    return status == UMI_STATUS_OK
        ? umi_data_server_get(server, key, value, capacity) : status;
}

static UmiStatus WriteField(UmiDataServer *server, const char *prefix,
    const char *field, const char *value)
{
    char key[192];
    UmiStatus status = ProfileKey(prefix, field, key);
    return status == UMI_STATUS_OK ? umi_data_server_set(server, key, value) : status;
}

/* Stored numbers have a deliberately narrow syntax. Reject trailing text,
 * negative unsigned fields and overflow before converting to the public type. */
static UmiStatus ReadUnsigned(UmiDataServer *server, const char *prefix,
    const char *field, uint64_t maximum, uint64_t *outValue)
{
    char text[32];
    char *end;
    uintmax_t value;
    UmiStatus status = ReadField(server, prefix, field, text, sizeof(text));
    if (status != UMI_STATUS_OK) return status;
    if (text[0] < '0' || text[0] > '9') return UMI_STATUS_PARSE_ERROR;
    errno = 0;
    value = strtoumax(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || value > maximum)
        return UMI_STATUS_PARSE_ERROR;
    *outValue = (uint64_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus ReadInteger(UmiDataServer *server, const char *prefix,
    const char *field, int *outValue)
{
    char text[32];
    char *end;
    intmax_t value;
    UmiStatus status = ReadField(server, prefix, field, text, sizeof(text));
    if (status != UMI_STATUS_OK) return status;
    if ((text[0] < '0' || text[0] > '9') &&
        !(text[0] == '-' && text[1] >= '0' && text[1] <= '9'))
        return UMI_STATUS_PARSE_ERROR;
    errno = 0;
    value = strtoimax(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || value < INT_MIN || value > INT_MAX)
        return UMI_STATUS_PARSE_ERROR;
    *outValue = (int)value;
    return UMI_STATUS_OK;
}

/* Missing schema means absent only when the entire known record is absent.
 * A partial/corrupt record must not be silently overwritten as a new project. */
static UmiStatus CheckAbsent(UmiDataServer *server, const char *prefix)
{
    char value[4096];
    for (size_t index = 0U; index < sizeof(TEXT_FIELDS)/sizeof(TEXT_FIELDS[0]); ++index) {
        UmiStatus status = ReadField(server, prefix, TEXT_FIELDS[index].name, value, sizeof(value));
        if (status != UMI_STATUS_NOT_FOUND)
            return status == UMI_STATUS_OK ? UMI_STATUS_PARSE_ERROR : status;
    }
    for (size_t index = 0U; index < sizeof(NUMBER_FIELDS)/sizeof(NUMBER_FIELDS[0]); ++index) {
        UmiStatus status = ReadField(server, prefix, NUMBER_FIELDS[index], value, sizeof(value));
        if (status != UMI_STATUS_NOT_FOUND)
            return status == UMI_STATUS_OK ? UMI_STATUS_PARSE_ERROR : status;
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Read inside a transaction so no independently committed writer can mix two
 * revisions. The candidate remains private until its complete contract passes. */
static UmiStatus ReadProfile(UmiDataServer *server, const char *prefix,
    const char *root, UmiBuildProfile *profile, uint64_t *revision)
{
    char schema[32];
    uint64_t number = 0U;
    UmiStatus status = ReadField(server, prefix, "schema", schema, sizeof(schema));
    if (status == UMI_STATUS_NOT_FOUND) return CheckAbsent(server, prefix);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(schema, "1") != 0) return UMI_STATUS_INVALID_STATE;
    (void)memset(profile, 0, sizeof(*profile));
    for (size_t index = 0U; index < sizeof(TEXT_FIELDS)/sizeof(TEXT_FIELDS[0]); ++index) {
        const ProfileTextField *field = &TEXT_FIELDS[index];
        status = ReadField(server, prefix, field->name,
            (char *)profile + field->offset, field->capacity);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK) status = ReadUnsigned(server, prefix, "parallel_jobs", UINT_MAX, &number);
    if (status == UMI_STATUS_OK) profile->parallel_jobs = (unsigned)number;
    if (status == UMI_STATUS_OK) status = ReadUnsigned(server, prefix, "timeout_ms", UINT32_MAX, &number);
    if (status == UMI_STATUS_OK) profile->timeout_ms = (uint32_t)number;
    if (status == UMI_STATUS_OK) status = ReadInteger(server, prefix, "build_testing", &profile->build_testing);
    if (status == UMI_STATUS_OK) status = ReadInteger(server, prefix, "strict_warnings", &profile->strict_warnings);
    if (status == UMI_STATUS_OK) status = ReadUnsigned(server, prefix, "revision", UINT64_MAX, revision);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) return status;
    if (*revision == 0U || !umi_path_is_absolute(profile->source_directory) ||
        !umi_path_equal(root, profile->source_directory)) return UMI_STATUS_INVALID_STATE;
    return umi_build_profile_validate(profile, NULL, 0U) == UMI_STATUS_OK
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus UmiBuildProfileStoreLoad(UmiDataServer *server,
    const char *workspaceRoot, UmiBuildProfile *outProfile, uint64_t *outRevision)
{
    UmiBuildProfile candidate;
    char root[UMI_BUILD_PATH_CAPACITY];
    char prefix[128];
    uint64_t revision = 0U;
    UmiStatus status, finish;
    if (server == NULL || outProfile == NULL || outRevision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = ProfileIdentity(workspaceRoot, root, prefix);
    if (status != UMI_STATUS_OK) return status;
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) return status;
    status = ReadProfile(server, prefix, root, &candidate, &revision);
    finish = umi_data_server_rollback(server); /* End the read-only transaction. */
    if (finish != UMI_STATUS_OK) return finish;
    if (status == UMI_STATUS_OK) { *outProfile = candidate; *outRevision = revision; }
    else if (status == UMI_STATUS_NOT_FOUND) *outRevision = 0U;
    return status;
}

UmiStatus UmiBuildProfileStoreSave(UmiDataServer *server,
    const UmiBuildProfile *profile, uint64_t expectedRevision, uint64_t *outRevision)
{
    UmiBuildProfile candidate, previous;
    char root[UMI_BUILD_PATH_CAPACITY], prefix[128], number[32];
    uint64_t revision = 0U;
    UmiStatus status, finish;
    if (server == NULL || outRevision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    status = ProfileIdentity(profile->source_directory, root, prefix);
    if (status != UMI_STATUS_OK) return status;
    candidate = *profile;
    (void)memcpy(candidate.source_directory, root, strlen(root) + 1U);
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) return status;
    status = ReadProfile(server, prefix, root, &previous, &revision);
    if (status == UMI_STATUS_NOT_FOUND) { revision = 0U; status = UMI_STATUS_OK; }
    if (status == UMI_STATUS_OK && expectedRevision != revision) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < sizeof(TEXT_FIELDS)/sizeof(TEXT_FIELDS[0]); ++index) {
        const ProfileTextField *field = &TEXT_FIELDS[index];
        status = WriteField(server, prefix, field->name, (const char *)&candidate + field->offset);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(number, sizeof(number), "%u", candidate.parallel_jobs);
        status = WriteField(server, prefix, "parallel_jobs", number);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(number, sizeof(number), "%" PRIu32, candidate.timeout_ms);
        status = WriteField(server, prefix, "timeout_ms", number);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(number, sizeof(number), "%d", candidate.build_testing);
        status = WriteField(server, prefix, "build_testing", number);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(number, sizeof(number), "%d", candidate.strict_warnings);
        status = WriteField(server, prefix, "strict_warnings", number);
    }
    if (status == UMI_STATUS_OK) {
        (void)snprintf(number, sizeof(number), "%" PRIu64, revision + 1U);
        status = WriteField(server, prefix, "revision", number);
    }
    if (status == UMI_STATUS_OK) status = WriteField(server, prefix, "schema", "1");
    if (status == UMI_STATUS_OK) {
        status = umi_data_server_commit(server);
        if (status == UMI_STATUS_OK) { *outRevision = revision + 1U; return status; }
    }
    finish = umi_data_server_rollback(server);
    return finish == UMI_STATUS_OK ? status : finish;
}
