/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/actions.c
 *
 * PURPOSE:
 *   Resolve stable trading workstation action identifiers without embedding
 *   product-specific branching in Studio, Trader or future frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading_ui/actions.h"

#include <string.h>

typedef struct UmiTradingUiActionEntry {
    UmiTradingUiActionKind kind;
    const char *action_id;
    int requires_payload;
} UmiTradingUiActionEntry;

static const UmiTradingUiActionEntry action_entries[] = {
    {UMI_TRADING_UI_ACTION_KIND_REFRESH,
     UMI_TRADING_UI_ACTION_REFRESH, 0},
    {UMI_TRADING_UI_ACTION_KIND_SET_ENVIRONMENT,
     UMI_TRADING_UI_ACTION_SET_ENVIRONMENT, 1},
    {UMI_TRADING_UI_ACTION_KIND_FILTER_INSTRUMENTS,
     UMI_TRADING_UI_ACTION_FILTER_INSTRUMENTS, 1},
    {UMI_TRADING_UI_ACTION_KIND_SELECT_INSTRUMENT,
     UMI_TRADING_UI_ACTION_SELECT_INSTRUMENT, 1},
    {UMI_TRADING_UI_ACTION_KIND_SET_SIDE,
     UMI_TRADING_UI_ACTION_SET_SIDE, 1},
    {UMI_TRADING_UI_ACTION_KIND_SET_ORDER_TYPE,
     UMI_TRADING_UI_ACTION_SET_ORDER_TYPE, 1},
    {UMI_TRADING_UI_ACTION_KIND_SET_QUANTITY,
     UMI_TRADING_UI_ACTION_SET_QUANTITY, 1},
    {UMI_TRADING_UI_ACTION_KIND_SET_PRICES,
     UMI_TRADING_UI_ACTION_SET_PRICES, 1},
    {UMI_TRADING_UI_ACTION_KIND_PREVIEW_ORDER,
     UMI_TRADING_UI_ACTION_PREVIEW_ORDER, 0},
    {UMI_TRADING_UI_ACTION_KIND_SUBMIT_ORDER,
     UMI_TRADING_UI_ACTION_SUBMIT_ORDER, 0},
    {UMI_TRADING_UI_ACTION_KIND_FILTER_ORDERS,
     UMI_TRADING_UI_ACTION_FILTER_ORDERS, 1},
    {UMI_TRADING_UI_ACTION_KIND_SELECT_ORDER,
     UMI_TRADING_UI_ACTION_SELECT_ORDER, 1},
    {UMI_TRADING_UI_ACTION_KIND_CANCEL_ORDER,
     UMI_TRADING_UI_ACTION_CANCEL_ORDER, 0},
    {UMI_TRADING_UI_ACTION_KIND_ENGAGE_KILL_SWITCH,
     UMI_TRADING_UI_ACTION_ENGAGE_KILL_SWITCH, 0},
    {UMI_TRADING_UI_ACTION_KIND_RESET_KILL_SWITCH,
     UMI_TRADING_UI_ACTION_RESET_KILL_SWITCH, 0}
};

static size_t action_entry_count(void)
{
    return sizeof(action_entries) / sizeof(action_entries[0]);
}

UmiTradingUiActionKind umi_trading_ui_action_kind(const char *action_id)
{
    size_t index;
    if (action_id == NULL || action_id[0] == '\0')
        return UMI_TRADING_UI_ACTION_KIND_UNKNOWN;
    for (index = 0U; index < action_entry_count(); ++index) {
        if (strcmp(action_entries[index].action_id, action_id) == 0)
            return action_entries[index].kind;
    }
    return UMI_TRADING_UI_ACTION_KIND_UNKNOWN;
}

const char *umi_trading_ui_action_id(UmiTradingUiActionKind kind)
{
    size_t index;
    for (index = 0U; index < action_entry_count(); ++index) {
        if (action_entries[index].kind == kind)
            return action_entries[index].action_id;
    }
    return NULL;
}

int umi_trading_ui_action_requires_payload(UmiTradingUiActionKind kind)
{
    size_t index;
    for (index = 0U; index < action_entry_count(); ++index) {
        if (action_entries[index].kind == kind)
            return action_entries[index].requires_payload;
    }
    return 0;
}
