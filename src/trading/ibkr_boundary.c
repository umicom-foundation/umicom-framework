/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/ibkr_boundary.c
 *
 * PURPOSE:
 *   Validate Interactive Brokers connection settings without exposing vendor SDK types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of ibkr boundary. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/ibkr_boundary.h"
/* Check that ibkr settings satisfies its contract before another service relies on it. */
int umi_ibkr_settings_valid(const UmiIbkrConnectionSettings *s){return s!=NULL&&s->host[0]!='\0'&&s->port>0U&&s->client_id>=0;}
