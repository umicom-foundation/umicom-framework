/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/replica_descriptor.c
 *
 * PURPOSE:
 *   Describe a Data Server replica endpoint, health and role without embedding network transport ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/conditional_archive_internal.h"
#include "umicom/data/enterprise/replica_descriptor.h"
#include "../../base/value_construction_internal.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_data_replica_descriptor_init(UmiDataReplicaDescriptor *item, const char *replica_id, const char *endpoint, uint32_t priority, bool primary) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->replica_id,sizeof(item->replica_id),replica_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->endpoint,sizeof(item->endpoint),endpoint);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->priority=priority;item->primary=primary;item->healthy=true;item->writable=primary;
    return umi_data_replica_descriptor_validate(item);
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_data_replica_descriptor_init_build_value(UmiDataReplicaDescriptor *item, const char *replica_id, const char *endpoint, uint32_t priority, bool primary) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->replica_id,sizeof(item->replica_id),replica_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->endpoint,sizeof(item->endpoint),endpoint);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->priority=priority;item->primary=primary;item->healthy=true;item->writable=primary;
    return umi_data_replica_descriptor_validate(item);
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_data_replica_descriptor_init, UmiDataReplicaDescriptor, item,
    (UmiDataReplicaDescriptor *item, const char *replica_id, const char *endpoint, uint32_t priority, bool primary),
    umi_data_replica_descriptor_init_build_value(candidate, replica_id, endpoint, priority, primary))


/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_replica_descriptor_validate(const UmiDataReplicaDescriptor *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->replica_id, '\0', sizeof(item->replica_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->endpoint, '\0', sizeof(item->endpoint)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->replica_id[0] != '\0' && item->endpoint[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataReplicaDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x94a6ca7401e94564);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataReplicaDescriptor *)0)->replica_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataReplicaDescriptor *)0)->endpoint)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataReplicaDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataReplicaDescriptor *)0)->replica_id) - 1U +
        8U + sizeof(((UmiDataReplicaDescriptor *)0)->endpoint) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataReplicaDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiDataReplicaDescriptor *value)
{
    UmiArchiveWriteText(writer, value->replica_id, sizeof(value->replica_id));
    UmiArchiveWriteText(writer, value->endpoint, sizeof(value->endpoint));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->primary);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->healthy);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->writable);
}
static void UmiDataReplicaDescriptorArchiveRead(UmiArchiveReader *reader, UmiDataReplicaDescriptor *value)
{
    UmiArchiveReadText(reader, value->replica_id, sizeof(value->replica_id));
    UmiArchiveReadText(reader, value->endpoint, sizeof(value->endpoint));
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->primary = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->healthy = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->writable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataReplicaDescriptorArchiveValidate(const UmiDataReplicaDescriptor *value)
{
    return umi_data_replica_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_replica_descriptor_archive_encode, umi_data_replica_descriptor_archive_decode,
    UmiDataReplicaDescriptor, UmiDataReplicaDescriptorArchiveSchema, UmiDataReplicaDescriptorArchiveBound, UmiDataReplicaDescriptorArchiveWrite, UmiDataReplicaDescriptorArchiveRead, UmiDataReplicaDescriptorArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_data_replica_descriptor_archive_decode_with_workspace, UmiDataReplicaDescriptor,
    UmiDataReplicaDescriptorArchiveSchema, UmiDataReplicaDescriptorArchiveBound, UmiDataReplicaDescriptorArchiveRead, UmiDataReplicaDescriptorArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_data_replica_descriptor_archive_decode_if_unchanged, umi_data_replica_descriptor_archive_encode,
    UmiDataReplicaDescriptor, UmiDataReplicaDescriptorArchiveSchema, UmiDataReplicaDescriptorArchiveBound, UmiDataReplicaDescriptorArchiveRead, UmiDataReplicaDescriptorArchiveValidate)
