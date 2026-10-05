/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_package.c
 *
 * PURPOSE:
 *   source distribution metadata, licence and reproducibility flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/source_package.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr source package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_source_package_init(UmiDrSourcePackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrSourcePackage){0};  } }
/* Check that dr source package satisfies its contract before another service relies on it. */
bool umi_dr_source_package_valid(const UmiDrSourcePackage *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->revision, '\0', sizeof(value->revision)) == NULL) return 0;
    if (memchr(value->licence, '\0', sizeof(value->licence)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->revision[0] != '\0' && value->licence[0] != '\0'); }
/*
 * Provide the dr source package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_source_package_fingerprint(const UmiDrSourcePackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_source_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrSourcePackageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x851ddc0b5eafc04c);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrSourcePackage *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrSourcePackage *)0)->revision)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrSourcePackage *)0)->licence)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrSourcePackageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrSourcePackage *)0)->id) - 1U +
        8U + sizeof(((UmiDrSourcePackage *)0)->revision) - 1U +
        8U + sizeof(((UmiDrSourcePackage *)0)->licence) - 1U +
        8U +
        8U;
}
static void UmiDrSourcePackageArchiveWrite(UmiArchiveWriter *writer, const UmiDrSourcePackage *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->revision, sizeof(value->revision));
    UmiArchiveWriteText(writer, value->licence, sizeof(value->licence));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->complete);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reproducible);
}
static void UmiDrSourcePackageArchiveRead(UmiArchiveReader *reader, UmiDrSourcePackage *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->revision, sizeof(value->revision));
    UmiArchiveReadText(reader, value->licence, sizeof(value->licence));
    value->complete = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->reproducible = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrSourcePackageArchiveValidate(const UmiDrSourcePackage *value)
{
    return umi_dr_source_package_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_source_package_archive_encode, umi_dr_source_package_archive_decode,
    UmiDrSourcePackage, UmiDrSourcePackageArchiveSchema, UmiDrSourcePackageArchiveBound, UmiDrSourcePackageArchiveWrite, UmiDrSourcePackageArchiveRead, UmiDrSourcePackageArchiveValidate)
