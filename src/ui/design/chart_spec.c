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
#include "../../base/value_archive_internal.h"

/* Check that design chart spec satisfies its contract before another service relies on it. */
int umi_design_chart_spec_valid(const UmiDesignChartSpec *spec){return spec!=NULL&&spec->kind>=UMI_DESIGN_CHART_LINE&&spec->kind<=UMI_DESIGN_CHART_CANDLESTICK&&spec->series_count>0U&&spec->series_count<=64U?1:0;}
/*
 * Initialise design chart spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_chart_spec_init(UmiDesignChartSpec *spec,UmiDesignChartKind kind,uint16_t series_count,int legend,int crosshair,int zoom,int pan){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(spec==NULL)return UMI_STATUS_INVALID_ARGUMENT;spec->kind=kind;spec->series_count=series_count;spec->legend=legend?1:0;spec->crosshair=crosshair?1:0;spec->zoom=zoom?1:0;spec->pan=pan?1:0;return umi_design_chart_spec_valid(spec)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignChartSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdbd0d8323fd2bf9f);

    return schema;
}
static size_t UmiDesignChartSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignChartSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignChartSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->series_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->legend);
    UmiArchiveWriteSigned(writer, (int64_t)value->crosshair);
    UmiArchiveWriteSigned(writer, (int64_t)value->zoom);
    UmiArchiveWriteSigned(writer, (int64_t)value->pan);
}
static void UmiDesignChartSpecArchiveRead(UmiArchiveReader *reader, UmiDesignChartSpec *value)
{
    value->kind = (UmiDesignChartKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->series_count = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->legend = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->crosshair = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->zoom = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->pan = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignChartSpecArchiveValidate(const UmiDesignChartSpec *value)
{
    return umi_design_chart_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_chart_spec_archive_encode, umi_design_chart_spec_archive_decode,
    UmiDesignChartSpec, UmiDesignChartSpecArchiveSchema, UmiDesignChartSpecArchiveBound, UmiDesignChartSpecArchiveWrite, UmiDesignChartSpecArchiveRead, UmiDesignChartSpecArchiveValidate)
