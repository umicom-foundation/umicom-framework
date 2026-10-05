/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/submodule.c
 *
 * PURPOSE:
 *   Model one Framework-owned Git submodule dependency.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/submodule.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

#include "umicom/repository/path.h"
#include "umicom/repository/ref.h"

/* Provide the copy field operation used by this module and its client applications. */
static UmiStatus copy_field(
    char *out, size_t capacity, const char *text, int allow_empty)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((!allow_empty && length == 0U) || length + 1U > capacity) {
        return length + 1U > capacity
            ? UMI_STATUS_CAPACITY_EXCEEDED
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise repository submodule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_repository_submodule_init(
    UmiRepositorySubmodule *submodule,
    const char *name,
    const char *path,
    const char *url,
    const char *branch,
    int required)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (submodule == NULL || name == NULL || path == NULL || url == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(submodule, 0, sizeof(*submodule));
    status = copy_field(submodule->name, sizeof(submodule->name), name, 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_control_path_normalize(
        path, submodule->path, sizeof(submodule->path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_field(submodule->url, sizeof(submodule->url), url, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (branch != NULL && branch[0] != '\0') {
        status = umi_repository_ref_copy(
            branch, submodule->branch, sizeof(submodule->branch));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    submodule->required = required != 0;
    return UMI_STATUS_OK;
}

/*
 * Check that repository submodule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_repository_submodule_validate(
    const UmiRepositorySubmodule *submodule)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (submodule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(submodule->name, '\0', sizeof(submodule->name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(submodule->path, '\0', sizeof(submodule->path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(submodule->url, '\0', sizeof(submodule->url)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(submodule->branch, '\0', sizeof(submodule->branch)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (submodule == NULL || submodule->name[0] == '\0' ||
        !umi_repository_control_path_is_safe_relative(submodule->path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (submodule->branch[0] != '\0' &&
        !umi_repository_ref_is_valid(submodule->branch)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRepositorySubmoduleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf4fab126e71a05ee);
    schema = (schema ^ (uint64_t)sizeof(((UmiRepositorySubmodule *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRepositorySubmodule *)0)->path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRepositorySubmodule *)0)->url)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRepositorySubmodule *)0)->branch)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRepositorySubmoduleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRepositorySubmodule *)0)->name) - 1U +
        8U + sizeof(((UmiRepositorySubmodule *)0)->path) - 1U +
        8U + sizeof(((UmiRepositorySubmodule *)0)->url) - 1U +
        8U + sizeof(((UmiRepositorySubmodule *)0)->branch) - 1U +
        8U;
}
static void UmiRepositorySubmoduleArchiveWrite(UmiArchiveWriter *writer, const UmiRepositorySubmodule *value)
{
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteText(writer, value->url, sizeof(value->url));
    UmiArchiveWriteText(writer, value->branch, sizeof(value->branch));
    UmiArchiveWriteSigned(writer, (int64_t)value->required);
}
static void UmiRepositorySubmoduleArchiveRead(UmiArchiveReader *reader, UmiRepositorySubmodule *value)
{
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    UmiArchiveReadText(reader, value->url, sizeof(value->url));
    UmiArchiveReadText(reader, value->branch, sizeof(value->branch));
    value->required = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiRepositorySubmoduleArchiveValidate(const UmiRepositorySubmodule *value)
{
    return umi_repository_submodule_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_repository_submodule_archive_encode, umi_repository_submodule_archive_decode,
    UmiRepositorySubmodule, UmiRepositorySubmoduleArchiveSchema, UmiRepositorySubmoduleArchiveBound, UmiRepositorySubmoduleArchiveWrite, UmiRepositorySubmoduleArchiveRead, UmiRepositorySubmoduleArchiveValidate)
