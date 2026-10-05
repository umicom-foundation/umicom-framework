/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/cross_build_contract.c
 *
 * PURPOSE:
 *   Declare cross-build requirements while leaving actual compiler/tool discovery to the existing Toolchain subsystem.
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

#include "umicom/platform/cross_target/cross_build_contract.h"
#include "../../base/value_archive_internal.h"

/*
 * Check that ct cross build contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ct_cross_build_contract_validate(const UmiCtCrossBuildContract*c){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (c == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(c->contract_id, '\0', sizeof(c->contract_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(c->target.triple, '\0', sizeof(c->target.triple)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(c->target.vendor, '\0', sizeof(c->target.vendor)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(c->required_toolchain_family, '\0', sizeof(c->required_toolchain_family)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(c->required_abi, '\0', sizeof(c->required_abi)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||!umi_ct_id_valid(c->contract_id)||c->target.architecture==UMI_CT_ARCH_UNKNOWN||c->required_toolchain_family[0]=='\0'||c->required_abi[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->target.operating_system==UMI_CT_OS_BARE_METAL&&!c->require_sysroot)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtCrossBuildContractArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x52c2535808ab2669);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossBuildContract *)0)->contract_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossBuildContract *)0)->target.triple)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossBuildContract *)0)->target.vendor)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossBuildContract *)0)->required_toolchain_family)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtCrossBuildContract *)0)->required_abi)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtCrossBuildContractArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtCrossBuildContract *)0)->contract_id) - 1U +
        8U +
        8U + sizeof(((UmiCtCrossBuildContract *)0)->target.triple) - 1U +
        8U + sizeof(((UmiCtCrossBuildContract *)0)->target.vendor) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiCtCrossBuildContract *)0)->required_toolchain_family) - 1U +
        8U + sizeof(((UmiCtCrossBuildContract *)0)->required_abi) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtCrossBuildContractArchiveWrite(UmiArchiveWriter *writer, const UmiCtCrossBuildContract *value)
{
    UmiArchiveWriteText(writer, value->contract_id, sizeof(value->contract_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target.api_version);
    UmiArchiveWriteText(writer, value->target.triple, sizeof(value->target.triple));
    UmiArchiveWriteText(writer, value->target.vendor, sizeof(value->target.vendor));
    UmiArchiveWriteSigned(writer, (int64_t)value->target.architecture);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.operating_system);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.environment);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target.pointer_bits);
    UmiArchiveWriteSigned(writer, (int64_t)value->target.endian);
    UmiArchiveWriteText(writer, value->required_toolchain_family, sizeof(value->required_toolchain_family));
    UmiArchiveWriteText(writer, value->required_abi, sizeof(value->required_abi));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_sysroot);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_emulator);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_debugger);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->require_assembly);
}
static void UmiCtCrossBuildContractArchiveRead(UmiArchiveReader *reader, UmiCtCrossBuildContract *value)
{
    UmiArchiveReadText(reader, value->contract_id, sizeof(value->contract_id));
    value->target.structure_size = (uint32_t)sizeof(value->target);
    value->target.api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->target.triple, sizeof(value->target.triple));
    UmiArchiveReadText(reader, value->target.vendor, sizeof(value->target.vendor));
    value->target.architecture = (UmiCtArchitecture)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->target.operating_system = (UmiCtOperatingSystem)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->target.environment = (UmiCtEnvironment)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->target.pointer_bits = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->target.endian = (UmiCtEndian)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->required_toolchain_family, sizeof(value->required_toolchain_family));
    UmiArchiveReadText(reader, value->required_abi, sizeof(value->required_abi));
    value->require_sysroot = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_emulator = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_debugger = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->require_assembly = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtCrossBuildContractArchiveValidate(const UmiCtCrossBuildContract *value)
{
    return umi_ct_cross_build_contract_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_cross_build_contract_archive_encode, umi_ct_cross_build_contract_archive_decode,
    UmiCtCrossBuildContract, UmiCtCrossBuildContractArchiveSchema, UmiCtCrossBuildContractArchiveBound, UmiCtCrossBuildContractArchiveWrite, UmiCtCrossBuildContractArchiveRead, UmiCtCrossBuildContractArchiveValidate)
