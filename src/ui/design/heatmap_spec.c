/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/heatmap_spec.c
 *
 * PURPOSE:
 *   Define heatmap matrix dimensions and numeric domain for risk, analytics and diagnostics surfaces.
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

#include "umicom/ui/design/heatmap_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design heatmap spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_heatmap_spec_valid(const UmiDesignHeatmapSpec *spec) { return spec!=NULL && (spec->rows>0U && spec->columns>0U && spec->rows<=128U && spec->columns<=128U && umi_design_number_valid(spec->minimum) && umi_design_number_valid(spec->maximum) && spec->maximum>spec->minimum) ? 1 : 0; }
/*
 * Initialise design heatmap spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_heatmap_spec_init(UmiDesignHeatmapSpec *spec, uint16_t rows, uint16_t columns, double minimum, double maximum, int show_labels)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->rows=rows;spec->columns=columns;spec->minimum=minimum;spec->maximum=maximum;spec->show_labels=show_labels?1:0;
    return umi_design_heatmap_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignHeatmapSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5fbeced70b1a463b);

    return schema;
}
static size_t UmiDesignHeatmapSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignHeatmapSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignHeatmapSpec *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->rows);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->columns);
    UmiArchiveWriteDouble(writer, value->minimum);
    UmiArchiveWriteDouble(writer, value->maximum);
    UmiArchiveWriteSigned(writer, (int64_t)value->show_labels);
}
static void UmiDesignHeatmapSpecArchiveRead(UmiArchiveReader *reader, UmiDesignHeatmapSpec *value)
{
    value->rows = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->columns = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->minimum = UmiArchiveReadDouble(reader);
    value->maximum = UmiArchiveReadDouble(reader);
    value->show_labels = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignHeatmapSpecArchiveValidate(const UmiDesignHeatmapSpec *value)
{
    return umi_design_heatmap_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_heatmap_spec_archive_encode, umi_design_heatmap_spec_archive_decode,
    UmiDesignHeatmapSpec, UmiDesignHeatmapSpecArchiveSchema, UmiDesignHeatmapSpecArchiveBound, UmiDesignHeatmapSpecArchiveWrite, UmiDesignHeatmapSpecArchiveRead, UmiDesignHeatmapSpecArchiveValidate)
