/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/query_plan.c
 *
 * PURPOSE:
 *   Compose predicate, projection, ordering and join counts into a reviewable backend-neutral query plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/query_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation keeps query semantics independent from SQL-string construction. */ UmiStatus umi_data_query_plan_init(UmiDataQueryPlan *plan,const char *plan_id,const char *root_table){UmiStatus s;if(plan==NULL||plan_id==NULL||root_table==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(plan,0,sizeof(*plan));s=umi_data_enterprise_copy_text(plan->plan_id,sizeof(plan->plan_id),plan_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(plan->root_table,sizeof(plan->root_table),root_table);if(s!=UMI_STATUS_OK)return s;plan->read_only=true;return UMI_STATUS_OK;}
/* Shape evidence feeds cost, policy and observability without executing a query. */ UmiStatus umi_data_query_plan_shape(UmiDataQueryPlan *plan,size_t predicates,size_t projections,size_t joins,size_t orders,uint64_t row_limit){if(plan==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(predicates>UMI_DATA_ENTERPRISE_MAX_ITEMS||projections>UMI_DATA_ENTERPRISE_MAX_ITEMS||joins>UMI_DATA_ENTERPRISE_MAX_ITEMS||orders>UMI_DATA_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;plan->predicate_count=predicates;plan->projection_count=projections;plan->join_count=joins;plan->order_count=orders;plan->row_limit=row_limit;return umi_data_query_plan_validate(plan);}
/* Validation blocks accidental unbounded reads by requiring an explicit row limit. */ UmiStatus umi_data_query_plan_validate(const UmiDataQueryPlan *plan){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->plan_id, '\0', sizeof(plan->plan_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->root_table, '\0', sizeof(plan->root_table)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* The final success return is independent of the rejected-plan branch.
 * Spell out the original branch boundaries without changing the row-limit
 * requirement or the bounded string checks above.
 * The superseded implementation is retained below for engineering review. */
#if 0
if(plan==NULL||plan->plan_id[0]=='\0'||plan->root_table[0]=='\0'||plan->row_limit==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
#endif
    /* Reject the same incomplete or unbounded plan, then return success separately. */
    if (plan == NULL || plan->plan_id[0] == '\0' ||
        plan->root_table[0] == '\0' || plan->row_limit == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataQueryPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x11fb6550b44e9688);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryPlan *)0)->plan_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataQueryPlan *)0)->root_table)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataQueryPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataQueryPlan *)0)->plan_id) - 1U +
        8U + sizeof(((UmiDataQueryPlan *)0)->root_table) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataQueryPlanArchiveWrite(UmiArchiveWriter *writer, const UmiDataQueryPlan *value)
{
    UmiArchiveWriteText(writer, value->plan_id, sizeof(value->plan_id));
    UmiArchiveWriteText(writer, value->root_table, sizeof(value->root_table));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->predicate_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->projection_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->join_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->order_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->row_limit);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->read_only);
}
static void UmiDataQueryPlanArchiveRead(UmiArchiveReader *reader, UmiDataQueryPlan *value)
{
    UmiArchiveReadText(reader, value->plan_id, sizeof(value->plan_id));
    UmiArchiveReadText(reader, value->root_table, sizeof(value->root_table));
    value->predicate_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->projection_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->join_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->order_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->row_limit = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->read_only = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataQueryPlanArchiveValidate(const UmiDataQueryPlan *value)
{
    return umi_data_query_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_query_plan_archive_encode, umi_data_query_plan_archive_decode,
    UmiDataQueryPlan, UmiDataQueryPlanArchiveSchema, UmiDataQueryPlanArchiveBound, UmiDataQueryPlanArchiveWrite, UmiDataQueryPlanArchiveRead, UmiDataQueryPlanArchiveValidate)
