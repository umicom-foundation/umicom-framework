/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/holiday_catalogue.c
 *
 * PURPOSE:
 *   Implement holiday-calendar catalogue lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/holiday_catalogue.h"

#include <string.h>
/* Reset catalogue. */ void umi_holiday_catalogue_init(UmiHolidayCatalogue *c){if(c!=NULL)memset(c,0,sizeof *c);}
/* Find calendar. */ const UmiHolidayCalendar *umi_holiday_catalogue_find(const UmiHolidayCatalogue *c,const char *id){size_t i;if(c==NULL||id==NULL)return NULL;for(i=0U;i<c->count;i++)if(strcmp(c->items[i].calendar_id.value,id)==0)return &c->items[i];return NULL;}
/* Add unique calendar. */ UmiStatus umi_holiday_catalogue_add(UmiHolidayCatalogue *c,const UmiHolidayCalendar *item){if(c==NULL||item==NULL||!umi_financial_id_is_valid(&item->calendar_id))return UMI_STATUS_INVALID_ARGUMENT;if(umi_holiday_catalogue_find(c,item->calendar_id.value)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(c->count>=16U)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*item;return UMI_STATUS_OK;}
