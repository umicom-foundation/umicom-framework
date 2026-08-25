/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_selection.h
 *
 * PURPOSE:
 *   Select the best healthy service instance using deterministic priority and weight scoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_SELECTION_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_SELECTION_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/service_registry.h"
#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_fabric_service_select(const UmiFabricServiceRegistry *registry,const char *service_id,UmiFabricServiceInstance *out_instance);

#ifdef __cplusplus
}
#endif
#endif
