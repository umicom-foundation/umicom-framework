/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/release_manifest.c
 *
 * PURPOSE:
 *   release identity, channel, platform matrix and artifact summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/release_manifest.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr release manifest from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_release_manifest_init(UmiDrReleaseManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrReleaseManifest){0};  } }
/*
 * Check that dr release manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_release_manifest_valid(const UmiDrReleaseManifest *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->application_id, '\0', sizeof(value->application_id)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->channel != 0 && value->artifact_count>0U && value->platform_count>0U); }
/*
 * Provide the dr release manifest fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_release_manifest_fingerprint(const UmiDrReleaseManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_release_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrReleaseManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x514442cbc918f3df);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrReleaseManifest *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrReleaseManifest *)0)->application_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrReleaseManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrReleaseManifest *)0)->id) - 1U +
        8U + sizeof(((UmiDrReleaseManifest *)0)->application_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDrReleaseManifestArchiveWrite(UmiArchiveWriter *writer, const UmiDrReleaseManifest *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.major);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->version.patch);
    UmiArchiveWriteSigned(writer, (int64_t)value->channel);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->artifact_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->platform_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiDrReleaseManifestArchiveRead(UmiArchiveReader *reader, UmiDrReleaseManifest *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    value->version.major = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.minor = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->version.patch = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->channel = (UmiDrChannelKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->artifact_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->platform_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiDrReleaseManifestArchiveValidate(const UmiDrReleaseManifest *value)
{
    return umi_dr_release_manifest_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_release_manifest_archive_encode, umi_dr_release_manifest_archive_decode,
    UmiDrReleaseManifest, UmiDrReleaseManifestArchiveSchema, UmiDrReleaseManifestArchiveBound, UmiDrReleaseManifestArchiveWrite, UmiDrReleaseManifestArchiveRead, UmiDrReleaseManifestArchiveValidate)
