/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/json.h
 *
 * PURPOSE:
 *   Serialise chart state for browser and remote frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Chart data is independent of GTK4, JavaScript and TradingView. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_JSON_H
#define UMICOM_CHART_JSON_H
#include "umicom/chart/model.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_chart_model_to_json(const UmiChartModel *model,char *out_json,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
