/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cross_target_snapshot.c
 *
 * PURPOSE:
 *   Aggregate immutable cross-target platform evidence for diagnostics, release gates and remote execution.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/cross_target_snapshot.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct cross target snapshot satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ct_cross_target_snapshot_validate(const UmiCtCrossTargetSnapshot*s){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (s == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(s->target.triple, '\0', sizeof(s->target.triple)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(s->target.vendor, '\0', sizeof(s->target.vendor)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(s->abi, '\0', sizeof(s->abi)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->target.architecture==UMI_CT_ARCH_UNKNOWN||s->abi[0]=='\0'||s->cpu_count==0U||s->page_size==0U||s->fingerprint==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->health.health==UMI_CT_HEALTH_BLOCKED)return UMI_STATUS_UNAVAILABLE;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtCrossTargetSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x781d1864ecfc6a8b);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossTargetSnapshot *)0)->target.triple)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossTargetSnapshot *)0)->target.vendor)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossTargetSnapshot *)0)->abi)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtCrossTargetSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiCtCrossTargetSnapshot *)0)->target.triple) - 1U +
        8U + sizeof(((UmiCtCrossTargetSnapshot *)0)->target.vendor) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiCtCrossTargetSnapshot *)0)->abi) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtCrossTargetSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiCtCrossTargetSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target.api_version);
    UmiArchiveWriteText(writer, value->target.triple, sizeof(value->target.triple));
    UmiArchiveWriteText(writer, value->target.vendor, sizeof(value->target.vendor));
    UmiArchiveWriteSigned(writer, (int64_t)value->target.architecture);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.operating_system);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.environment);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target.pointer_bits);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.endian);
    UmiArchiveWriteText(writer, value->abi, sizeof(value->abi));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cpu_features);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cpu_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->memory_bytes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->page_size);
    UmiArchiveWriteSigned(writer, (int64_t)value->health.health);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->health.blockers);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->health.warnings);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->health.readiness_percent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiCtCrossTargetSnapshotArchiveRead(UmiArchiveReader *reader, UmiCtCrossTargetSnapshot *value)
{
    value->target.structure_size = (uint32_t)sizeof(value->target);
    value->target.api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->target.triple, sizeof(value->target.triple));
    UmiArchiveReadText(reader, value->target.vendor, sizeof(value->target.vendor));
    value->target.architecture = (UmiCtArchitecture)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->target.operating_system = (UmiCtOperatingSystem)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->target.environment = (UmiCtEnvironment)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->target.pointer_bits = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->target.endian = (UmiCtEndian)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->abi, sizeof(value->abi));
    value->cpu_features = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->cpu_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->memory_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->page_size = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->health.health = (UmiCtHealth)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->health.blockers = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->health.warnings = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->health.readiness_percent = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiCtCrossTargetSnapshotArchiveValidate(const UmiCtCrossTargetSnapshot *value)
{
    return umi_ct_cross_target_snapshot_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_cross_target_snapshot_archive_encode, umi_ct_cross_target_snapshot_archive_decode,
    UmiCtCrossTargetSnapshot, UmiCtCrossTargetSnapshotArchiveSchema, UmiCtCrossTargetSnapshotArchiveBound, UmiCtCrossTargetSnapshotArchiveWrite, UmiCtCrossTargetSnapshotArchiveRead, UmiCtCrossTargetSnapshotArchiveValidate)
