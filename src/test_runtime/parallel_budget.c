/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/parallel_budget.c
 *
 * PURPOSE:
 *   Implement define safe parallelism for constrained windows and ci environments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/parallel_budget.h"
#include "../base/value_archive_internal.h"
#include "umicom/base/text.h"
#include "../base/record_update_internal.h"
#include <string.h>
/*
 * Initialise test runtime parallel budget from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_parallel_budget_init(UmiTestRuntimeParallelBudget *value,const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->active=true;
    value->revision=1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
/*
 * Check that test runtime parallel budget satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_parallel_budget_validate(const UmiTestRuntimeParallelBudget *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->category,sizeof(value->category))||!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget set category operation used by this module and
 * its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_test_runtime_parallel_budget_set_category(UmiTestRuntimeParallelBudget *value,const char *category)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->category,sizeof(value->category),category);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
#endif
UmiStatus umi_test_runtime_parallel_budget_set_category(UmiTestRuntimeParallelBudget *value,const char *category)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || category == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->category, sizeof(value->category),
                           category, &value->revision);
}
/*
 * Provide the test runtime parallel budget set detail operation used by this module and
 * its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_test_runtime_parallel_budget_set_detail(UmiTestRuntimeParallelBudget *value,const char *detail)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_test_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
#endif
UmiStatus umi_test_runtime_parallel_budget_set_detail(UmiTestRuntimeParallelBudget *value,const char *detail)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}
/*
 * Provide the test runtime parallel budget set maximum parallel operation used by this
 * module and its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_parallel_budget_set_maximum_parallel(UmiTestRuntimeParallelBudget *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->maximum_parallel=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_test_runtime_parallel_budget_set_maximum_parallel(UmiTestRuntimeParallelBudget *value,uint64_t number)
{
    /* Do not change a field when its observation token cannot advance.
     * Reusing an old revision could make a stale review appear current. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->maximum_parallel=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget set processor cost operation used by this
 * module and its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_parallel_budget_set_processor_cost(UmiTestRuntimeParallelBudget *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->processor_cost=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_test_runtime_parallel_budget_set_processor_cost(UmiTestRuntimeParallelBudget *value,uint64_t number)
{
    /* Do not change a field when its observation token cannot advance.
     * Reusing an old revision could make a stale review appear current. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->processor_cost=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget set active operation used by this module and
 * its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_parallel_budget_set_active(UmiTestRuntimeParallelBudget *value,bool active)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_test_runtime_parallel_budget_set_active(UmiTestRuntimeParallelBudget *value,bool active)
{
    /* Do not change a field when its observation token cannot advance.
     * Reusing an old revision could make a stale review appear current. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->active=active;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime parallel budget same identity operation used by this module and
 * its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_test_runtime_parallel_budget_same_identity(const UmiTestRuntimeParallelBudget *left,const UmiTestRuntimeParallelBudget *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
#endif
bool umi_test_runtime_parallel_budget_same_identity(const UmiTestRuntimeParallelBudget *left, const UmiTestRuntimeParallelBudget *right)
{
    /* Treat missing terminators as invalid identities instead of reading into
     * adjacent fields. Other record state does not change identity equality. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/* A caller can reject an invalid parallel budget identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_test_runtime_parallel_budget_init_checked,
    UmiTestRuntimeParallelBudget, umi_test_runtime_parallel_budget_init, umi_test_runtime_parallel_budget_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7c377d2062d7bb4e);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeParallelBudget *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeParallelBudget *)0)->category)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeParallelBudget *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTestRuntimeParallelBudget *)0)->id) - 1U +
        8U + sizeof(((UmiTestRuntimeParallelBudget *)0)->category) - 1U +
        8U + sizeof(((UmiTestRuntimeParallelBudget *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiTestRuntimeParallelBudget *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->category, sizeof(value->category));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_parallel);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->processor_cost);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiTestRuntimeParallelBudget *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->category, sizeof(value->category));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->maximum_parallel = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->processor_cost = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus ArchiveValidate(const UmiTestRuntimeParallelBudget *value)
{
    return umi_test_runtime_parallel_budget_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_runtime_parallel_budget_archive_encode, umi_test_runtime_parallel_budget_archive_decode,
    UmiTestRuntimeParallelBudget, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
