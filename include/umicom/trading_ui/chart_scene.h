/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/chart_scene.h
 * PURPOSE: Build reusable interactive trading chart scenes from canonical evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_UI_CHART_SCENE_H
#define UMICOM_TRADING_UI_CHART_SCENE_H
#include "umicom/trading/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TRADING_CHART_WIDTH 1000.0
#define UMI_TRADING_CHART_HEIGHT 560.0
typedef struct UmiTradingChartSceneInfo {
    UmiChartPlotViewport price;
    UmiChartWindow window;
    size_t retained_bars;
    char instrument_id[UMI_FINANCE_ID_CAPACITY];
} UmiTradingChartSceneInfo;
/* The caller owns the returned scene. Prices/volumes/drawings/orders come only
 * from this workspace. Scene construction never changes the ticket or orders.
 * No history returns NOT_FOUND and a null scene. Coordinate size is fixed;
 * adapters scale input and rendering to the same logical dimensions. */
UmiStatus UmiTradingChartBuildScene(UmiTradingWorkspace *workspace,
    UmiTradingChartSceneInfo *out_info, UmiChartRenderScene **out_scene);
#ifdef __cplusplus
}
#endif
#endif
