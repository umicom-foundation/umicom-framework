/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_snapshot.c
 *
 * PURPOSE:
 *   Persist resolved appearance identity and revisions for deterministic session restore and visual tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_snapshot_init(UmiAppearanceAppearanceSnapshot *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->snapshot_id,sizeof item->snapshot_id,"appearance.snapshot");
    (void)umi_appearance_copy_text(item->profile_id,sizeof item->profile_id,"appearance.default");
    (void)umi_appearance_copy_text(item->theme_pack_id,sizeof item->theme_pack_id,"theme.default.dark");
    item->effective_scale=1.0;
    item->semantic_revision=1U;
    item->fingerprint=1469598103934665603ULL;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_snapshot_is_valid(const UmiAppearanceAppearanceSnapshot *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->snapshot_id, '\0', sizeof(item->snapshot_id)) == NULL) return 0;
    if (memchr(item->profile_id, '\0', sizeof(item->profile_id)) == NULL) return 0;
    if (memchr(item->theme_pack_id, '\0', sizeof(item->theme_pack_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->snapshot_id) && item->effective_scale > 0.0 && item->semantic_revision > 0U && item->fingerprint != 0U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceAppearanceSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6bdc20148c7ba2d3);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAppearanceSnapshot *)0)->snapshot_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAppearanceSnapshot *)0)->profile_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceAppearanceSnapshot *)0)->theme_pack_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceAppearanceSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceAppearanceSnapshot *)0)->snapshot_id) - 1U +
        8U + sizeof(((UmiAppearanceAppearanceSnapshot *)0)->profile_id) - 1U +
        8U + sizeof(((UmiAppearanceAppearanceSnapshot *)0)->theme_pack_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceAppearanceSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceAppearanceSnapshot *value)
{
    UmiArchiveWriteText(writer, value->snapshot_id, sizeof(value->snapshot_id));
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteText(writer, value->theme_pack_id, sizeof(value->theme_pack_id));
    UmiArchiveWriteDouble(writer, value->effective_scale);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->semantic_revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiAppearanceAppearanceSnapshotArchiveRead(UmiArchiveReader *reader, UmiAppearanceAppearanceSnapshot *value)
{
    UmiArchiveReadText(reader, value->snapshot_id, sizeof(value->snapshot_id));
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    UmiArchiveReadText(reader, value->theme_pack_id, sizeof(value->theme_pack_id));
    value->effective_scale = UmiArchiveReadDouble(reader);
    value->semantic_revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiAppearanceAppearanceSnapshotArchiveValidate(const UmiAppearanceAppearanceSnapshot *value)
{
    return umi_appearance_snapshot_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_snapshot_archive_encode, umi_appearance_snapshot_archive_decode,
    UmiAppearanceAppearanceSnapshot, UmiAppearanceAppearanceSnapshotArchiveSchema, UmiAppearanceAppearanceSnapshotArchiveBound, UmiAppearanceAppearanceSnapshotArchiveWrite, UmiAppearanceAppearanceSnapshotArchiveRead, UmiAppearanceAppearanceSnapshotArchiveValidate)
