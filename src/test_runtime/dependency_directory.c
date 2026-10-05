/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/dependency_directory.c
 *
 * PURPOSE:
 *   Implement represent one directory scanned for runtime dll or shared-library dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/dependency_directory.h"
#include "../base/value_archive_internal.h"
#include "umicom/base/text.h"
#include "../base/record_update_internal.h"
#include <string.h>
/*
 * Initialise test runtime dependency directory from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_dependency_directory_init(UmiTestRuntimeDependencyDirectory *value,const char *id)
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
 * Check that test runtime dependency directory satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_dependency_directory_validate(const UmiTestRuntimeDependencyDirectory *value)
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
 * Provide the test runtime dependency directory set category operation used by this module
 * and its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_test_runtime_dependency_directory_set_category(UmiTestRuntimeDependencyDirectory *value,const char *category)
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
UmiStatus umi_test_runtime_dependency_directory_set_category(UmiTestRuntimeDependencyDirectory *value,const char *category)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || category == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->category, sizeof(value->category),
                           category, &value->revision);
}
/*
 * Provide the test runtime dependency directory set detail operation used by this module
 * and its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_test_runtime_dependency_directory_set_detail(UmiTestRuntimeDependencyDirectory *value,const char *detail)
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
UmiStatus umi_test_runtime_dependency_directory_set_detail(UmiTestRuntimeDependencyDirectory *value,const char *detail)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}
/*
 * Provide the test runtime dependency directory set priority operation used by this module
 * and its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_dependency_directory_set_priority(UmiTestRuntimeDependencyDirectory *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->priority=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_test_runtime_dependency_directory_set_priority(UmiTestRuntimeDependencyDirectory *value,uint64_t number)
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
    value->priority=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Return the number of records represented by test runtime dependency directory set
 * resolved without changing their state.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_dependency_directory_set_resolved_count(UmiTestRuntimeDependencyDirectory *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->resolved_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_test_runtime_dependency_directory_set_resolved_count(UmiTestRuntimeDependencyDirectory *value,uint64_t number)
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
    value->resolved_count=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime dependency directory set active operation used by this module
 * and its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_dependency_directory_set_active(UmiTestRuntimeDependencyDirectory *value,bool active)
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
UmiStatus umi_test_runtime_dependency_directory_set_active(UmiTestRuntimeDependencyDirectory *value,bool active)
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
 * Provide the test runtime dependency directory same identity operation used by this
 * module and its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_test_runtime_dependency_directory_same_identity(const UmiTestRuntimeDependencyDirectory *left,const UmiTestRuntimeDependencyDirectory *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
#endif
bool umi_test_runtime_dependency_directory_same_identity(const UmiTestRuntimeDependencyDirectory *left, const UmiTestRuntimeDependencyDirectory *right)
{
    /* Treat missing terminators as invalid identities instead of reading into
     * adjacent fields. Other record state does not change identity equality. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/* A caller can reject an invalid dependency directory identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_test_runtime_dependency_directory_init_checked,
    UmiTestRuntimeDependencyDirectory, umi_test_runtime_dependency_directory_init, umi_test_runtime_dependency_directory_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf9216496a0c03bb7);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeDependencyDirectory *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeDependencyDirectory *)0)->category)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeDependencyDirectory *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTestRuntimeDependencyDirectory *)0)->id) - 1U +
        8U + sizeof(((UmiTestRuntimeDependencyDirectory *)0)->category) - 1U +
        8U + sizeof(((UmiTestRuntimeDependencyDirectory *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiTestRuntimeDependencyDirectory *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->category, sizeof(value->category));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->resolved_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiTestRuntimeDependencyDirectory *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->category, sizeof(value->category));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->priority = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->resolved_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus ArchiveValidate(const UmiTestRuntimeDependencyDirectory *value)
{
    return umi_test_runtime_dependency_directory_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_runtime_dependency_directory_archive_encode, umi_test_runtime_dependency_directory_archive_decode,
    UmiTestRuntimeDependencyDirectory, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
