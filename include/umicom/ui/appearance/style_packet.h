/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/style_packet.h
 *
 * PURPOSE:
 *   Bundle resolved theme, typography, density, scale and motion identities for one renderer update.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_STYLE_PACKET_H
#define UMICOM_UI_APPEARANCE_STYLE_PACKET_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance style packet data shared with callers of this public contract.
 */
typedef struct UmiAppearanceStylePacket {
    char packet_id[UMI_APPEARANCE_ID_CAPACITY];
    char theme_pack_id[UMI_APPEARANCE_ID_CAPACITY];
    char typography_policy_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignDensity density;
    double scale;
    bool reduced_motion;
    uint64_t revision;
} UmiAppearanceStylePacket;

/* Initialise one style packet record with deterministic defaults. */
UmiStatus umi_appearance_style_packet_init(UmiAppearanceStylePacket *item);
/* Validate the required production invariants for this style packet. */
int umi_appearance_style_packet_is_valid(const UmiAppearanceStylePacket *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_style_packet_archive_encode(const UmiAppearanceStylePacket *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_style_packet_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceStylePacket *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAppearanceStylePacket object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_appearance_style_packet_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAppearanceStylePacket *workspace, UmiAppearanceStylePacket *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_appearance_style_packet_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAppearanceStylePacket *expected, UmiAppearanceStylePacket *value);


#ifdef __cplusplus
}
#endif
#endif
