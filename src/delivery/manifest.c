/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/manifest.c
 *
 * PURPOSE:
 *   Describe an application release independently from the package technology used to distribute it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The manifest is the stable identity card for a release and links source revision, version, channel and generation.
 */

#include "umicom/delivery/manifest.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise delivery manifest from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_manifest_init(UmiDeliveryManifest *manifest,
                                     const char *application_id,
                                     const char *release_id,
                                     const char *version,
                                     UmiReleaseChannel channel)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || application_id == NULL || release_id == NULL ||
        version == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(manifest, 0, sizeof(*manifest));
    status = umi_delivery_copy_text(manifest->application_id,
                                    sizeof(manifest->application_id),
                                    application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->release_id,
                                    sizeof(manifest->release_id), release_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(manifest->version,
                                    sizeof(manifest->version), version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    manifest->channel = channel;
    return UMI_STATUS_OK;
}

/*
 * Provide the delivery manifest set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_delivery_manifest_set_generation(UmiDeliveryManifest *manifest,
                                               const char *generation_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || generation_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(manifest->generation_id,
                                  sizeof(manifest->generation_id), generation_id);
}

/*
 * Provide the delivery manifest set source revision operation used by this module and its
 * client applications.
 */
UmiStatus umi_delivery_manifest_set_source_revision(UmiDeliveryManifest *manifest,
                                                    const char *revision)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || revision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(manifest->source_revision,
                                  sizeof(manifest->source_revision), revision);
}

/* Check that delivery manifest satisfies its contract before another service relies on it. */
UmiStatus umi_delivery_manifest_validate(const UmiDeliveryManifest *manifest)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(manifest->application_id, '\0', sizeof(manifest->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(manifest->release_id, '\0', sizeof(manifest->release_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(manifest->version, '\0', sizeof(manifest->version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(manifest->generation_id, '\0', sizeof(manifest->generation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(manifest->source_revision, '\0', sizeof(manifest->source_revision)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (manifest->application_id[0] == '\0' || manifest->release_id[0] == '\0' ||
        manifest->version[0] == '\0') return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDeliveryManifestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc2159ae5e000aa29);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeliveryManifest *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeliveryManifest *)0)->release_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeliveryManifest *)0)->version)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeliveryManifest *)0)->generation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDeliveryManifest *)0)->source_revision)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDeliveryManifestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDeliveryManifest *)0)->application_id) - 1U +
        8U + sizeof(((UmiDeliveryManifest *)0)->release_id) - 1U +
        8U + sizeof(((UmiDeliveryManifest *)0)->version) - 1U +
        8U + sizeof(((UmiDeliveryManifest *)0)->generation_id) - 1U +
        8U + sizeof(((UmiDeliveryManifest *)0)->source_revision) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDeliveryManifestArchiveWrite(UmiArchiveWriter *writer, const UmiDeliveryManifest *value)
{
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->release_id, sizeof(value->release_id));
    UmiArchiveWriteText(writer, value->version, sizeof(value->version));
    UmiArchiveWriteText(writer, value->generation_id, sizeof(value->generation_id));
    UmiArchiveWriteText(writer, value->source_revision, sizeof(value->source_revision));
    UmiArchiveWriteSigned(writer, (int64_t)value->channel);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->created_epoch_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->artifact_count);
}
static void UmiDeliveryManifestArchiveRead(UmiArchiveReader *reader, UmiDeliveryManifest *value)
{
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->release_id, sizeof(value->release_id));
    UmiArchiveReadText(reader, value->version, sizeof(value->version));
    UmiArchiveReadText(reader, value->generation_id, sizeof(value->generation_id));
    UmiArchiveReadText(reader, value->source_revision, sizeof(value->source_revision));
    value->channel = (UmiReleaseChannel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->created_epoch_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->artifact_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
}
static UmiStatus UmiDeliveryManifestArchiveValidate(const UmiDeliveryManifest *value)
{
    return umi_delivery_manifest_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_delivery_manifest_archive_encode, umi_delivery_manifest_archive_decode,
    UmiDeliveryManifest, UmiDeliveryManifestArchiveSchema, UmiDeliveryManifestArchiveBound, UmiDeliveryManifestArchiveWrite, UmiDeliveryManifestArchiveRead, UmiDeliveryManifestArchiveValidate)
