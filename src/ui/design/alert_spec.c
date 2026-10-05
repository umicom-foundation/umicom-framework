/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/alert_spec.c
 *
 * PURPOSE:
 *   Define inline and panel alert semantics with severity, dismissal and optional action.
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

#include "umicom/ui/design/alert_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/* Check that design alert spec satisfies its contract before another service relies on it. */
int umi_design_alert_spec_valid(const UmiDesignAlertSpec *spec) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return 0;
    if (memchr(spec->message, '\0', sizeof(spec->message)) == NULL) return 0;
 return spec!=NULL && (spec->severity>=UMI_UI_SEVERITY_INFORMATION && spec->severity<=UMI_UI_SEVERITY_ERROR && spec->message[0]!='\0' ) ? 1 : 0; }
/*
 * Initialise design alert spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_alert_spec_init(UmiDesignAlertSpec *spec, UmiUiSeverity severity, const char *message, int dismissible, int actionable)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    spec->severity = severity;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(spec->message, sizeof spec->message, message) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->dismissible = dismissible ? 1 : 0;
    spec->actionable = actionable ? 1 : 0;
    return umi_design_alert_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignAlertSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x59ac2f7b6cb9a98c);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignAlertSpec *)0)->message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignAlertSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiDesignAlertSpec *)0)->message) - 1U +
        8U +
        8U;
}
static void UmiDesignAlertSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignAlertSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->severity);
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteSigned(writer, (int64_t)value->dismissible);
    UmiArchiveWriteSigned(writer, (int64_t)value->actionable);
}
static void UmiDesignAlertSpecArchiveRead(UmiArchiveReader *reader, UmiDesignAlertSpec *value)
{
    value->severity = (UmiUiSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    value->dismissible = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->actionable = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignAlertSpecArchiveValidate(const UmiDesignAlertSpec *value)
{
    return umi_design_alert_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_alert_spec_archive_encode, umi_design_alert_spec_archive_decode,
    UmiDesignAlertSpec, UmiDesignAlertSpecArchiveSchema, UmiDesignAlertSpecArchiveBound, UmiDesignAlertSpecArchiveWrite, UmiDesignAlertSpecArchiveRead, UmiDesignAlertSpecArchiveValidate)
