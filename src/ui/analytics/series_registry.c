/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/series_registry.c
 *
 * PURPOSE:
 *   Register non-owning analytics series references by stable identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/series_registry.h"

#include <string.h>
void umi_analytics_series_registry_init(UmiAnalyticsSeriesRegistry *registry){if(registry!=NULL)memset(registry,0,sizeof *registry);}
UmiStatus umi_analytics_series_registry_add(UmiAnalyticsSeriesRegistry *registry,UmiAnalyticsSeries *series){size_t i;if(registry==NULL||series==NULL||series->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<registry->count;i++)if(strcmp(registry->items[i]->id,series->id)==0)return UMI_STATUS_ALREADY_EXISTS;if(registry->count>=UMI_ANALYTICS_MAX_SERIES)return UMI_STATUS_CAPACITY_EXCEEDED;registry->items[registry->count++]=series;registry->revision++;return UMI_STATUS_OK;}
UmiAnalyticsSeries *umi_analytics_series_registry_find(const UmiAnalyticsSeriesRegistry *registry,const char *id){size_t i;if(registry==NULL||id==NULL)return NULL;for(i=0;i<registry->count;i++)if(strcmp(registry->items[i]->id,id)==0)return registry->items[i];return NULL;}
