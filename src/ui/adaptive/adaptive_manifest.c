/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/adaptive_manifest.c
 *
 * PURPOSE:
 *   Declare application-wide adaptive shell capabilities and renderer coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/adaptive_manifest.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialise an adaptive manifest with all current semantic renderer families enabled. */
UmiStatus umi_adaptive_manifest_init(UmiAdaptiveManifest *manifest,
                                     const char *application_id,
                                     const char *shell_profile_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || application_id == NULL || shell_profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(manifest, 0, sizeof *manifest);
    status = umi_adaptive_copy_text(manifest->application_id, sizeof manifest->application_id, application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_adaptive_copy_text(manifest->shell_profile_id, sizeof manifest->shell_profile_id, shell_profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    manifest->renderer_mask = 0x07U;
    manifest->supports_orientation_change = 1;
    return UMI_STATUS_OK;
}

/* Require stable identities and at least one renderer before the adaptive shell is considered valid. */
int umi_adaptive_manifest_valid(const UmiAdaptiveManifest *manifest)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (manifest == NULL) return 0;
    if (memchr(manifest->application_id, '\0', sizeof(manifest->application_id)) == NULL) return 0;
    if (memchr(manifest->shell_profile_id, '\0', sizeof(manifest->shell_profile_id)) == NULL) return 0;

    return manifest != NULL && manifest->application_id[0] != '\0' &&
           manifest->shell_profile_id[0] != '\0' && manifest->renderer_mask != 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAdaptiveManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x33357e28bbf39ead);
    schema = (schema ^ (uint64_t)sizeof(((UmiAdaptiveManifest *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAdaptiveManifest *)0)->shell_profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAdaptiveManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAdaptiveManifest *)0)->application_id) - 1U +
        8U + sizeof(((UmiAdaptiveManifest *)0)->shell_profile_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAdaptiveManifestArchiveWrite(UmiArchiveWriter *writer, const UmiAdaptiveManifest *value)
{
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->shell_profile_id, sizeof(value->shell_profile_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->renderer_mask);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->breakpoint_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->supports_orientation_change);
    UmiArchiveWriteSigned(writer, (int64_t)value->supports_multi_window);
}
static void UmiAdaptiveManifestArchiveRead(UmiArchiveReader *reader, UmiAdaptiveManifest *value)
{
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->shell_profile_id, sizeof(value->shell_profile_id));
    value->renderer_mask = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->breakpoint_count = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->supports_orientation_change = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->supports_multi_window = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAdaptiveManifestArchiveValidate(const UmiAdaptiveManifest *value)
{
    return umi_adaptive_manifest_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_adaptive_manifest_archive_encode, umi_adaptive_manifest_archive_decode,
    UmiAdaptiveManifest, UmiAdaptiveManifestArchiveSchema, UmiAdaptiveManifestArchiveBound, UmiAdaptiveManifestArchiveWrite, UmiAdaptiveManifestArchiveRead, UmiAdaptiveManifestArchiveValidate)
