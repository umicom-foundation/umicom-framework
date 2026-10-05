/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/abi_descriptor.c
 *
 * PURPOSE:
 *   Describe data model, calling convention, stack alignment and floating-point ABI properties for cross-target compatibility checks.
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

#include "umicom/platform/cross_target/abi_descriptor.h"
#include "../../base/value_archive_internal.h"

/* Check that ct abi descriptor satisfies its contract before another service relies on it. */
UmiStatus umi_ct_abi_descriptor_validate(const UmiCtAbiDescriptor*d){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (d == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(d->abi_id, '\0', sizeof(d->abi_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->abi_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->data_model<UMI_CT_DATA_ILP32||d->data_model>UMI_CT_DATA_LLP64)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->pointer_bits!=32U&&d->pointer_bits!=64U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->stack_alignment==0U||(d->stack_alignment&(d->stack_alignment-1U))!=0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d->data_model==UMI_CT_DATA_ILP32&&d->pointer_bits!=32U)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if((d->data_model==UMI_CT_DATA_LP64||d->data_model==UMI_CT_DATA_LLP64)&&d->pointer_bits!=64U)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCtAbiDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7d7631c536e5112d);
    schema = (schema ^ (uint64_t)sizeof(((UmiCtAbiDescriptor *)0)->abi_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCtAbiDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCtAbiDescriptor *)0)->abi_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCtAbiDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiCtAbiDescriptor *value)
{
    UmiArchiveWriteText(writer, value->abi_id, sizeof(value->abi_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->data_model);
    UmiArchiveWriteSigned(writer, (int64_t)value->calling_convention);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->pointer_bits);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->stack_alignment);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->long_double_bits);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->hard_float);
}
static void UmiCtAbiDescriptorArchiveRead(UmiArchiveReader *reader, UmiCtAbiDescriptor *value)
{
    UmiArchiveReadText(reader, value->abi_id, sizeof(value->abi_id));
    value->data_model = (UmiCtDataModel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->calling_convention = (UmiCtCallingConvention)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->pointer_bits = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->stack_alignment = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->long_double_bits = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->hard_float = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCtAbiDescriptorArchiveValidate(const UmiCtAbiDescriptor *value)
{
    return umi_ct_abi_descriptor_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ct_abi_descriptor_archive_encode, umi_ct_abi_descriptor_archive_decode,
    UmiCtAbiDescriptor, UmiCtAbiDescriptorArchiveSchema, UmiCtAbiDescriptorArchiveBound, UmiCtAbiDescriptorArchiveWrite, UmiCtAbiDescriptorArchiveRead, UmiCtAbiDescriptorArchiveValidate)
