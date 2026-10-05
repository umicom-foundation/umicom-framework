/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_manifest.c
 *
 * PURPOSE:
 *   cross-platform application identity and runtime requirement manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_manifest.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr application manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_application_manifest_init(UmiDrApplicationManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrApplicationManifest){0};  } }
/*
 * Check that dr application manifest satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_application_manifest_valid(const UmiDrApplicationManifest *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->name[0] != '\0'); }
/*
 * Provide the dr application manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_manifest_fingerprint(const UmiDrApplicationManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_application_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrApplicationManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4ef2bb47c530fc75);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationManifest *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrApplicationManifest *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrApplicationManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrApplicationManifest *)0)->id) - 1U +
        8U + sizeof(((UmiDrApplicationManifest *)0)->name) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrApplicationManifestArchiveWrite(UmiArchiveWriter *writer, const UmiDrApplicationManifest *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.patch);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_capabilities);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->gui);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->networked);
}
static void UmiDrApplicationManifestArchiveRead(UmiArchiveReader *reader, UmiDrApplicationManifest *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->required_capabilities = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->gui = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->networked = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrApplicationManifestArchiveValidate(const UmiDrApplicationManifest *value)
{
    return umi_dr_application_manifest_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_application_manifest_archive_encode, umi_dr_application_manifest_archive_decode,
    UmiDrApplicationManifest, UmiDrApplicationManifestArchiveSchema, UmiDrApplicationManifestArchiveBound, UmiDrApplicationManifestArchiveWrite, UmiDrApplicationManifestArchiveRead, UmiDrApplicationManifestArchiveValidate)
