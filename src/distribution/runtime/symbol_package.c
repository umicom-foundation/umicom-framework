/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/symbol_package.c
 *
 * PURPOSE:
 *   debug symbol package metadata and build-id matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/symbol_package.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr symbol package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_symbol_package_init(UmiDrSymbolPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrSymbolPackage){0};  } }
/* Check that dr symbol package satisfies its contract before another service relies on it. */
bool umi_dr_symbol_package_valid(const UmiDrSymbolPackage *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->build_id, '\0', sizeof(value->build_id)) == NULL) return 0;
    if (memchr(value->digest, '\0', sizeof(value->digest)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->build_id[0] != '\0' && value->digest[0] != '\0'); }
/*
 * Provide the dr symbol package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_symbol_package_fingerprint(const UmiDrSymbolPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_symbol_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrSymbolPackageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x405c7476eea4ab42);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrSymbolPackage *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrSymbolPackage *)0)->build_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrSymbolPackage *)0)->digest)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrSymbolPackageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrSymbolPackage *)0)->id) - 1U +
        8U + sizeof(((UmiDrSymbolPackage *)0)->build_id) - 1U +
        8U + sizeof(((UmiDrSymbolPackage *)0)->digest) - 1U +
        8U;
}
static void UmiDrSymbolPackageArchiveWrite(UmiArchiveWriter *writer, const UmiDrSymbolPackage *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->build_id, sizeof(value->build_id));
    UmiArchiveWriteText(writer, value->digest, sizeof(value->digest));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->size_bytes);
}
static void UmiDrSymbolPackageArchiveRead(UmiArchiveReader *reader, UmiDrSymbolPackage *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->build_id, sizeof(value->build_id));
    UmiArchiveReadText(reader, value->digest, sizeof(value->digest));
    value->size_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDrSymbolPackageArchiveValidate(const UmiDrSymbolPackage *value)
{
    return umi_dr_symbol_package_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_symbol_package_archive_encode, umi_dr_symbol_package_archive_decode,
    UmiDrSymbolPackage, UmiDrSymbolPackageArchiveSchema, UmiDrSymbolPackageArchiveBound, UmiDrSymbolPackageArchiveWrite, UmiDrSymbolPackageArchiveRead, UmiDrSymbolPackageArchiveValidate)
