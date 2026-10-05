/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/runtime_dependency.c
 *
 * PURPOSE:
 *   Implement describe one dynamic runtime dependency and where it was resolved.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/runtime_dependency.h"
#include "../base/value_archive_internal.h"
#include "../base/record_update_internal.h"
#include "umicom/base/text.h"
#include <string.h>

/*
 * Initialise test runtime runtime dependency from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_runtime_dependency_init(UmiTestRuntimeRuntimeDependency *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_test_runtime_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that test runtime runtime dependency satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_dependency_validate(const UmiTestRuntimeRuntimeDependency *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_test_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_test_runtime_text_is_valid(value->name, sizeof(value->name))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_test_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime runtime dependency set name operation used by this module and
 * its client applications.
 */
/* The shared text edit publishes a field and revision together. The former copy could clear the field on refusal; it is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_runtime_dependency_set_name(UmiTestRuntimeRuntimeDependency *value, const char *name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->name, sizeof(value->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}
#endif
UmiStatus umi_test_runtime_runtime_dependency_set_name(UmiTestRuntimeRuntimeDependency *value, const char *name)
{
    /* Publish name and its revision together. Reusing the
     * Framework edit helper keeps an oversized or invalid value from clearing
     * the previous field while leaving its observation token unchanged. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->name, sizeof(value->name),
                           name, &value->revision);
}

/*
 * Provide the test runtime runtime dependency set detail operation used by this module and
 * its client applications.
 */
/* The shared text edit publishes a field and revision together. The former copy could clear the field on refusal; it is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_runtime_dependency_set_detail(UmiTestRuntimeRuntimeDependency *value, const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}
#endif
UmiStatus umi_test_runtime_runtime_dependency_set_detail(UmiTestRuntimeRuntimeDependency *value, const char *detail)
{
    /* Publish detail and its revision together. Reusing the
     * Framework edit helper keeps an oversized or invalid value from clearing
     * the previous field while leaving its observation token unchanged. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}

/*
 * Provide the test runtime runtime dependency set required operation used by this module
 * and its client applications.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_runtime_dependency_set_required(UmiTestRuntimeRuntimeDependency *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->required = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_runtime_runtime_dependency_set_required(UmiTestRuntimeRuntimeDependency *value, uint64_t number)
{
    /* Keep observation tokens monotonic: changing a field after the last
     * usable revision would make an older observation appear current again. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->required = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime runtime dependency set resolved operation used by this module
 * and its client applications.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_runtime_dependency_set_resolved(UmiTestRuntimeRuntimeDependency *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->resolved = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_runtime_runtime_dependency_set_resolved(UmiTestRuntimeRuntimeDependency *value, uint64_t number)
{
    /* Keep observation tokens monotonic: changing a field after the last
     * usable revision would make an older observation appear current again. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->resolved = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime runtime dependency touch operation used by this module and its
 * client applications.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_runtime_dependency_touch(UmiTestRuntimeRuntimeDependency *value, uint64_t updated_at_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->updated_at_ms = updated_at_ms;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_runtime_runtime_dependency_touch(UmiTestRuntimeRuntimeDependency *value, uint64_t updated_at_ms)
{
    /* Keep observation tokens monotonic: changing a field after the last
     * usable revision would make an older observation appear current again. */
    if (value != NULL && value->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->updated_at_ms = updated_at_ms;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime runtime dependency same identity operation used by this module
 * and its client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_test_runtime_runtime_dependency_same_identity(const UmiTestRuntimeRuntimeDependency *left, const UmiTestRuntimeRuntimeDependency *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
#endif
bool umi_test_runtime_runtime_dependency_same_identity(const UmiTestRuntimeRuntimeDependency *left, const UmiTestRuntimeRuntimeDependency *right)
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
UMI_DEFINE_REVIEWED_RECORD_EDIT(umi_test_runtime_runtime_dependency_replace_if_current,
    UmiTestRuntimeRuntimeDependency, umi_test_runtime_runtime_dependency_validate)

/* A caller can reject an invalid runtime dependency identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_test_runtime_runtime_dependency_init_checked,
    UmiTestRuntimeRuntimeDependency, umi_test_runtime_runtime_dependency_init, umi_test_runtime_runtime_dependency_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x079a218366b20b95);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeRuntimeDependency *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeRuntimeDependency *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeRuntimeDependency *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTestRuntimeRuntimeDependency *)0)->id) - 1U +
        8U + sizeof(((UmiTestRuntimeRuntimeDependency *)0)->name) - 1U +
        8U + sizeof(((UmiTestRuntimeRuntimeDependency *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiTestRuntimeRuntimeDependency *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->resolved);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->updated_at_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiTestRuntimeRuntimeDependency *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->required = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->resolved = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->updated_at_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus ArchiveValidate(const UmiTestRuntimeRuntimeDependency *value)
{
    return umi_test_runtime_runtime_dependency_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_runtime_runtime_dependency_archive_encode, umi_test_runtime_runtime_dependency_archive_decode,
    UmiTestRuntimeRuntimeDependency, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
