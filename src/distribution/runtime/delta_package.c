/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/delta_package.c
 *
 * PURPOSE:
 *   delta package base/target version and savings validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/delta_package.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr delta package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_delta_package_init(UmiDrDeltaPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDeltaPackage){0};  } }
/* Check that dr delta package satisfies its contract before another service relies on it. */
bool umi_dr_delta_package_valid(const UmiDrDeltaPackage *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->digest, '\0', sizeof(value->digest)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && umi_dr_version_compare(value->target_version,value->base_version)>0 && value->full_size>0U && value->delta_size>0U && value->delta_size<value->full_size && value->digest[0] != '\0'); }
/*
 * Provide the dr delta package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_delta_package_fingerprint(const UmiDrDeltaPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_delta_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrDeltaPackageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb2643486c9f286f2);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDeltaPackage *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDeltaPackage *)0)->digest)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrDeltaPackageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrDeltaPackage *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiDrDeltaPackage *)0)->digest) - 1U;
}
static void UmiDrDeltaPackageArchiveWrite(UmiArchiveWriter *writer, const UmiDrDeltaPackage *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base_version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base_version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->base_version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target_version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target_version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target_version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->full_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->delta_size);
    UmiArchiveWriteText(writer, value->digest, sizeof(value->digest));
}
static void UmiDrDeltaPackageArchiveRead(UmiArchiveReader *reader, UmiDrDeltaPackage *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->base_version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->base_version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->base_version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->target_version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->target_version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->target_version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->full_size = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->delta_size = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->digest, sizeof(value->digest));
}
static UmiStatus UmiDrDeltaPackageArchiveValidate(const UmiDrDeltaPackage *value)
{
    return umi_dr_delta_package_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_delta_package_archive_encode, umi_dr_delta_package_archive_decode,
    UmiDrDeltaPackage, UmiDrDeltaPackageArchiveSchema, UmiDrDeltaPackageArchiveBound, UmiDrDeltaPackageArchiveWrite, UmiDrDeltaPackageArchiveRead, UmiDrDeltaPackageArchiveValidate)
