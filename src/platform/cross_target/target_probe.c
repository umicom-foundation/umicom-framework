/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_probe.c
 *
 * PURPOSE:
 *   Record host/target probe evidence without hard-coding OS-specific probing in application repositories.
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

#include "umicom/platform/cross_target/target_probe.h"
#include "../../base/value_archive_internal.h"

/* Check that ct target probe satisfies its contract before another service relies on it. */
UmiStatus umi_ct_target_probe_validate(const UmiCtTargetProbe*p){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (p == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(p->target.triple, '\0', sizeof(p->target.triple)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(p->target.vendor, '\0', sizeof(p->target.vendor)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->target.architecture==UMI_CT_ARCH_UNKNOWN||p->cpu_count==0U||p->page_size==0U||p->confidence>100U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the ct target probe score operation used by this module and its client
 * applications.
 */
uint8_t umi_ct_target_probe_score(const UmiCtTargetProbe*p,const UmiCtTarget*e){unsigned s=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||e==NULL)return 0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->target.architecture==e->architecture)s+=35U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->target.operating_system==e->operating_system)s+=35U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->target.environment==e->environment)s+=20U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->target.pointer_bits==e->pointer_bits)s+=10U;return (uint8_t)((s*(unsigned)p->confidence)/100U);}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtTargetProbeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd721a9630d95def9);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtTargetProbe *)0)->target.triple)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtTargetProbe *)0)->target.vendor)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtTargetProbeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiCtTargetProbe *)0)->target.triple) - 1U +
        8U + sizeof(((UmiCtTargetProbe *)0)->target.vendor) - 1U +
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
static void UmiCtTargetProbeArchiveWrite(UmiArchiveWriter *writer, const UmiCtTargetProbe *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target.api_version);
    UmiArchiveWriteText(writer, value->target.triple, sizeof(value->target.triple));
    UmiArchiveWriteText(writer, value->target.vendor, sizeof(value->target.vendor));
    UmiArchiveWriteSigned(writer, (int64_t)value->target.architecture);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.operating_system);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.environment);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target.pointer_bits);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.endian);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cpu_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->memory_bytes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->page_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cpu_features);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->confidence);
}
static void UmiCtTargetProbeArchiveRead(UmiArchiveReader *reader, UmiCtTargetProbe *value)
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
    value->cpu_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->memory_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->page_size = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->cpu_features = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->confidence = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
}
static UmiStatus UmiCtTargetProbeArchiveValidate(const UmiCtTargetProbe *value)
{
    return umi_ct_target_probe_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_target_probe_archive_encode, umi_ct_target_probe_archive_decode,
    UmiCtTargetProbe, UmiCtTargetProbeArchiveSchema, UmiCtTargetProbeArchiveBound, UmiCtTargetProbeArchiveWrite, UmiCtTargetProbeArchiveRead, UmiCtTargetProbeArchiveValidate)
