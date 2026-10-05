/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/event.c
 *
 * PURPOSE:
 *   Implement describe typed test-runtime events published after state changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/event.h"
#include "../base/value_archive_internal.h"
#include "../base/record_update_internal.h"
#include "umicom/base/text.h"
#include <string.h>

/*
 * Initialise test runtime event from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_runtime_event_init(UmiTestRuntimeEvent *value, const char *id)
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
 * Check that test runtime event satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_event_validate(const UmiTestRuntimeEvent *value)
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
 * Provide the test runtime event set name operation used by this module and its client
 * applications.
 */
/* The shared text edit publishes a field and revision together. The former copy could clear the field on refusal; it is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_event_set_name(UmiTestRuntimeEvent *value, const char *name)
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
UmiStatus umi_test_runtime_event_set_name(UmiTestRuntimeEvent *value, const char *name)
{
    /* Publish name and its revision together. Reusing the
     * Framework edit helper keeps an oversized or invalid value from clearing
     * the previous field while leaving its observation token unchanged. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->name, sizeof(value->name),
                           name, &value->revision);
}

/*
 * Provide the test runtime event set detail operation used by this module and its client
 * applications.
 */
/* The shared text edit publishes a field and revision together. The former copy could clear the field on refusal; it is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_event_set_detail(UmiTestRuntimeEvent *value, const char *detail)
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
UmiStatus umi_test_runtime_event_set_detail(UmiTestRuntimeEvent *value, const char *detail)
{
    /* Publish detail and its revision together. Reusing the
     * Framework edit helper keeps an oversized or invalid value from clearing
     * the previous field while leaving its observation token unchanged. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_update(value->detail, sizeof(value->detail),
                           detail, &value->revision);
}

/*
 * Provide the test runtime event set kind operation used by this module and its client
 * applications.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_event_set_kind(UmiTestRuntimeEvent *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->kind = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_runtime_event_set_kind(UmiTestRuntimeEvent *value, uint64_t number)
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
    value->kind = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime event set sequence operation used by this module and its client
 * applications.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_event_set_sequence(UmiTestRuntimeEvent *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->sequence = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_test_runtime_event_set_sequence(UmiTestRuntimeEvent *value, uint64_t number)
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
    value->sequence = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime event touch operation used by this module and its client
 * applications.
 */
/* Revision exhaustion is checked before changing the field. The former unchecked mutation is retained for engineering review. */
#if 0
UmiStatus umi_test_runtime_event_touch(UmiTestRuntimeEvent *value, uint64_t updated_at_ms)
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
UmiStatus umi_test_runtime_event_touch(UmiTestRuntimeEvent *value, uint64_t updated_at_ms)
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
 * Provide the test runtime event same identity operation used by this module and its
 * client applications.
 */
/* Bounded identity comparison replaces an unchecked string scan. The
 * previous comparison remains here for review of compatibility behavior. */
#if 0
bool umi_test_runtime_event_same_identity(const UmiTestRuntimeEvent *left, const UmiTestRuntimeEvent *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
#endif
bool umi_test_runtime_event_same_identity(const UmiTestRuntimeEvent *left, const UmiTestRuntimeEvent *right)
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
UMI_DEFINE_REVIEWED_RECORD_EDIT(umi_test_runtime_event_replace_if_current,
    UmiTestRuntimeEvent, umi_test_runtime_event_validate)

/* A caller can reject an invalid event identity without
 * erasing a previously accepted record. Defaults and domain validation stay
 * with this owner; Framework supplies the common staged publication boundary. */
UMI_DEFINE_CHECKED_RECORD_INIT(umi_test_runtime_event_init_checked,
    UmiTestRuntimeEvent, umi_test_runtime_event_init, umi_test_runtime_event_validate)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf7e1698612f16789);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeEvent *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeEvent *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestRuntimeEvent *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTestRuntimeEvent *)0)->id) - 1U +
        8U + sizeof(((UmiTestRuntimeEvent *)0)->name) - 1U +
        8U + sizeof(((UmiTestRuntimeEvent *)0)->detail) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiTestRuntimeEvent *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->updated_at_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiTestRuntimeEvent *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
    value->kind = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->updated_at_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus ArchiveValidate(const UmiTestRuntimeEvent *value)
{
    return umi_test_runtime_event_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_runtime_event_archive_encode, umi_test_runtime_event_archive_decode,
    UmiTestRuntimeEvent, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)
