/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/venue_descriptor.c
 *
 * PURPOSE:
 *   Define exchange and execution-venue identity and capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_descriptor.h"

#include <string.h>
/* Initialise a venue descriptor with bounded identifiers and text. */
UmiStatus umi_trading_venue_descriptor_init(UmiTradingVenueDescriptor *venue,const char *id,const char *mic,const char *name,bool auctions,bool hidden,uint32_t priority) {
    if(venue==NULL||id==NULL||mic==NULL||name==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(venue,0,sizeof *venue);
    if(umi_trading_core_id_assign(&venue->venue_id,id)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_trading_core_copy_text(venue->mic,sizeof venue->mic,mic)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_trading_core_copy_text(venue->name,sizeof venue->name,name)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    venue->supports_auctions=auctions; venue->supports_hidden_liquidity=hidden; venue->priority=priority;
    return umi_trading_venue_descriptor_valid(venue)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the required venue identity fields. */
bool umi_trading_venue_descriptor_valid(const UmiTradingVenueDescriptor *venue) { return venue!=NULL&&venue->venue_id.value[0]!='\0'&&venue->mic[0]!='\0'&&venue->name[0]!='\0'; }
