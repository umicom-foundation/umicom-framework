/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/dependency.c
 *
 * PURPOSE:
 *   Define reusable repository dependency nodes.
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
#include "umicom/repository/dependency.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

#include "umicom/repository/path.h"

/*
 * Initialise repository dependency from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_repository_dependency_init(
    UmiRepositoryDependency *dependency,
    const char *id,
    const char *path,
    int required)
{
    size_t id_length;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dependency == NULL || id == NULL || path == NULL || id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    id_length = strlen(id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (id_length + 1U > sizeof(dependency->id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(dependency, 0, sizeof(*dependency));
    (void)memcpy(dependency->id, id, id_length + 1U);
    status = umi_repository_control_path_normalize(
        path, dependency->path, sizeof(dependency->path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    dependency->required = required != 0;
    return UMI_STATUS_OK;
}

/*
 * Check that repository dependency satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_repository_dependency_validate(
    const UmiRepositoryDependency *dependency)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (dependency == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(dependency->id, '\0', sizeof(dependency->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(dependency->path, '\0', sizeof(dependency->path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dependency == NULL || dependency->id[0] == '\0' ||
        !umi_repository_control_path_is_safe_relative(dependency->path)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRepositoryDependencyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x707268d474616dcf);
    schema = (schema ^ (uint64_t)sizeof(((UmiRepositoryDependency *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRepositoryDependency *)0)->path)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRepositoryDependencyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRepositoryDependency *)0)->id) - 1U +
        8U + sizeof(((UmiRepositoryDependency *)0)->path) - 1U +
        8U;
}
static void UmiRepositoryDependencyArchiveWrite(UmiArchiveWriter *writer, const UmiRepositoryDependency *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteSigned(writer, (int64_t)value->required);
}
static void UmiRepositoryDependencyArchiveRead(UmiArchiveReader *reader, UmiRepositoryDependency *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    value->required = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiRepositoryDependencyArchiveValidate(const UmiRepositoryDependency *value)
{
    return umi_repository_dependency_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_repository_dependency_archive_encode, umi_repository_dependency_archive_decode,
    UmiRepositoryDependency, UmiRepositoryDependencyArchiveSchema, UmiRepositoryDependencyArchiveBound, UmiRepositoryDependencyArchiveWrite, UmiRepositoryDependencyArchiveRead, UmiRepositoryDependencyArchiveValidate)
