/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/package_evidence.c
 *
 * PURPOSE:
 *   Implement aggregate validation evidence for one sdk/runtime package.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/package_evidence.h"
#include "../base/value_archive_internal.h"
#include "../base/record_update_internal.h"
#include "umicom/base/text.h"
#include <string.h>
/*
 * Initialise sdk runtime package evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_evidence_init(UmiSdkRuntimePackageEvidence *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->state = UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_sdk_runtime_copy_text(value->id, sizeof(value->id), id);
}
/*
 * Check that sdk runtime package evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_package_evidence_validate(const UmiSdkRuntimePackageEvidence *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->path, sizeof(value->path))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime package evidence set path operation used by this module and its
 * client applications.
 */
/* The shared text edit publishes a field and revision together. The former copy could clear the field on refusal; it is retained for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_package_evidence_set_path(UmiSdkRuntimePackageEvidence *value, const char *path)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
#endif
UmiStatus umi_sdk_runtime_package_evidence_set_path(UmiSdkRuntimePackageEvidence *value, const char *path)
{
    /* Publish path and its revision together. Reusing the
     * Framework edit helper keeps an oversized or invalid value from clearing
     * the previous field while leaving its observation token unchanged. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->path, sizeof(value->path),
                           path, &value->revision);
}
/*
 * Provide the sdk runtime package evidence set detail operation used by this module and
 * its client applications.
 */
/* The shared text edit publishes a field and revision together. The former copy could clear the field on refusal; it is retained for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_package_evidence_set_detail(UmiSdkRuntimePackageEvidence *value, const char *detail)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
#endif
UmiStatus umi_sdk_runtime_package_evidence_set_detail(UmiSdkRuntimePackageEvidence *value, const char *detail)
{
    /* Publish detail and its revision together. Reusing the
     * Framework edit helper keeps an oversized or invalid value from clearing
     * the previous field while leaving its observation token unchanged. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}
/*
 * Return the number of records represented by sdk runtime package evidence set passed
 * without changing their state.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_package_evidence_set_passed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->passed_count = number; value->revision += 1U; return UMI_STATUS_OK; }
#endif
UmiStatus umi_sdk_runtime_package_evidence_set_passed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number)
{
    /* Keep observation tokens monotonic: changing a field after the last
     * usable revision would make an older observation appear current again. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
 /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->passed_count = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Return the number of records represented by sdk runtime package evidence set failed
 * without changing their state.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_package_evidence_set_failed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->failed_count = number; value->revision += 1U; return UMI_STATUS_OK; }
#endif
UmiStatus umi_sdk_runtime_package_evidence_set_failed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number)
{
    /* Keep observation tokens monotonic: changing a field after the last
     * usable revision would make an older observation appear current again. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
 /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->failed_count = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime package evidence set state operation used by this module and its
 * client applications.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_sdk_runtime_package_evidence_set_state(UmiSdkRuntimePackageEvidence *value, UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
#endif
UmiStatus umi_sdk_runtime_package_evidence_set_state(UmiSdkRuntimePackageEvidence *value, UmiSdkRuntimeState state)
{
    /* Keep observation tokens monotonic: changing a field after the last
     * usable revision would make an older observation appear current again. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime package evidence same identity operation used by this module and
 * its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_sdk_runtime_package_evidence_same_identity(const UmiSdkRuntimePackageEvidence *left, const UmiSdkRuntimePackageEvidence *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
#endif
bool umi_sdk_runtime_package_evidence_same_identity(const UmiSdkRuntimePackageEvidence *left, const UmiSdkRuntimePackageEvidence *right)
{
    /* Treat missing terminators as invalid identities instead of reading into
     * adjacent fields. Other record state does not change identity equality. */
    return left != NULL && right != NULL &&
        UmiRecordTextFits(left->id, sizeof(left->id)) &&
        UmiRecordTextFits(right->id, sizeof(right->id)) &&
        strcmp(left->id, right->id) == 0;
}

/* Prepare related fields on a caller-owned copy, then publish them together.
 * Reusing the Framework guard keeps a delayed review from overwriting newer
 * state and leaves this model's validation rules with its existing validator. */
UMI_DEFINE_REVIEWED_RECORD_EDIT(umi_sdk_runtime_package_evidence_replace_if_current,
    UmiSdkRuntimePackageEvidence, umi_sdk_runtime_package_evidence_validate)

/* A caller can reject an invalid package evidence identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_sdk_runtime_package_evidence_init_checked,
    UmiSdkRuntimePackageEvidence, umi_sdk_runtime_package_evidence_init, umi_sdk_runtime_package_evidence_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7ee492a2a3d952e5);
    schema = (schema ^ (uint64_t)sizeof(((UmiSdkRuntimePackageEvidence *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSdkRuntimePackageEvidence *)0)->path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiSdkRuntimePackageEvidence *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiSdkRuntimePackageEvidence *)0)->id) - 1U +
        8U + sizeof(((UmiSdkRuntimePackageEvidence *)0)->path) - 1U +
        8U + sizeof(((UmiSdkRuntimePackageEvidence *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiSdkRuntimePackageEvidence *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->passed_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failed_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiSdkRuntimePackageEvidence *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->passed_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failed_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->state = (UmiSdkRuntimeState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus ArchiveValidate(const UmiSdkRuntimePackageEvidence *value)
{
    return umi_sdk_runtime_package_evidence_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_sdk_runtime_package_evidence_archive_encode, umi_sdk_runtime_package_evidence_archive_decode,
    UmiSdkRuntimePackageEvidence, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
