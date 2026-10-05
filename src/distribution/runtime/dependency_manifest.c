/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/dependency_manifest.c
 *
 * PURPOSE:
 *   package dependency declaration with version and optionality constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_manifest.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr dependency manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_dependency_manifest_init(UmiDrDependencyManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDependencyManifest){0};  } }
/*
 * Check that dr dependency manifest satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_dependency_manifest_valid(const UmiDrDependencyManifest *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->package_id, '\0', sizeof(value->package_id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->package_id[0] != '\0'); }
/*
 * Provide the dr dependency manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_dependency_manifest_fingerprint(const UmiDrDependencyManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_dependency_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrDependencyManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x64870d23026a3b11);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDependencyManifest *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrDependencyManifest *)0)->package_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrDependencyManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrDependencyManifest *)0)->id) - 1U +
        8U + sizeof(((UmiDrDependencyManifest *)0)->package_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrDependencyManifestArchiveWrite(UmiArchiveWriter *writer, const UmiDrDependencyManifest *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->package_id, sizeof(value->package_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->optional);
}
static void UmiDrDependencyManifestArchiveRead(UmiArchiveReader *reader, UmiDrDependencyManifest *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->package_id, sizeof(value->package_id));
    value->minimum_version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->optional = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrDependencyManifestArchiveValidate(const UmiDrDependencyManifest *value)
{
    return umi_dr_dependency_manifest_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_dependency_manifest_archive_encode, umi_dr_dependency_manifest_archive_decode,
    UmiDrDependencyManifest, UmiDrDependencyManifestArchiveSchema, UmiDrDependencyManifestArchiveBound, UmiDrDependencyManifestArchiveWrite, UmiDrDependencyManifestArchiveRead, UmiDrDependencyManifestArchiveValidate)
