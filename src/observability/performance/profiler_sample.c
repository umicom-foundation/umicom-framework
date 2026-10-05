/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/performance/profiler_sample.c
 *
 * PURPOSE:
 *   Implement represent and evaluate reusable CPU/process profiling state for profiler sample.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "../../base/value_archive_internal.h"
#include "umicom/observability/performance/profiler_sample.h"

/* Initialise deterministic record metadata before any measurement is observed. */
UmiStatus umi_performance_profiler_sample_init(UmiPerformanceProfilerSample *record, const char *id, const char *subject_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || id == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->api_version = UMI_PERFORMANCE_API_VERSION;
    record->state = UMI_PERFORMANCE_STATE_IDLE;
    record->severity = UMI_PERFORMANCE_SEVERITY_INFO;
    record->enabled = true;
    status = umi_performance_copy_text(record->id, sizeof(record->id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_performance_copy_text(record->subject_id, sizeof(record->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { record->id[0] = '\0'; return status; }
    return UMI_STATUS_OK;
}

/* Reject incompatible ABI snapshots and malformed stable identifiers. */
UmiStatus umi_performance_profiler_sample_validate(const UmiPerformanceProfilerSample *record) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->id, '\0', sizeof(record->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (record->structure_size != (uint32_t)sizeof(*record) || record->api_version != UMI_PERFORMANCE_API_VERSION) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_performance_id_valid(record->id) || !umi_performance_id_valid(record->subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((unsigned)record->state > (unsigned)UMI_PERFORMANCE_STATE_FAILED || (unsigned)record->severity > (unsigned)UMI_PERFORMANCE_SEVERITY_CRITICAL) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Store scalar evidence without silently changing the caller-selected lifecycle state. */
UmiStatus umi_performance_profiler_sample_observe(UmiPerformanceProfilerSample *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns) {
    UmiStatus status = umi_performance_profiler_sample_validate(record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->value = value;
    record->auxiliary = auxiliary;
    record->count = count;
    record->timestamp_ns = timestamp_ns;
    ++record->sequence;
    return UMI_STATUS_OK;
}

/* Stable identity is intentionally independent of sequence, severity and measured values. */
bool umi_performance_profiler_sample_same_identity(const UmiPerformanceProfilerSample *left, const UmiPerformanceProfilerSample *right) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0 && strcmp(left->subject_id, right->subject_id) == 0;
}

/* Report whether an observation strictly exceeds its threshold. */
bool umi_performance_profiler_sample_exceeds(double value, double threshold) {
    return value > threshold;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPerformanceProfilerSampleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd9c4875efc7ed1fb);
    schema = (schema ^ (uint64_t)sizeof(((UmiPerformanceProfilerSample *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPerformanceProfilerSample *)0)->subject_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPerformanceProfilerSampleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiPerformanceProfilerSample *)0)->id) - 1U +
        8U + sizeof(((UmiPerformanceProfilerSample *)0)->subject_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPerformanceProfilerSampleArchiveWrite(UmiArchiveWriter *writer, const UmiPerformanceProfilerSample *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ns);
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteDouble(writer, value->auxiliary);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiPerformanceProfilerSampleArchiveRead(UmiArchiveReader *reader, UmiPerformanceProfilerSample *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    value->state = (UmiPerformanceState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->severity = (UmiPerformanceSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->timestamp_ns = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->value = UmiArchiveReadDouble(reader);
    value->auxiliary = UmiArchiveReadDouble(reader);
    value->count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiPerformanceProfilerSampleArchiveValidate(const UmiPerformanceProfilerSample *value)
{
    return umi_performance_profiler_sample_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_performance_profiler_sample_archive_encode, umi_performance_profiler_sample_archive_decode,
    UmiPerformanceProfilerSample, UmiPerformanceProfilerSampleArchiveSchema, UmiPerformanceProfilerSampleArchiveBound, UmiPerformanceProfilerSampleArchiveWrite, UmiPerformanceProfilerSampleArchiveRead, UmiPerformanceProfilerSampleArchiveValidate)
