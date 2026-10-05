/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/timeline_spec.c
 *
 * PURPOSE:
 *   Define track, snapping and zoom semantics for media, audio, video and event timelines.
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

#include "umicom/ui/design/timeline_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design timeline spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_timeline_spec_valid(const UmiDesignTimelineSpec *spec) { return spec!=NULL && (spec->tracks>0U && spec->tracks<=512U && umi_design_number_valid(spec->pixels_per_unit) && spec->pixels_per_unit>0.0) ? 1 : 0; }
/*
 * Initialise design timeline spec from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_design_timeline_spec_init(UmiDesignTimelineSpec *spec, uint16_t tracks, double pixels_per_unit, int snapping, int zoomable, int scrubbable)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->tracks=tracks;spec->pixels_per_unit=pixels_per_unit;spec->snapping=snapping?1:0;spec->zoomable=zoomable?1:0;spec->scrubbable=scrubbable?1:0;
    return umi_design_timeline_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignTimelineSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x85799502ea19c322);

    return schema;
}
static size_t UmiDesignTimelineSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignTimelineSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignTimelineSpec *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->tracks);
    UmiArchiveWriteDouble(writer, value->pixels_per_unit);
    UmiArchiveWriteSigned(writer, (int64_t)value->snapping);
    UmiArchiveWriteSigned(writer, (int64_t)value->zoomable);
    UmiArchiveWriteSigned(writer, (int64_t)value->scrubbable);
}
static void UmiDesignTimelineSpecArchiveRead(UmiArchiveReader *reader, UmiDesignTimelineSpec *value)
{
    value->tracks = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->pixels_per_unit = UmiArchiveReadDouble(reader);
    value->snapping = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->zoomable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->scrubbable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignTimelineSpecArchiveValidate(const UmiDesignTimelineSpec *value)
{
    return umi_design_timeline_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_timeline_spec_archive_encode, umi_design_timeline_spec_archive_decode,
    UmiDesignTimelineSpec, UmiDesignTimelineSpecArchiveSchema, UmiDesignTimelineSpecArchiveBound, UmiDesignTimelineSpecArchiveWrite, UmiDesignTimelineSpecArchiveRead, UmiDesignTimelineSpecArchiveValidate)
