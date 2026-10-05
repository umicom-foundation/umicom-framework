/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/page_policy.c
 *
 * PURPOSE:
 *   Provide target page alignment and page-count calculations for virtual-memory and boot mapping plans.
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

#include "umicom/platform/cross_target/page_policy.h"
#include "../../base/value_archive_internal.h"

/* Check that ct page policy satisfies its contract before another service relies on it. */
UmiStatus umi_ct_page_policy_validate(const UmiCtPagePolicy*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||p->base_page_size<4096U||(p->base_page_size&(p->base_page_size-1U))!=0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->huge_pages&&(p->huge_page_size<p->base_page_size||(p->huge_page_size&p->base_page_size)!=0U))return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/* Provide the ct page align up operation used by this module and its client applications. */
uint64_t umi_ct_page_align_up(const UmiCtPagePolicy*p,uint64_t a){uint64_t s=p?p->base_page_size:1U;return ((a+s-1U)/s)*s;}
/* Return the number of records represented by ct page without changing their state. */
uint64_t umi_ct_page_count(const UmiCtPagePolicy*p,uint64_t b){uint64_t s=p?p->base_page_size:1U;return b==0U?0U:(b+s-1U)/s;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtPagePolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfaa95cfe0dc64991);

    return schema;
}
static size_t UmiCtPagePolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtPagePolicyArchiveWrite(UmiArchiveWriter *writer, const UmiCtPagePolicy *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base_page_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->huge_page_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->huge_pages);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->execute_never);
}
static void UmiCtPagePolicyArchiveRead(UmiArchiveReader *reader, UmiCtPagePolicy *value)
{
    value->base_page_size = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->huge_page_size = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->huge_pages = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->execute_never = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtPagePolicyArchiveValidate(const UmiCtPagePolicy *value)
{
    return umi_ct_page_policy_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_page_policy_archive_encode, umi_ct_page_policy_archive_decode,
    UmiCtPagePolicy, UmiCtPagePolicyArchiveSchema, UmiCtPagePolicyArchiveBound, UmiCtPagePolicyArchiveWrite, UmiCtPagePolicyArchiveRead, UmiCtPagePolicyArchiveValidate)
