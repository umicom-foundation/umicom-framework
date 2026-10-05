/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/kernel_boundary.c
 *
 * PURPOSE:
 *   Define explicit user/kernel/hypervisor ownership boundaries for Umicom OS reusable services.
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

#include "umicom/platform/cross_target/kernel_boundary.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct kernel boundary satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_kernel_boundary_validate(const UmiCtKernelBoundary*b){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (b == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(b->boundary_id, '\0', sizeof(b->boundary_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||!umi_ct_id_valid(b->boundary_id)||b->caller<UMI_CT_DOMAIN_USER||b->caller>UMI_CT_DOMAIN_FIRMWARE||b->callee<UMI_CT_DOMAIN_USER||b->callee>UMI_CT_DOMAIN_FIRMWARE)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->caller==UMI_CT_DOMAIN_USER&&b->callee==UMI_CT_DOMAIN_KERNEL&&!b->privileged)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtKernelBoundaryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x65f435203e6d8012);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtKernelBoundary *)0)->boundary_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtKernelBoundaryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtKernelBoundary *)0)->boundary_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtKernelBoundaryArchiveWrite(UmiArchiveWriter *writer, const UmiCtKernelBoundary *value)
{
    UmiArchiveWriteText(writer, value->boundary_id, sizeof(value->boundary_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->caller);
    UmiArchiveWriteSigned(writer, (int64_t)value->callee);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->copy_in);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->copy_out);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->privileged);
}
static void UmiCtKernelBoundaryArchiveRead(UmiArchiveReader *reader, UmiCtKernelBoundary *value)
{
    UmiArchiveReadText(reader, value->boundary_id, sizeof(value->boundary_id));
    value->caller = (UmiCtBoundaryDomain)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->callee = (UmiCtBoundaryDomain)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->copy_in = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->copy_out = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->privileged = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtKernelBoundaryArchiveValidate(const UmiCtKernelBoundary *value)
{
    return umi_ct_kernel_boundary_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_kernel_boundary_archive_encode, umi_ct_kernel_boundary_archive_decode,
    UmiCtKernelBoundary, UmiCtKernelBoundaryArchiveSchema, UmiCtKernelBoundaryArchiveBound, UmiCtKernelBoundaryArchiveWrite, UmiCtKernelBoundaryArchiveRead, UmiCtKernelBoundaryArchiveValidate)
