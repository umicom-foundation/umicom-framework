/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/sdk_package.c
 *
 * PURPOSE:
 *   developer SDK package metadata and ABI compatibility range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/sdk_package.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr sdk package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_sdk_package_init(UmiDrSdkPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrSdkPackage){0};  } }
/* Check that dr sdk package satisfies its contract before another service relies on it. */
bool umi_dr_sdk_package_valid(const UmiDrSdkPackage *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && (value->headers || value->libraries) && umi_dr_version_compare(value->maximum_abi,value->minimum_abi)>=0); }
/*
 * Provide the dr sdk package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_sdk_package_fingerprint(const UmiDrSdkPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_sdk_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrSdkPackageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc5c00f2c00abe4de);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrSdkPackage *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrSdkPackageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrSdkPackage *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrSdkPackageArchiveWrite(UmiArchiveWriter *writer, const UmiDrSdkPackage *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_abi.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_abi.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_abi.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_abi.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_abi.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_abi.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->headers);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->libraries);
}
static void UmiDrSdkPackageArchiveRead(UmiArchiveReader *reader, UmiDrSdkPackage *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_abi.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_abi.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_abi.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maximum_abi.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maximum_abi.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maximum_abi.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->headers = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->libraries = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrSdkPackageArchiveValidate(const UmiDrSdkPackage *value)
{
    return umi_dr_sdk_package_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_sdk_package_archive_encode, umi_dr_sdk_package_archive_decode,
    UmiDrSdkPackage, UmiDrSdkPackageArchiveSchema, UmiDrSdkPackageArchiveBound, UmiDrSdkPackageArchiveWrite, UmiDrSdkPackageArchiveRead, UmiDrSdkPackageArchiveValidate)
