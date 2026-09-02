/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/designer_service.h
 *
 * PURPOSE:
 *   Aggregate RAD designer readiness and active-session state for thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DESIGNER_SERVICE_H
#define UMICOM_DESIGNER_RAD_DESIGNER_SERVICE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad designer service data shared with callers of this public contract.
 */
typedef struct UmiRadDesignerService {
    size_t active_sessions;
    size_t open_documents;
    uint32_t conformance_score;
    bool initialized;
} UmiRadDesignerService;
/**
 * Initialise rad designer service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_designer_service_init(UmiRadDesignerService *item);
/**
 * Check that rad designer service satisfies its contract before another service relies on
 * it.
 */
int umi_rad_designer_service_is_valid(const UmiRadDesignerService *item);
#ifdef __cplusplus
}
#endif
#endif
