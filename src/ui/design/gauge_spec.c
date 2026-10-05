/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/gauge_spec.c
 *
 * PURPOSE:
 *   Define bounded gauge ranges, current values and warning thresholds for dashboard metrics.
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

#include "umicom/ui/design/gauge_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design gauge spec satisfies its contract before another service relies on it. */
int umi_design_gauge_spec_valid(const UmiDesignGaugeSpec *spec) { return spec!=NULL && (umi_design_number_valid(spec->minimum) && umi_design_number_valid(spec->maximum) && umi_design_number_valid(spec->value) && spec->maximum>spec->minimum && spec->value>=spec->minimum && spec->value<=spec->maximum && spec->warning_threshold>=spec->minimum && spec->warning_threshold<=spec->maximum && spec->danger_threshold>=spec->warning_threshold && spec->danger_threshold<=spec->maximum) ? 1 : 0; }
/*
 * Initialise design gauge spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_gauge_spec_init(UmiDesignGaugeSpec *spec, double minimum, double maximum, double value, double warning_threshold, double danger_threshold)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->minimum=minimum;spec->maximum=maximum;spec->value=value;spec->warning_threshold=warning_threshold;spec->danger_threshold=danger_threshold;
    return umi_design_gauge_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignGaugeSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa93a5e0613fcfc4c);

    return schema;
}
static size_t UmiDesignGaugeSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignGaugeSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignGaugeSpec *value)
{
    UmiArchiveWriteDouble(writer, value->minimum);
    UmiArchiveWriteDouble(writer, value->maximum);
    UmiArchiveWriteDouble(writer, value->value);
    UmiArchiveWriteDouble(writer, value->warning_threshold);
    UmiArchiveWriteDouble(writer, value->danger_threshold);
}
static void UmiDesignGaugeSpecArchiveRead(UmiArchiveReader *reader, UmiDesignGaugeSpec *value)
{
    value->minimum = UmiArchiveReadDouble(reader);
    value->maximum = UmiArchiveReadDouble(reader);
    value->value = UmiArchiveReadDouble(reader);
    value->warning_threshold = UmiArchiveReadDouble(reader);
    value->danger_threshold = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiDesignGaugeSpecArchiveValidate(const UmiDesignGaugeSpec *value)
{
    return umi_design_gauge_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_gauge_spec_archive_encode, umi_design_gauge_spec_archive_decode,
    UmiDesignGaugeSpec, UmiDesignGaugeSpecArchiveSchema, UmiDesignGaugeSpecArchiveBound, UmiDesignGaugeSpecArchiveWrite, UmiDesignGaugeSpecArchiveRead, UmiDesignGaugeSpecArchiveValidate)
