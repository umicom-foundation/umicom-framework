/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/architecture_descriptor.c
 *
 * PURPOSE:
 *   CPU architecture, ABI, pointer-width and endianness descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/architecture_descriptor.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr architecture descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_architecture_descriptor_init(UmiDrArchitectureDescriptor *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrArchitectureDescriptor){0}; value->architecture=UMI_DR_ARCH_X86_64; value->endian=UMI_DR_ENDIAN_LITTLE; value->pointer_bits=64U; } }
/*
 * Check that dr architecture descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_dr_architecture_descriptor_valid(const UmiDrArchitectureDescriptor *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->abi, '\0', sizeof(value->abi)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && (value->pointer_bits==32U || value->pointer_bits==64U) && value->abi[0] != '\0'); }
/*
 * Provide the dr architecture descriptor fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_architecture_descriptor_fingerprint(const UmiDrArchitectureDescriptor *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_architecture_descriptor_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrArchitectureDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x925c2a54fbf9ba68);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrArchitectureDescriptor *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrArchitectureDescriptor *)0)->abi)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrArchitectureDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrArchitectureDescriptor *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiDrArchitectureDescriptor *)0)->abi) - 1U;
}
static void UmiDrArchitectureDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiDrArchitectureDescriptor *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->architecture);
    UmiArchiveWriteSigned(writer, (int64_t)value->endian);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->pointer_bits);
    UmiArchiveWriteText(writer, value->abi, sizeof(value->abi));
}
static void UmiDrArchitectureDescriptorArchiveRead(UmiArchiveReader *reader, UmiDrArchitectureDescriptor *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->architecture = (UmiDrArchitecture)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->endian = (UmiDrEndian)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->pointer_bits = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->abi, sizeof(value->abi));
}
static UmiStatus UmiDrArchitectureDescriptorArchiveValidate(const UmiDrArchitectureDescriptor *value)
{
    return umi_dr_architecture_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_architecture_descriptor_archive_encode, umi_dr_architecture_descriptor_archive_decode,
    UmiDrArchitectureDescriptor, UmiDrArchitectureDescriptorArchiveSchema, UmiDrArchitectureDescriptorArchiveBound, UmiDrArchitectureDescriptorArchiveWrite, UmiDrArchitectureDescriptorArchiveRead, UmiDrArchitectureDescriptorArchiveValidate)
