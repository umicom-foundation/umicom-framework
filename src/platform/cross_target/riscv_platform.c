/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_platform.c
 *
 * PURPOSE:
 *   Describe RISC-V board/virtual-machine platform capabilities used by boot and device planning.
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

#include "umicom/platform/cross_target/riscv_platform.h"
#include "../../base/value_archive_internal.h"

/* Check that ct riscv platform satisfies its contract before another service relies on it. */
UmiStatus umi_ct_riscv_platform_validate(const UmiCtRiscvPlatform*p){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (p == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(p->platform_id, '\0', sizeof(p->platform_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||!umi_ct_id_valid(p->platform_id)||p->cpu_count==0U||p->memory_bytes<UINT64_C(16)*1024U*1024U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->machine==UMI_CT_RISCV_MACHINE_QEMU_VIRT&&(!p->plic||!p->clint))return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtRiscvPlatformArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x82f553f3dfbf08b7);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtRiscvPlatform *)0)->platform_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtRiscvPlatformArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtRiscvPlatform *)0)->platform_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtRiscvPlatformArchiveWrite(UmiArchiveWriter *writer, const UmiCtRiscvPlatform *value)
{
    UmiArchiveWriteText(writer, value->platform_id, sizeof(value->platform_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->machine);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->memory_bytes);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cpu_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->plic);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->clint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->pci);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->virtio);
}
static void UmiCtRiscvPlatformArchiveRead(UmiArchiveReader *reader, UmiCtRiscvPlatform *value)
{
    UmiArchiveReadText(reader, value->platform_id, sizeof(value->platform_id));
    value->machine = (UmiCtRiscvMachine)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->memory_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->cpu_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->plic = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->clint = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->pci = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->virtio = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtRiscvPlatformArchiveValidate(const UmiCtRiscvPlatform *value)
{
    return umi_ct_riscv_platform_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_riscv_platform_archive_encode, umi_ct_riscv_platform_archive_decode,
    UmiCtRiscvPlatform, UmiCtRiscvPlatformArchiveSchema, UmiCtRiscvPlatformArchiveBound, UmiCtRiscvPlatformArchiveWrite, UmiCtRiscvPlatformArchiveRead, UmiCtRiscvPlatformArchiveValidate)
