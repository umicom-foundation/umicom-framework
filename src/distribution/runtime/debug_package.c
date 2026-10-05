/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/debug_package.c
 *
 * PURPOSE:
 *   diagnostic/debug companion package metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/debug_package.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr debug package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_debug_package_init(UmiDrDebugPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDebugPackage){0};  } }
/* Check that dr debug package satisfies its contract before another service relies on it. */
bool umi_dr_debug_package_valid(const UmiDrDebugPackage *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->application_id, '\0', sizeof(value->application_id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && (value->symbols || value->diagnostics || value->source_maps)); }
/*
 * Provide the dr debug package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_debug_package_fingerprint(const UmiDrDebugPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_debug_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrDebugPackageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0b206fa0f6701847);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDebugPackage *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDebugPackage *)0)->application_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrDebugPackageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrDebugPackage *)0)->id) - 1U +
        8U + sizeof(((UmiDrDebugPackage *)0)->application_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDrDebugPackageArchiveWrite(UmiArchiveWriter *writer, const UmiDrDebugPackage *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->symbols);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->diagnostics);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->source_maps);
}
static void UmiDrDebugPackageArchiveRead(UmiArchiveReader *reader, UmiDrDebugPackage *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    value->symbols = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->diagnostics = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->source_maps = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrDebugPackageArchiveValidate(const UmiDrDebugPackage *value)
{
    return umi_dr_debug_package_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_debug_package_archive_encode, umi_dr_debug_package_archive_decode,
    UmiDrDebugPackage, UmiDrDebugPackageArchiveSchema, UmiDrDebugPackageArchiveBound, UmiDrDebugPackageArchiveWrite, UmiDrDebugPackageArchiveRead, UmiDrDebugPackageArchiveValidate)
