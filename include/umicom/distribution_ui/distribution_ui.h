/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution_ui/distribution_ui.h
 *
 * PURPOSE:
 *   Project distribution state into toolkit-neutral Marketplace, Installed,
 *   Updates, Transactions and Supply-chain Evidence views.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_UI_DISTRIBUTION_UI_H
#define UMICOM_DISTRIBUTION_UI_DISTRIBUTION_UI_H

#include "umicom/distribution/service.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DISTRIBUTION_UI_VISIBLE_ROWS 32U

/**
 * Initialise distribution ui marketplace view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_distribution_ui_marketplace_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise distribution ui installed view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_distribution_ui_installed_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise distribution ui updates view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_distribution_ui_updates_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise distribution ui transactions view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_distribution_ui_transactions_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view);
/**
 * Initialise distribution ui evidence view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_distribution_ui_evidence_view_create(
    const char *view_id,
    UmiDistributionService *service,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
