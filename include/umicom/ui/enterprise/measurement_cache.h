/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/measurement_cache.h
 *
 * PURPOSE:
 *   Cache measured row heights and column widths for adaptive rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_MEASUREMENT_CACHE_H
#define UMICOM_UI_ENTERPRISE_MEASUREMENT_CACHE_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntMeasurement { char id[UMI_UI_ENT_ID_CAPACITY]; int32_t pixels; uint64_t revision; } UmiUiEntMeasurement;
typedef struct UmiUiEntMeasurementCache { UmiUiEntMeasurement items[UMI_UI_ENT_MAX_CACHE]; size_t count; } UmiUiEntMeasurementCache;
void umi_ui_ent_measurement_cache_init(UmiUiEntMeasurementCache *c);
UmiStatus umi_ui_ent_measurement_cache_set(UmiUiEntMeasurementCache *c,const char *id,int32_t pixels,uint64_t revision);
int umi_ui_ent_measurement_cache_get(const UmiUiEntMeasurementCache *c,const char *id,int32_t *out_pixels);

#endif
