/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/launcher_manifest.c
 *
 * PURPOSE:
 *   launcher executable, arguments and working-directory contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/launcher_manifest.h"
#include "../../base/value_archive_internal.h"

/*
 * Initialise dr launcher manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_launcher_manifest_init(UmiDrLauncherManifest *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrLauncherManifest){0};  } }
/*
 * Check that dr launcher manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_launcher_manifest_valid(const UmiDrLauncherManifest *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;
    if (memchr(value->executable, '\0', sizeof(value->executable)) == NULL) return 0;
    if (memchr(value->working_directory, '\0', sizeof(value->working_directory)) == NULL) return 0;
    if (memchr(value->arguments, '\0', sizeof(value->arguments)) == NULL) return 0;
 return value != NULL && (value->id[0] != '\0' && value->executable[0] != '\0'); }
/*
 * Provide the dr launcher manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_launcher_manifest_fingerprint(const UmiDrLauncherManifest *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_launcher_manifest_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDrLauncherManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x3513a4cda241ac55);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLauncherManifest *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLauncherManifest *)0)->executable)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLauncherManifest *)0)->working_directory)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDrLauncherManifest *)0)->arguments)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDrLauncherManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDrLauncherManifest *)0)->id) - 1U +
        8U + sizeof(((UmiDrLauncherManifest *)0)->executable) - 1U +
        8U + sizeof(((UmiDrLauncherManifest *)0)->working_directory) - 1U +
        8U + sizeof(((UmiDrLauncherManifest *)0)->arguments) - 1U +
        8U;
}
static void UmiDrLauncherManifestArchiveWrite(UmiArchiveWriter *writer, const UmiDrLauncherManifest *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->executable, sizeof(value->executable));
    UmiArchiveWriteText(writer, value->working_directory, sizeof(value->working_directory));
    UmiArchiveWriteText(writer, value->arguments, sizeof(value->arguments));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->single_instance);
}
static void UmiDrLauncherManifestArchiveRead(UmiArchiveReader *reader, UmiDrLauncherManifest *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->executable, sizeof(value->executable));
    UmiArchiveReadText(reader, value->working_directory, sizeof(value->working_directory));
    UmiArchiveReadText(reader, value->arguments, sizeof(value->arguments));
    value->single_instance = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDrLauncherManifestArchiveValidate(const UmiDrLauncherManifest *value)
{
    return umi_dr_launcher_manifest_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_dr_launcher_manifest_archive_encode, umi_dr_launcher_manifest_archive_decode,
    UmiDrLauncherManifest, UmiDrLauncherManifestArchiveSchema, UmiDrLauncherManifestArchiveBound, UmiDrLauncherManifestArchiveWrite, UmiDrLauncherManifestArchiveRead, UmiDrLauncherManifestArchiveValidate)
