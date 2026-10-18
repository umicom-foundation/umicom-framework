/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/filter_expression.c
 *
 * PURPOSE:
 *   Describe a bounded textual equality/prefix filter used by route and workflow policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/integration/fabric/filter_expression.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
#include <limits.h>

/*
 * Initialise fabric filter expression from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_fabric_filter_expression_init(UmiFabricFilterExpression *item, const char *field, const char *operation, const char *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->field,sizeof(item->field),field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->operation,sizeof(item->operation),operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;return umi_fabric_copy_text(item->value,sizeof(item->value),value);
    return umi_fabric_filter_expression_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_fabric_filter_expression_init_build_value(UmiFabricFilterExpression *item, const char *field, const char *operation, const char *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item,0,sizeof(*item));
    UmiStatus s=umi_fabric_copy_text(item->field,sizeof(item->field),field);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_fabric_copy_text(item->operation,sizeof(item->operation),operation);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;return umi_fabric_copy_text(item->value,sizeof(item->value),value);
    return umi_fabric_filter_expression_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_fabric_filter_expression_init, UmiFabricFilterExpression, item,
    (UmiFabricFilterExpression *item, const char *field, const char *operation, const char *value),
    umi_fabric_filter_expression_init_build_value(candidate, field, operation, value))

/*
 * Check that fabric filter expression satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_filter_expression_validate(const UmiFabricFilterExpression *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->field, '\0', sizeof(item->field)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation, '\0', sizeof(item->operation)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->value, '\0', sizeof(item->value)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->field[0]!='\0' && (strcmp(item->operation,"eq")==0 || strcmp(item->operation,"prefix")==0) && item->value[0]!='\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiFabricFilterExpressionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6f462e4d460d2b47);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricFilterExpression *)0)->field)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricFilterExpression *)0)->operation)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiFabricFilterExpression *)0)->value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiFabricFilterExpressionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiFabricFilterExpression *)0)->field) - 1U +
        8U + sizeof(((UmiFabricFilterExpression *)0)->operation) - 1U +
        8U + sizeof(((UmiFabricFilterExpression *)0)->value) - 1U;
}
static void UmiFabricFilterExpressionArchiveWrite(UmiArchiveWriter *writer, const UmiFabricFilterExpression *value)
{
    UmiArchiveWriteText(writer, value->field, sizeof(value->field));
    UmiArchiveWriteText(writer, value->operation, sizeof(value->operation));
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
}
static void UmiFabricFilterExpressionArchiveRead(UmiArchiveReader *reader, UmiFabricFilterExpression *value)
{
    UmiArchiveReadText(reader, value->field, sizeof(value->field));
    UmiArchiveReadText(reader, value->operation, sizeof(value->operation));
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
}
static UmiStatus UmiFabricFilterExpressionArchiveValidate(const UmiFabricFilterExpression *value)
{
    return umi_fabric_filter_expression_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_fabric_filter_expression_archive_encode, umi_fabric_filter_expression_archive_decode,
    UmiFabricFilterExpression, UmiFabricFilterExpressionArchiveSchema, UmiFabricFilterExpressionArchiveBound, UmiFabricFilterExpressionArchiveWrite, UmiFabricFilterExpressionArchiveRead, UmiFabricFilterExpressionArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_fabric_filter_expression_archive_decode_with_workspace, UmiFabricFilterExpression,
    UmiFabricFilterExpressionArchiveSchema, UmiFabricFilterExpressionArchiveBound, UmiFabricFilterExpressionArchiveRead, UmiFabricFilterExpressionArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_fabric_filter_expression_archive_decode_if_unchanged, umi_fabric_filter_expression_archive_encode,
    UmiFabricFilterExpression, UmiFabricFilterExpressionArchiveSchema, UmiFabricFilterExpressionArchiveBound, UmiFabricFilterExpressionArchiveRead, UmiFabricFilterExpressionArchiveValidate)
