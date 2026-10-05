/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/repository_state.c
 *
 * PURPOSE:
 *   Aggregate branch/upstream and in-progress Git operation state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/repository_state.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced repository state from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_repository_state_init(UmiVcsAdvancedRepositoryState *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced repository state satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_repository_state_validate(const UmiVcsAdvancedRepositoryState *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->branch, '\0', sizeof(value->branch)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->upstream, '\0', sizeof(value->upstream)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->head_oid, '\0', sizeof(value->head_oid)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->head_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced repository state operation in progress operation used by this
 * module and its client applications.
 */
int umi_vcs_advanced_repository_state_operation_in_progress(const UmiVcsAdvancedRepositoryState *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0;
    return value->merge_in_progress || value->rebase_in_progress ||
           value->cherry_pick_in_progress || value->revert_in_progress ||
           value->bisect_in_progress;
}
/*
 * Provide the vcs advanced repository state diverged operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_repository_state_diverged(const UmiVcsAdvancedRepositoryState *value)
{
    return value != NULL && value->ahead > 0U && value->behind > 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedRepositoryStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb0409600654e5c8b);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedRepositoryState *)0)->branch)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedRepositoryState *)0)->upstream)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedRepositoryState *)0)->head_oid)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiVcsAdvancedRepositoryStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiVcsAdvancedRepositoryState *)0)->branch) - 1U +
        8U + sizeof(((UmiVcsAdvancedRepositoryState *)0)->upstream) - 1U +
        8U + sizeof(((UmiVcsAdvancedRepositoryState *)0)->head_oid) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedRepositoryStateArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedRepositoryState *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->branch, sizeof(value->branch));
    UmiArchiveWriteText(writer, value->upstream, sizeof(value->upstream));
    UmiArchiveWriteText(writer, value->head_oid, sizeof(value->head_oid));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->ahead);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->behind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->conflicts);
    UmiArchiveWriteSigned(writer, (int64_t)value->detached_head);
    UmiArchiveWriteSigned(writer, (int64_t)value->merge_in_progress);
    UmiArchiveWriteSigned(writer, (int64_t)value->rebase_in_progress);
    UmiArchiveWriteSigned(writer, (int64_t)value->cherry_pick_in_progress);
    UmiArchiveWriteSigned(writer, (int64_t)value->revert_in_progress);
    UmiArchiveWriteSigned(writer, (int64_t)value->bisect_in_progress);
}
static void UmiVcsAdvancedRepositoryStateArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedRepositoryState *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->branch, sizeof(value->branch));
    UmiArchiveReadText(reader, value->upstream, sizeof(value->upstream));
    UmiArchiveReadText(reader, value->head_oid, sizeof(value->head_oid));
    value->ahead = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->behind = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->conflicts = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->detached_head = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->merge_in_progress = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->rebase_in_progress = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->cherry_pick_in_progress = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revert_in_progress = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->bisect_in_progress = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiVcsAdvancedRepositoryStateArchiveValidate(const UmiVcsAdvancedRepositoryState *value)
{
    return umi_vcs_advanced_repository_state_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_repository_state_archive_encode, umi_vcs_advanced_repository_state_archive_decode,
    UmiVcsAdvancedRepositoryState, UmiVcsAdvancedRepositoryStateArchiveSchema, UmiVcsAdvancedRepositoryStateArchiveBound, UmiVcsAdvancedRepositoryStateArchiveWrite, UmiVcsAdvancedRepositoryStateArchiveRead, UmiVcsAdvancedRepositoryStateArchiveValidate)
