/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/package_profile.c
 *
 * PURPOSE:
 *   named package profile selecting format, scope, compression and symbols policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_profile.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr package profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_package_profile_init(UmiDrPackageProfile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrPackageProfile){0}; value->compression_level=6U; } }
/*
 * Check that dr package profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_package_profile_valid(const UmiDrPackageProfile *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->format != 0 && value->scope != 0 && value->compression_level<=9U); }
/*
 * Provide the dr package profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_package_profile_fingerprint(const UmiDrPackageProfile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_package_profile_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrPackageProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x12afad69873b6a23);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrPackageProfile *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrPackageProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrPackageProfile *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrPackageProfileArchiveWrite(UmiArchiveWriter *writer, const UmiDrPackageProfile *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->format);
    UmiArchiveWriteSigned(writer, (int64_t)value->scope);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->compression_level);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->include_symbols);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->deterministic);
}
static void UmiDrPackageProfileArchiveRead(UmiArchiveReader *reader, UmiDrPackageProfile *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->format = (UmiDrPackageFormat)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->scope = (UmiDrInstallScope)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->compression_level = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->include_symbols = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->deterministic = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrPackageProfileArchiveValidate(const UmiDrPackageProfile *value)
{
    return umi_dr_package_profile_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_package_profile_archive_encode, umi_dr_package_profile_archive_decode,
    UmiDrPackageProfile, UmiDrPackageProfileArchiveSchema, UmiDrPackageProfileArchiveBound, UmiDrPackageProfileArchiveWrite, UmiDrPackageProfileArchiveRead, UmiDrPackageProfileArchiveValidate)
