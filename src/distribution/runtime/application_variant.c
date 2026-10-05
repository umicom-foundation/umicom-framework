/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_variant.c
 *
 * PURPOSE:
 *   platform-specific application variants without moving reusable logic into products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_variant.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr application variant from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_variant_init(UmiDrApplicationVariant *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrApplicationVariant){0}; value->platform=UMI_DR_PLATFORM_WINDOWS; value->architecture=UMI_DR_ARCH_X86_64; value->preferred_format=UMI_DR_PACKAGE_ZIP; } }
/*
 * Check that dr application variant satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_variant_valid(const UmiDrApplicationVariant *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->entrypoint, '\0', sizeof(value->entrypoint)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->platform != 0 && value->architecture != 0 && value->entrypoint[0] != '\0'); }
/*
 * Provide the dr application variant fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_variant_fingerprint(const UmiDrApplicationVariant *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_application_variant_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrApplicationVariantArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa7e6891e81d3e457);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationVariant *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationVariant *)0)->entrypoint)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrApplicationVariantArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrApplicationVariant *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiDrApplicationVariant *)0)->entrypoint) - 1U;
}
static void UmiDrApplicationVariantArchiveWrite(UmiArchiveWriter *writer, const UmiDrApplicationVariant *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->platform);
    UmiArchiveWriteSigned(writer, (int64_t)value->architecture);
    UmiArchiveWriteSigned(writer, (int64_t)value->preferred_format);
    UmiArchiveWriteText(writer, value->entrypoint, sizeof(value->entrypoint));
}
static void UmiDrApplicationVariantArchiveRead(UmiArchiveReader *reader, UmiDrApplicationVariant *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->platform = (UmiDrPlatform)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->architecture = (UmiDrArchitecture)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->preferred_format = (UmiDrPackageFormat)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->entrypoint, sizeof(value->entrypoint));
}
static UmiStatus UmiDrApplicationVariantArchiveValidate(const UmiDrApplicationVariant *value)
{
    return umi_dr_application_variant_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_application_variant_archive_encode, umi_dr_application_variant_archive_decode,
    UmiDrApplicationVariant, UmiDrApplicationVariantArchiveSchema, UmiDrApplicationVariantArchiveBound, UmiDrApplicationVariantArchiveWrite, UmiDrApplicationVariantArchiveRead, UmiDrApplicationVariantArchiveValidate)
