/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_join.c
 *
 * PURPOSE:
 *   Describe backend-neutral joins for cost analysis and SQL generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_join.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_query_join_init(UmiDataQueryJoin *item, const char *join_id, const char *left_table, const char *right_table, const char *condition, bool outer_join) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->join_id,sizeof(item->join_id),join_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->left_table,sizeof(item->left_table),left_table);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->right_table,sizeof(item->right_table),right_table);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->condition,sizeof(item->condition),condition);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->outer_join=outer_join;
    return umi_data_query_join_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_query_join_validate(const UmiDataQueryJoin *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->join_id, '\0', sizeof(item->join_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->left_table, '\0', sizeof(item->left_table)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->right_table, '\0', sizeof(item->right_table)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->condition, '\0', sizeof(item->condition)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->join_id[0] != '\0' && item->left_table[0] != '\0' && item->right_table[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataQueryJoinArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3b60a256e80d4760);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryJoin *)0)->join_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryJoin *)0)->left_table)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryJoin *)0)->right_table)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryJoin *)0)->condition)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataQueryJoinArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataQueryJoin *)0)->join_id) - 1U +
        8U + sizeof(((UmiDataQueryJoin *)0)->left_table) - 1U +
        8U + sizeof(((UmiDataQueryJoin *)0)->right_table) - 1U +
        8U + sizeof(((UmiDataQueryJoin *)0)->condition) - 1U +
        8U;
}
static void UmiDataQueryJoinArchiveWrite(UmiArchiveWriter *writer, const UmiDataQueryJoin *value)
{
    UmiArchiveWriteText(writer, value->join_id, sizeof(value->join_id));
    UmiArchiveWriteText(writer, value->left_table, sizeof(value->left_table));
    UmiArchiveWriteText(writer, value->right_table, sizeof(value->right_table));
    UmiArchiveWriteText(writer, value->condition, sizeof(value->condition));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->outer_join);
}
static void UmiDataQueryJoinArchiveRead(UmiArchiveReader *reader, UmiDataQueryJoin *value)
{
    UmiArchiveReadText(reader, value->join_id, sizeof(value->join_id));
    UmiArchiveReadText(reader, value->left_table, sizeof(value->left_table));
    UmiArchiveReadText(reader, value->right_table, sizeof(value->right_table));
    UmiArchiveReadText(reader, value->condition, sizeof(value->condition));
    value->outer_join = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataQueryJoinArchiveValidate(const UmiDataQueryJoin *value)
{
    return umi_data_query_join_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_query_join_archive_encode, umi_data_query_join_archive_decode,
    UmiDataQueryJoin, UmiDataQueryJoinArchiveSchema, UmiDataQueryJoinArchiveBound, UmiDataQueryJoinArchiveWrite, UmiDataQueryJoinArchiveRead, UmiDataQueryJoinArchiveValidate)
