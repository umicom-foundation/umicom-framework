/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/dependency.h
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
#ifndef UMICOM_REPOSITORY_DEPENDENCY_H
#define UMICOM_REPOSITORY_DEPENDENCY_H
#include "umicom/repository/control_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository dependency data shared with callers of this public contract.
 */
typedef struct UmiRepositoryDependency {
    char id[UMI_REPOSITORY_CONTROL_NAME_CAPACITY];
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    int required;
} UmiRepositoryDependency;
/**
 * Initialise repository dependency from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_repository_dependency_init(
    UmiRepositoryDependency *dependency,
    const char *id,
    const char *path,
    int required);
/**
 * Check that repository dependency satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_repository_dependency_validate(
    const UmiRepositoryDependency *dependency);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_repository_dependency_archive_encode(const UmiRepositoryDependency *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_repository_dependency_archive_decode(const void *bytes, size_t byte_count,
    UmiRepositoryDependency *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_repository_dependency_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRepositoryDependency *expected, UmiRepositoryDependency *value);


#ifdef __cplusplus
}
#endif
#endif
