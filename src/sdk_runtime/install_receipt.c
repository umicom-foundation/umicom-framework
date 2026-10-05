/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/install_receipt.c
 *
 * PURPOSE:
 *   Implement retain installed component receipts for repair and uninstall planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/install_receipt.h"
#include "../base/value_archive_internal.h"
#include "umicom/base/text.h"
#include "../base/record_update_internal.h"
#include <string.h>
/*
 * Initialise sdk runtime install receipt from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_install_receipt_init(UmiSdkRuntimeInstallReceipt *value,const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return;
    memset(value,0,sizeof(*value));
    value->structure_size=(uint32_t)sizeof(*value);
    value->state=UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled=true;
    value->revision=1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(id!=NULL)(void)umi_sdk_runtime_copy_text(value->id,sizeof(value->id),id);
    }
/*
 * Check that sdk runtime install receipt satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_install_receipt_validate(const UmiSdkRuntimeInstallReceipt *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||value->structure_size!=sizeof(*value))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_sdk_runtime_text_is_valid(value->id,sizeof(value->id))||value->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_sdk_runtime_text_is_valid(value->path,sizeof(value->path))||!umi_sdk_runtime_text_is_valid(value->detail,sizeof(value->detail)))return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime install receipt set path operation used by this module and its
 * client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_sdk_runtime_install_receipt_set_path(UmiSdkRuntimeInstallReceipt *value,const char *path)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->path,sizeof(value->path),path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
#endif
UmiStatus umi_sdk_runtime_install_receipt_set_path(UmiSdkRuntimeInstallReceipt *value,const char *path)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->path, sizeof(value->path),
                           path, &value->revision);
}
/*
 * Provide the sdk runtime install receipt set detail operation used by this module and its
 * client applications.
 */
/* The shared text publication helper replaces a separate copy and revision increment. It avoids partial edits, overlapping-copy hazards and revision reuse; the previous implementation remains for review. */
#if 0
UmiStatus umi_sdk_runtime_install_receipt_set_detail(UmiSdkRuntimeInstallReceipt *value,const char *detail)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_sdk_runtime_copy_text(value->detail,sizeof(value->detail),detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)value->revision+=1U;
    return s;
    }
#endif
UmiStatus umi_sdk_runtime_install_receipt_set_detail(UmiSdkRuntimeInstallReceipt *value,const char *detail)
{
    /* A text edit and its revision are one publication. Framework's shared
     * helper checks capacity before writing and supports text from this field
     * itself. Refused edits leave the complete previous record unchanged. */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}
/*
 * Return the number of records represented by sdk runtime install receipt set entry
 * without changing their state.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_install_receipt_set_entry_count(UmiSdkRuntimeInstallReceipt *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->entry_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_sdk_runtime_install_receipt_set_entry_count(UmiSdkRuntimeInstallReceipt *value,uint64_t n)
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
    value->entry_count=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime install receipt set generation operation used by this module and
 * its client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_install_receipt_set_generation(UmiSdkRuntimeInstallReceipt *value,uint64_t n)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->generation=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_sdk_runtime_install_receipt_set_generation(UmiSdkRuntimeInstallReceipt *value,uint64_t n)
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
    value->generation=n;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime install receipt set state operation used by this module and its
 * client applications.
 */
/* Check revision capacity before mutation so refused edits preserve the field and token. The former unchecked implementation remains for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_install_receipt_set_state(UmiSdkRuntimeInstallReceipt *value,UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||state>UMI_SDK_RUNTIME_STATE_MISSING)return UMI_STATUS_INVALID_ARGUMENT;
    value->state=state;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
#endif
UmiStatus umi_sdk_runtime_install_receipt_set_state(UmiSdkRuntimeInstallReceipt *value,UmiSdkRuntimeState state)
{
    /* Do not change a field when its observation token cannot advance.
     * Reusing an old revision could make a stale review appear current. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL||state>UMI_SDK_RUNTIME_STATE_MISSING)return UMI_STATUS_INVALID_ARGUMENT;
    value->state=state;
    value->revision+=1U;
    return UMI_STATUS_OK;
    }
/*
 * Provide the sdk runtime install receipt same identity operation used by this module and
 * its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_sdk_runtime_install_receipt_same_identity(const UmiSdkRuntimeInstallReceipt *left,const UmiSdkRuntimeInstallReceipt *right)
{
    return left!=NULL&&right!=NULL&&strcmp(left->id,right->id)==0;
    }
#endif
bool umi_sdk_runtime_install_receipt_same_identity(const UmiSdkRuntimeInstallReceipt *left, const UmiSdkRuntimeInstallReceipt *right)
{
    /* Treat missing terminators as invalid identities instead of reading into
     * adjacent fields. Other record state does not change identity equality. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/* A caller can reject an invalid install receipt identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_sdk_runtime_install_receipt_init_checked,
    UmiSdkRuntimeInstallReceipt, umi_sdk_runtime_install_receipt_init, umi_sdk_runtime_install_receipt_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x83d00929f1d57953);
    schema = (schema ^ (uint64_t)sizeof(((UmiSdkRuntimeInstallReceipt *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSdkRuntimeInstallReceipt *)0)->path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSdkRuntimeInstallReceipt *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiSdkRuntimeInstallReceipt *)0)->id) - 1U +
        8U + sizeof(((UmiSdkRuntimeInstallReceipt *)0)->path) - 1U +
        8U + sizeof(((UmiSdkRuntimeInstallReceipt *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiSdkRuntimeInstallReceipt *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->entry_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->generation);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiSdkRuntimeInstallReceipt *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->entry_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->generation = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->state = (UmiSdkRuntimeState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus ArchiveValidate(const UmiSdkRuntimeInstallReceipt *value)
{
    return umi_sdk_runtime_install_receipt_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_sdk_runtime_install_receipt_archive_encode, umi_sdk_runtime_install_receipt_archive_decode,
    UmiSdkRuntimeInstallReceipt, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
