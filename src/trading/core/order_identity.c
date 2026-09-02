/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_identity.c
 *
 * PURPOSE:
 *   Provide stable client, broker and venue identifiers for one order lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_identity.h"

#include <string.h>
/* Initialise a client identity with optional downstream identifiers empty. */
UmiStatus umi_trading_order_identity_init(UmiTradingOrderIdentity *identity,const char *client_order_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(identity,0,sizeof *identity);return umi_trading_core_id_assign(&identity->client_order_id,client_order_id);}
/* Assign the broker identifier once routing has accepted the order. */
UmiStatus umi_trading_order_identity_set_broker(UmiTradingOrderIdentity *identity,const char *broker_order_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_trading_core_id_assign(&identity->broker_order_id,broker_order_id);}
/* Assign the venue identifier once the venue acknowledges the order. */
UmiStatus umi_trading_order_identity_set_venue(UmiTradingOrderIdentity *identity,const char *venue_order_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_trading_core_id_assign(&identity->venue_order_id,venue_order_id);}
