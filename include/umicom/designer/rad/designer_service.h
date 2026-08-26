/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/designer_service.h
 *
 * PURPOSE:
 *   Aggregate RAD designer readiness and active-session state for thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DESIGNER_SERVICE_H
#define UMICOM_DESIGNER_RAD_DESIGNER_SERVICE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDesignerService {
    size_t active_sessions;
    size_t open_documents;
    uint32_t conformance_score;
    bool initialized;
} UmiRadDesignerService;
UmiStatus umi_rad_designer_service_init(UmiRadDesignerService *item);
int umi_rad_designer_service_is_valid(const UmiRadDesignerService *item);
#ifdef __cplusplus
}
#endif
#endif
