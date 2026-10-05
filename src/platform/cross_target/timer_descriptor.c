/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/timer_descriptor.c
 *
 * PURPOSE:
 *   Describe platform timer frequency, width and monotonic/oneshot properties for scheduler portability.
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

#include "umicom/platform/cross_target/timer_descriptor.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct timer descriptor satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_timer_descriptor_validate(const UmiCtTimerDescriptor*d){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (d == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(d->timer_id, '\0', sizeof(d->timer_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||!umi_ct_id_valid(d->timer_id)||d->frequency_hz==0U||(d->counter_bits!=32U&&d->counter_bits!=64U)||!d->monotonic)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the ct timer ns to ticks operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_timer_ns_to_ticks(const UmiCtTimerDescriptor*d,uint64_t ns){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->frequency_hz==0U)return 0U;return (ns/UINT64_C(1000000000))*d->frequency_hz+(ns%UINT64_C(1000000000))*d->frequency_hz/UINT64_C(1000000000);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtTimerDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x10f1d6aabb8d838d);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtTimerDescriptor *)0)->timer_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtTimerDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtTimerDescriptor *)0)->timer_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtTimerDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiCtTimerDescriptor *value)
{
    UmiArchiveWriteText(writer, value->timer_id, sizeof(value->timer_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->frequency_hz);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->counter_bits);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->monotonic);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->oneshot);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->per_cpu);
}
static void UmiCtTimerDescriptorArchiveRead(UmiArchiveReader *reader, UmiCtTimerDescriptor *value)
{
    UmiArchiveReadText(reader, value->timer_id, sizeof(value->timer_id));
    value->frequency_hz = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->counter_bits = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->monotonic = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->oneshot = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->per_cpu = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtTimerDescriptorArchiveValidate(const UmiCtTimerDescriptor *value)
{
    return umi_ct_timer_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_timer_descriptor_archive_encode, umi_ct_timer_descriptor_archive_decode,
    UmiCtTimerDescriptor, UmiCtTimerDescriptorArchiveSchema, UmiCtTimerDescriptorArchiveBound, UmiCtTimerDescriptorArchiveWrite, UmiCtTimerDescriptorArchiveRead, UmiCtTimerDescriptorArchiveValidate)
