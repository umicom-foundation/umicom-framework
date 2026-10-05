/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/style_packet.c
 *
 * PURPOSE:
 *   Bundle resolved theme, typography, density, scale and motion identities for one renderer update.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/style_packet.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_style_packet_init(UmiAppearanceStylePacket *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->packet_id,sizeof item->packet_id,"packet.default");
    (void)umi_appearance_copy_text(item->theme_pack_id,sizeof item->theme_pack_id,"theme.default.dark");
    (void)umi_appearance_copy_text(item->typography_policy_id,sizeof item->typography_policy_id,"typography.default");
    item->density=UMI_DESIGN_DENSITY_STANDARD;
    item->scale=1.0;
    item->revision=1U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_style_packet_is_valid(const UmiAppearanceStylePacket *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->packet_id, '\0', sizeof(item->packet_id)) == NULL) return 0;
    if (memchr(item->theme_pack_id, '\0', sizeof(item->theme_pack_id)) == NULL) return 0;
    if (memchr(item->typography_policy_id, '\0', sizeof(item->typography_policy_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->packet_id) && umi_appearance_id_valid(item->theme_pack_id) && item->scale > 0.0 && item->revision > 0U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceStylePacketArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xedd7ff5acdac7294);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceStylePacket *)0)->packet_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceStylePacket *)0)->theme_pack_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceStylePacket *)0)->typography_policy_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceStylePacketArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceStylePacket *)0)->packet_id) - 1U +
        8U + sizeof(((UmiAppearanceStylePacket *)0)->theme_pack_id) - 1U +
        8U + sizeof(((UmiAppearanceStylePacket *)0)->typography_policy_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceStylePacketArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceStylePacket *value)
{
    UmiArchiveWriteText(writer, value->packet_id, sizeof(value->packet_id));
    UmiArchiveWriteText(writer, value->theme_pack_id, sizeof(value->theme_pack_id));
    UmiArchiveWriteText(writer, value->typography_policy_id, sizeof(value->typography_policy_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->density);
    UmiArchiveWriteDouble(writer, value->scale);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reduced_motion);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiAppearanceStylePacketArchiveRead(UmiArchiveReader *reader, UmiAppearanceStylePacket *value)
{
    UmiArchiveReadText(reader, value->packet_id, sizeof(value->packet_id));
    UmiArchiveReadText(reader, value->theme_pack_id, sizeof(value->theme_pack_id));
    UmiArchiveReadText(reader, value->typography_policy_id, sizeof(value->typography_policy_id));
    value->density = (UmiDesignDensity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->scale = UmiArchiveReadDouble(reader);
    value->reduced_motion = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiAppearanceStylePacketArchiveValidate(const UmiAppearanceStylePacket *value)
{
    return umi_appearance_style_packet_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_style_packet_archive_encode, umi_appearance_style_packet_archive_decode,
    UmiAppearanceStylePacket, UmiAppearanceStylePacketArchiveSchema, UmiAppearanceStylePacketArchiveBound, UmiAppearanceStylePacketArchiveWrite, UmiAppearanceStylePacketArchiveRead, UmiAppearanceStylePacketArchiveValidate)
