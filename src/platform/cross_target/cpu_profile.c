/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cpu_profile.c
 *
 * PURPOSE:
 *   Describe deployable CPU profiles and evaluate runtime feature/XLEN compatibility.
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

#include "umicom/platform/cross_target/cpu_profile.h"
#include "../../base/value_archive_internal.h"

/* Check that ct cpu profile satisfies its contract before another service relies on it. */
UmiStatus umi_ct_cpu_profile_validate(const UmiCtCpuProfile*p){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (p == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(p->profile_id, '\0', sizeof(p->profile_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||!umi_ct_id_valid(p->profile_id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->xlen!=32U&&p->xlen!=64U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->minimum_cores==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((p->architecture==UMI_CT_ARCH_RISCV32&&p->xlen!=32U)||(p->architecture==UMI_CT_ARCH_RISCV64&&p->xlen!=64U))return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
/*
 * Provide the ct cpu profile matches operation used by this module and its client
 * applications.
 */
bool umi_ct_cpu_profile_matches(const UmiCtCpuProfile*p,UmiCtArchitecture a,uint32_t x,uint32_t c,const UmiCtCpuFeatureSet*f){return p!=NULL&&f!=NULL&&p->architecture==a&&p->xlen==x&&c>=p->minimum_cores&&umi_ct_cpu_feature_set_missing(f,&p->required)==0U;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtCpuProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe81accda892a6e40);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCpuProfile *)0)->profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtCpuProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtCpuProfile *)0)->profile_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtCpuProfileArchiveWrite(UmiArchiveWriter *writer, const UmiCtCpuProfile *value)
{
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->architecture);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->xlen);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required.bits);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_cores);
}
static void UmiCtCpuProfileArchiveRead(UmiArchiveReader *reader, UmiCtCpuProfile *value)
{
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    value->architecture = (UmiCtArchitecture)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->xlen = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->required.bits = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->minimum_cores = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiCtCpuProfileArchiveValidate(const UmiCtCpuProfile *value)
{
    return umi_ct_cpu_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_cpu_profile_archive_encode, umi_ct_cpu_profile_archive_decode,
    UmiCtCpuProfile, UmiCtCpuProfileArchiveSchema, UmiCtCpuProfileArchiveBound, UmiCtCpuProfileArchiveWrite, UmiCtCpuProfileArchiveRead, UmiCtCpuProfileArchiveValidate)
