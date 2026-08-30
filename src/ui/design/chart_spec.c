/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/chart_spec.c
 *
 * PURPOSE:
 *   Define common analytical chart presentation and interaction semantics shared by Trader, TMS, finance and dashboards.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/chart_spec.h"

int umi_design_chart_spec_valid(const UmiDesignChartSpec *spec){return spec!=NULL&&spec->kind>=UMI_DESIGN_CHART_LINE&&spec->kind<=UMI_DESIGN_CHART_CANDLESTICK&&spec->series_count>0U&&spec->series_count<=64U?1:0;}
UmiStatus umi_design_chart_spec_init(UmiDesignChartSpec *spec,UmiDesignChartKind kind,uint16_t series_count,int legend,int crosshair,int zoom,int pan){if(spec==NULL)return UMI_STATUS_INVALID_ARGUMENT;spec->kind=kind;spec->series_count=series_count;spec->legend=legend?1:0;spec->crosshair=crosshair?1:0;spec->zoom=zoom?1:0;spec->pan=pan?1:0;return umi_design_chart_spec_valid(spec)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
