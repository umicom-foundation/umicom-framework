/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/trader_layouts.h
 *
 * PURPOSE:
 *   Define Framework-owned Trader presets for active trading, charts, scalping, research, strategy, portfolio risk and options workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TRADER_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_TRADER_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ws trader layout active operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_trader_layout_active(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws trader layout multi chart operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_trader_layout_multi_chart(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws trader layout research operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_trader_layout_research(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws trader layout portfolio risk operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_trader_layout_portfolio_risk(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws trader layout scalper operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_trader_layout_scalper(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws trader layout strategy lab operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_trader_layout_strategy_lab(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws trader layout options operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_trader_layout_options(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
