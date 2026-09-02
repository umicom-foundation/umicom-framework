/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_service.h
 *
 * PURPOSE:
 *   Expose aggregate readiness for Framework-owned production appearance services consumed by every thin application.
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
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_SERVICE_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_SERVICE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance appearance service data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAppearanceService {
    char service_id[UMI_APPEARANCE_ID_CAPACITY];
    bool themes_ready;
    bool typography_ready;
    bool scaling_ready;
    bool accessibility_ready;
    bool renderers_ready;
    uint64_t revision;
} UmiAppearanceAppearanceService;

/* Initialise one appearance service record with deterministic defaults. */
UmiStatus umi_appearance_service_init(UmiAppearanceAppearanceService *item);
/* Validate the required production invariants for this appearance service. */
int umi_appearance_service_is_valid(const UmiAppearanceAppearanceService *item);
/* Return one only when all production appearance capability groups are ready. */
int umi_appearance_service_ready(const UmiAppearanceAppearanceService *item);

#ifdef __cplusplus
}
#endif
#endif
