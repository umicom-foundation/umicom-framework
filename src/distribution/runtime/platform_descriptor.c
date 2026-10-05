/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/platform_descriptor.c
 *
 * PURPOSE:
 *   runtime operating-system descriptors and minimum platform revision requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/platform_descriptor.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr platform descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_platform_descriptor_init(UmiDrPlatformDescriptor *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrPlatformDescriptor){0}; value->platform=UMI_DR_PLATFORM_LINUX; value->desktop=true; } }
/*
 * Check that dr platform descriptor satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_platform_descriptor_valid(const UmiDrPlatformDescriptor *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->platform >= UMI_DR_PLATFORM_WINDOWS && value->platform <= UMI_DR_PLATFORM_UMICOM_OS); }
/*
 * Provide the dr platform descriptor fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_platform_descriptor_fingerprint(const UmiDrPlatformDescriptor *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_platform_descriptor_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrPlatformDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x12e4af01e79e5792);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrPlatformDescriptor *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrPlatformDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrPlatformDescriptor *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrPlatformDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiDrPlatformDescriptor *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->platform);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->desktop);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->server);
}
static void UmiDrPlatformDescriptorArchiveRead(UmiArchiveReader *reader, UmiDrPlatformDescriptor *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->platform = (UmiDrPlatform)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->minimum_version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->desktop = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->server = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrPlatformDescriptorArchiveValidate(const UmiDrPlatformDescriptor *value)
{
    return umi_dr_platform_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_platform_descriptor_archive_encode, umi_dr_platform_descriptor_archive_decode,
    UmiDrPlatformDescriptor, UmiDrPlatformDescriptorArchiveSchema, UmiDrPlatformDescriptorArchiveBound, UmiDrPlatformDescriptorArchiveWrite, UmiDrPlatformDescriptorArchiveRead, UmiDrPlatformDescriptorArchiveValidate)
