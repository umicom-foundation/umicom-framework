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

#ifdef __cplusplus
}
#endif
#endif
