/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/platform_requirement.c
 *
 * PURPOSE:
 *   Implement describe os and architecture requirements for one test family.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/platform_requirement.h"
#include "../base/value_archive_internal.h"
#include "umicom/base/text.h"
#include "../base/record_update_internal.h"
#include <string.h>
/*
 * Initialise test runtime platform requirement from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_platform_requirement_init(UmiTestRuntimePlatformRequirement *value,const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->revision=1U;
    value->enabled=true;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL)(void)umi_test_runtime_copy_text(value->id,sizeof(value->id),id);
    }
/*
 * Check that test runtime platform requirement satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_platform_requirement_validate(const UmiTestRuntimePlatformRequirement *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_test_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime platform requirement set detail operation used by this module
 * and its client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_test_runtime_platform_requirement_set_detail(UmiTestRuntimePlatformRequirement *value,const char *detail)
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
UmiStatus umi_test_runtime_platform_requirement_set_detail(UmiTestRuntimePlatformRequirement *value,const char *detail)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}
/*
 * Provide the test runtime platform requirement set architecture bits operation used by
 * this module and its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_platform_requirement_set_architecture_bits(UmiTestRuntimePlatformRequirement *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->architecture_bits=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_test_runtime_platform_requirement_set_architecture_bits(UmiTestRuntimePlatformRequirement *value,uint64_t number)
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
    value->architecture_bits=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime platform requirement set required operation used by this module
 * and its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_test_runtime_platform_requirement_set_required(UmiTestRuntimePlatformRequirement *value,uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->required=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_test_runtime_platform_requirement_set_required(UmiTestRuntimePlatformRequirement *value,uint64_t number)
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
    value->required=number;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the test runtime platform requirement same identity operation used by this
 * module and its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_test_runtime_platform_requirement_same_identity(const UmiTestRuntimePlatformRequirement *left,const UmiTestRuntimePlatformRequirement *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
#endif
bool umi_test_runtime_platform_requirement_same_identity(const UmiTestRuntimePlatformRequirement *left, const UmiTestRuntimePlatformRequirement *right)
{
    /* Treat missing terminators as invalid identities instead of reading into
     * adjacent fields. Other record state does not change identity equality. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/* A caller can reject an invalid platform requirement identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_test_runtime_platform_requirement_init_checked,
    UmiTestRuntimePlatformRequirement, umi_test_runtime_platform_requirement_init, umi_test_runtime_platform_requirement_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8b0a8556420d8836);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimePlatformRequirement *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimePlatformRequirement *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTestRuntimePlatformRequirement *)0)->id) - 1U +
        8U + sizeof(((UmiTestRuntimePlatformRequirement *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiTestRuntimePlatformRequirement *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->architecture_bits);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiTestRuntimePlatformRequirement *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->architecture_bits = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->required = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus ArchiveValidate(const UmiTestRuntimePlatformRequirement *value)
{
    return umi_test_runtime_platform_requirement_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_runtime_platform_requirement_archive_encode, umi_test_runtime_platform_requirement_archive_decode,
    UmiTestRuntimePlatformRequirement, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
