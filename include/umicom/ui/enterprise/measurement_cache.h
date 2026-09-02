/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/measurement_cache.h
 *
 * PURPOSE:
 *   Cache measured row heights and column widths for adaptive rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_MEASUREMENT_CACHE_H
#define UMICOM_UI_ENTERPRISE_MEASUREMENT_CACHE_H

#include "umicom/ui/enterprise/types.h"
/**
 * Represent the ui ent measurement data shared with callers of this public contract.
 */
typedef struct UmiUiEntMeasurement { char id[UMI_UI_ENT_ID_CAPACITY]; int32_t pixels; uint64_t revision; } UmiUiEntMeasurement;
/**
 * Represent the ui ent measurement cache data shared with callers of this public contract.
 */
typedef struct UmiUiEntMeasurementCache { UmiUiEntMeasurement items[UMI_UI_ENT_MAX_CACHE]; size_t count; } UmiUiEntMeasurementCache;
/**
 * Initialise ui ent measurement cache from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_measurement_cache_init(UmiUiEntMeasurementCache *c);
/**
 * Copy ui ent measurement cache into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_ui_ent_measurement_cache_set(UmiUiEntMeasurementCache *c,const char *id,int32_t pixels,uint64_t revision);
/**
 * Provide the ui ent measurement cache get operation used by this module and its client
 * applications.
 */
int umi_ui_ent_measurement_cache_get(const UmiUiEntMeasurementCache *c,const char *id,int32_t *out_pixels);

#endif
