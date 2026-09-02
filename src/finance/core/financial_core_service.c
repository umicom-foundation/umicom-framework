/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/financial_core_service.c
 *
 * PURPOSE:
 *   Implement aggregate shared financial core service state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/financial_core_service.h"

#include <string.h>
/* Initialize service. */ void umi_financial_core_service_init(UmiFinancialCoreService *s) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (s == NULL)
    return;
  memset(s, 0, sizeof *s);
  umi_currency_catalogue_init(&s->currencies);
  umi_legal_entity_registry_init(&s->legal_entities);
  umi_counterparty_registry_init(&s->counterparties);
  umi_book_registry_init(&s->books);
  umi_portfolio_registry_init(&s->portfolios);
  umi_account_registry_init(&s->accounts);
  umi_financial_product_registry_init(&s->products);
  umi_trade_registry_init(&s->trades);
}
/* Start service after reference data configuration. */ UmiStatus
umi_financial_core_service_start(UmiFinancialCoreService *s) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (s == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this operation only while the related capability or state is available. */
  if (s->ready)
    return UMI_STATUS_ALREADY_EXISTS;
  s->ready = true;
  s->revision++;
  return UMI_STATUS_OK;
}
/* Return total registered reference-data records. */ size_t
umi_financial_core_service_inventory(const UmiFinancialCoreService *s) {
  return s == NULL ? 0U
                   : s->currencies.count + s->legal_entities.count + s->counterparties.count +
                         s->books.count + s->portfolios.count + s->accounts.count +
                         s->products.count + s->trades.count;
}
