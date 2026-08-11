/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/party.c
 *
 * PURPOSE:
 *   Validate canonical party records used by broker, TMS and banking adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The implementation is deliberately small and deterministic so callers can test party behaviour without starting a complete product.
 */

#include "umicom/finance/party.h"
#include "umicom/finance/identifier.h"
int umi_financial_party_valid(const UmiFinancialParty *party){return party!=NULL && umi_financial_id_valid(&party->party_id) && party->display_name[0]!='\0';}
