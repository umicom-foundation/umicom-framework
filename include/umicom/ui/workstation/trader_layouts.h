/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/trader_layouts.h
 *
 * PURPOSE:
 *   Define Framework-owned Trader presets for active trading, charts, scalping, research, strategy, portfolio risk and options workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TRADER_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_TRADER_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ws_trader_layout_active(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_trader_layout_multi_chart(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_trader_layout_research(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_trader_layout_portfolio_risk(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_trader_layout_scalper(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_trader_layout_strategy_lab(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_trader_layout_options(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
