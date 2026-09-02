/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/matrix_layout.c
 *
 * PURPOSE:
 *   Resolve heatmap/matrix cell rectangles from logical viewport dimensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/matrix_layout.h"

/*
 * Provide the analytics matrix layout cell operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_matrix_layout_cell(uint16_t rows,uint16_t columns,uint16_t row,uint16_t column,double width,double height,double gap,UmiAnalyticsMatrixRect *out_rect){double cw,ch;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_rect==NULL||rows==0U||columns==0U||row>=rows||column>=columns||width<=0.0||height<=0.0||gap<0.0)return UMI_STATUS_INVALID_ARGUMENT;cw=(width-gap*(double)(columns-1U))/(double)columns;ch=(height-gap*(double)(rows-1U))/(double)rows;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cw<=0.0||ch<=0.0)return UMI_STATUS_INVALID_ARGUMENT;out_rect->x=(cw+gap)*(double)column;out_rect->y=(ch+gap)*(double)row;out_rect->width=cw;out_rect->height=ch;return UMI_STATUS_OK;}
