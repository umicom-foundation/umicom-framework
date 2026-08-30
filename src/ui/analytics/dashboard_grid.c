/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/dashboard_grid.c
 *
 * PURPOSE:
 *   Validate non-overlapping dashboard tile placement on bounded logical grids.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/dashboard_grid.h"

#include <string.h>
static int overlap(const UmiAnalyticsDashboardTile *a,const UmiAnalyticsDashboardTile *b){uint32_t ar=(uint32_t)a->row+(uint32_t)a->row_span,ac=(uint32_t)a->column+(uint32_t)a->column_span,br=(uint32_t)b->row+(uint32_t)b->row_span,bc=(uint32_t)b->column+(uint32_t)b->column_span;return a->row<br&&b->row<ar&&a->column<bc&&b->column<ac;}
UmiStatus umi_analytics_dashboard_grid_init(UmiAnalyticsDashboardGrid *g,uint16_t rows,uint16_t columns){if(g==NULL||rows==0U||columns==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(g,0,sizeof *g);g->rows=rows;g->columns=columns;return UMI_STATUS_OK;}
UmiStatus umi_analytics_dashboard_grid_add(UmiAnalyticsDashboardGrid *g,const UmiAnalyticsDashboardTile *tile){size_t i;if(g==NULL||!umi_analytics_dashboard_tile_valid(tile))return UMI_STATUS_INVALID_ARGUMENT;if((uint32_t)tile->row+(uint32_t)tile->row_span>(uint32_t)g->rows||(uint32_t)tile->column+(uint32_t)tile->column_span>(uint32_t)g->columns)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<g->count;i++)if(overlap(&g->tiles[i],tile))return UMI_STATUS_INVALID_STATE;if(g->count>=UMI_ANALYTICS_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;g->tiles[g->count++]=*tile;return UMI_STATUS_OK;}
