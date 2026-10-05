/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/scheduler_profile.c
 *
 * PURPOSE:
 *   Describe portable scheduler policy inputs for Umicom OS and runtime conformance.
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

#include "umicom/platform/cross_target/scheduler_profile.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct scheduler profile satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_scheduler_profile_validate(const UmiCtSchedulerProfile*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->scheduler_class<UMI_CT_SCHED_COOPERATIVE||p->scheduler_class>UMI_CT_SCHED_REALTIME||p->cpu_count==0U||p->priority_levels==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->scheduler_class!=UMI_CT_SCHED_COOPERATIVE&&p->timeslice_us==0U)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtSchedulerProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbf40ab89f2364c19);

    return schema;
}
static size_t UmiCtSchedulerProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtSchedulerProfileArchiveWrite(UmiArchiveWriter *writer, const UmiCtSchedulerProfile *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->scheduler_class);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cpu_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timeslice_us);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority_levels);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->affinity);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->load_balancing);
}
static void UmiCtSchedulerProfileArchiveRead(UmiArchiveReader *reader, UmiCtSchedulerProfile *value)
{
    value->scheduler_class = (UmiCtSchedulerClass)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->cpu_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->timeslice_us = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->priority_levels = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->affinity = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->load_balancing = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtSchedulerProfileArchiveValidate(const UmiCtSchedulerProfile *value)
{
    return umi_ct_scheduler_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_scheduler_profile_archive_encode, umi_ct_scheduler_profile_archive_decode,
    UmiCtSchedulerProfile, UmiCtSchedulerProfileArchiveSchema, UmiCtSchedulerProfileArchiveBound, UmiCtSchedulerProfileArchiveWrite, UmiCtSchedulerProfileArchiveRead, UmiCtSchedulerProfileArchiveValidate)
