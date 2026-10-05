/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/clock_semantics.c
 *
 * PURPOSE:
 *   Describe monotonic/wall-clock availability and timer resolution for scheduler and profiling portability.
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

#include "umicom/platform/cross_target/clock_semantics.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct clock semantics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_clock_semantics_validate(const UmiCtClockSemantics*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->frequency_hz==0U||s->resolution_ns==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!s->monotonic)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
/*
 * Provide the ct clock ticks to ns operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_clock_ticks_to_ns(const UmiCtClockSemantics*s,uint64_t t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->frequency_hz==0U)return 0U;return (t/ s->frequency_hz)*UINT64_C(1000000000)+(t % s->frequency_hz)*UINT64_C(1000000000)/s->frequency_hz;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtClockSemanticsArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9e718ea6b9a5b5b5);

    return schema;
}
static size_t UmiCtClockSemanticsArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtClockSemanticsArchiveWrite(UmiArchiveWriter *writer, const UmiCtClockSemantics *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->monotonic);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->wall_clock);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->high_resolution);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->frequency_hz);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->resolution_ns);
}
static void UmiCtClockSemanticsArchiveRead(UmiArchiveReader *reader, UmiCtClockSemantics *value)
{
    value->monotonic = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->wall_clock = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->high_resolution = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->frequency_hz = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->resolution_ns = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiCtClockSemanticsArchiveValidate(const UmiCtClockSemantics *value)
{
    return umi_ct_clock_semantics_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_clock_semantics_archive_encode, umi_ct_clock_semantics_archive_decode,
    UmiCtClockSemantics, UmiCtClockSemanticsArchiveSchema, UmiCtClockSemanticsArchiveBound, UmiCtClockSemanticsArchiveWrite, UmiCtClockSemanticsArchiveRead, UmiCtClockSemanticsArchiveValidate)
