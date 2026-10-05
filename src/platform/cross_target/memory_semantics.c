/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/memory_semantics.c
 *
 * PURPOSE:
 *   Describe virtual memory, guard page and executable mapping capabilities without embedding OS APIs in applications.
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

#include "umicom/platform/cross_target/memory_semantics.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct memory semantics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_memory_semantics_validate(const UmiCtMemorySemantics*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->page_size==0U||(s->page_size&(s->page_size-1U))!=0U||s->allocation_granularity<s->page_size)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
/*
 * Provide the ct memory round up operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_memory_round_up(const UmiCtMemorySemantics*s,uint64_t b){uint64_t g;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->allocation_granularity==0U)return b;g=s->allocation_granularity;return b==0U?0U:((b+g-1U)/g)*g;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtMemorySemanticsArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc021e6802048eff7);

    return schema;
}
static size_t UmiCtMemorySemanticsArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtMemorySemanticsArchiveWrite(UmiArchiveWriter *writer, const UmiCtMemorySemantics *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->page_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allocation_granularity);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->virtual_memory);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->guard_pages);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->executable_memory);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->huge_pages);
}
static void UmiCtMemorySemanticsArchiveRead(UmiArchiveReader *reader, UmiCtMemorySemantics *value)
{
    value->page_size = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->allocation_granularity = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->virtual_memory = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->guard_pages = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->executable_memory = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->huge_pages = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtMemorySemanticsArchiveValidate(const UmiCtMemorySemantics *value)
{
    return umi_ct_memory_semantics_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_memory_semantics_archive_encode, umi_ct_memory_semantics_archive_decode,
    UmiCtMemorySemantics, UmiCtMemorySemanticsArchiveSchema, UmiCtMemorySemanticsArchiveBound, UmiCtMemorySemanticsArchiveWrite, UmiCtMemorySemanticsArchiveRead, UmiCtMemorySemanticsArchiveValidate)
