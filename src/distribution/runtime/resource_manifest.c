/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/resource_manifest.c
 *
 * PURPOSE:
 *   resource-pack identity, locale, scale and content metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/resource_manifest.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr resource manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_resource_manifest_init(UmiDrResourceManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrResourceManifest){0}; value->scale_percent=100U; } }
/*
 * Check that dr resource manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_resource_manifest_valid(const UmiDrResourceManifest *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->locale, '\0', sizeof(value->locale)) == NULL) return 0;
    if (memchr(value->digest, '\0', sizeof(value->digest)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->scale_percent>=50U && value->scale_percent<=400U && value->digest[0] != '\0'); }
/*
 * Provide the dr resource manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_resource_manifest_fingerprint(const UmiDrResourceManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_resource_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrResourceManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7cb6e9e9ec0cad4c);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrResourceManifest *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrResourceManifest *)0)->locale)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrResourceManifest *)0)->digest)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrResourceManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrResourceManifest *)0)->id) - 1U +
        8U + sizeof(((UmiDrResourceManifest *)0)->locale) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiDrResourceManifest *)0)->digest) - 1U;
}
static void UmiDrResourceManifestArchiveWrite(UmiArchiveWriter *writer, const UmiDrResourceManifest *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->locale, sizeof(value->locale));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->scale_percent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->size_bytes);
    UmiArchiveWriteText(writer, value->digest, sizeof(value->digest));
}
static void UmiDrResourceManifestArchiveRead(UmiArchiveReader *reader, UmiDrResourceManifest *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->locale, sizeof(value->locale));
    value->scale_percent = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->size_bytes = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->digest, sizeof(value->digest));
}
static UmiStatus UmiDrResourceManifestArchiveValidate(const UmiDrResourceManifest *value)
{
    return umi_dr_resource_manifest_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_resource_manifest_archive_encode, umi_dr_resource_manifest_archive_decode,
    UmiDrResourceManifest, UmiDrResourceManifestArchiveSchema, UmiDrResourceManifestArchiveBound, UmiDrResourceManifestArchiveWrite, UmiDrResourceManifestArchiveRead, UmiDrResourceManifestArchiveValidate)
