/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/commit_node.c
 *
 * PURPOSE:
 *   Implement one commit in the Framework-owned history graph.
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
#include "umicom/vcs/advanced/commit_node.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced commit node from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_node_init(UmiVcsAdvancedCommitNode *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->generation = 1U;
}

/*
 * Check that vcs advanced commit node satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_commit_node_validate(const UmiVcsAdvancedCommitNode *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->oid, '\0', sizeof(value->oid)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->subject, '\0', sizeof(value->subject)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->author, '\0', sizeof(value->author)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced commit node into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_commit_node_set(UmiVcsAdvancedCommitNode *value,
                                              const char *oid,
                                              const char *subject,
                                              const char *author)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !umi_vcs_advanced_text_present(oid)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->oid, sizeof(value->oid), oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->subject, sizeof(value->subject), subject);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_vcs_advanced_copy_text(value->author, sizeof(value->author), author);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedCommitNodeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x27e1db1943eb0795);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedCommitNode *)0)->oid)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedCommitNode *)0)->subject)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiVcsAdvancedCommitNode *)0)->author)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiVcsAdvancedCommitNodeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiVcsAdvancedCommitNode *)0)->oid) - 1U +
        8U + sizeof(((UmiVcsAdvancedCommitNode *)0)->subject) - 1U +
        8U + sizeof(((UmiVcsAdvancedCommitNode *)0)->author) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedCommitNodeArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedCommitNode *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->oid, sizeof(value->oid));
    UmiArchiveWriteText(writer, value->subject, sizeof(value->subject));
    UmiArchiveWriteText(writer, value->author, sizeof(value->author));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_seconds);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->parent_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
    UmiArchiveWriteSigned(writer, (int64_t)value->merge_commit);
}
static void UmiVcsAdvancedCommitNodeArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedCommitNode *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->oid, sizeof(value->oid));
    UmiArchiveReadText(reader, value->subject, sizeof(value->subject));
    UmiArchiveReadText(reader, value->author, sizeof(value->author));
    value->timestamp_seconds = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->parent_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->generation = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->merge_commit = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiVcsAdvancedCommitNodeArchiveValidate(const UmiVcsAdvancedCommitNode *value)
{
    return umi_vcs_advanced_commit_node_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_commit_node_archive_encode, umi_vcs_advanced_commit_node_archive_decode,
    UmiVcsAdvancedCommitNode, UmiVcsAdvancedCommitNodeArchiveSchema, UmiVcsAdvancedCommitNodeArchiveBound, UmiVcsAdvancedCommitNodeArchiveWrite, UmiVcsAdvancedCommitNodeArchiveRead, UmiVcsAdvancedCommitNodeArchiveValidate)
