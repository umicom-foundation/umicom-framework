/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/form_field_spec.c
 *
 * PURPOSE:
 *   Define labels, help text, required state and validation severity for reusable form fields.
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

#include "umicom/ui/design/form_field_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design form field spec satisfies its contract before another service relies
 * on it.
 */
int umi_design_form_field_spec_valid(const UmiDesignFormFieldSpec *spec) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (spec == NULL) return 0;
    if (memchr(spec->label, '\0', sizeof(spec->label)) == NULL) return 0;
    if (memchr(spec->help_text, '\0', sizeof(spec->help_text)) == NULL) return 0;
 return spec!=NULL && (spec->label[0]!='\0' && spec->validation_severity>=UMI_UI_SEVERITY_INFORMATION && spec->validation_severity<=UMI_UI_SEVERITY_ERROR) ? 1 : 0; }
/*
 * Initialise design form field spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_form_field_spec_init(UmiDesignFormFieldSpec *spec, const char *label, const char *help_text, int required, UmiUiSeverity validation_severity)
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
        if (label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(spec->label, sizeof spec->label, label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (help_text != NULL && umi_design_copy_text(spec->help_text, sizeof spec->help_text, help_text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->required = required ? 1 : 0;
    spec->validation_severity = validation_severity;
    return umi_design_form_field_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignFormFieldSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd25bd4ac852e7432);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignFormFieldSpec *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDesignFormFieldSpec *)0)->help_text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDesignFormFieldSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDesignFormFieldSpec *)0)->label) - 1U +
        8U + sizeof(((UmiDesignFormFieldSpec *)0)->help_text) - 1U +
        8U +
        8U;
}
static void UmiDesignFormFieldSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignFormFieldSpec *value)
{
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->help_text, sizeof(value->help_text));
    UmiArchiveWriteSigned(writer, (int64_t)value->required);
    UmiArchiveWriteSigned(writer, (int64_t)value->validation_severity);
}
static void UmiDesignFormFieldSpecArchiveRead(UmiArchiveReader *reader, UmiDesignFormFieldSpec *value)
{
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->help_text, sizeof(value->help_text));
    value->required = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->validation_severity = (UmiUiSeverity)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignFormFieldSpecArchiveValidate(const UmiDesignFormFieldSpec *value)
{
    return umi_design_form_field_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_form_field_spec_archive_encode, umi_design_form_field_spec_archive_decode,
    UmiDesignFormFieldSpec, UmiDesignFormFieldSpecArchiveSchema, UmiDesignFormFieldSpecArchiveBound, UmiDesignFormFieldSpecArchiveWrite, UmiDesignFormFieldSpecArchiveRead, UmiDesignFormFieldSpecArchiveValidate)
