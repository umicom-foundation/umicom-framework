/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/broker.c
 *
 * PURPOSE:
 *   Define the stable C ABI contract used by simulated and vendor broker adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of broker. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/broker.h"
/* Check that broker contract satisfies its contract before another service relies on it. */
int umi_broker_contract_valid(const UmiBroker *b){return b!=NULL&&b->instance!=NULL&&b->provider_name!=NULL&&b->connect!=NULL&&b->submit_order!=NULL&&b->cancel_order!=NULL&&b->destroy!=NULL;}
