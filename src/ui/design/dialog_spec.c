/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/dialog_spec.c
 *
 * PURPOSE:
 *   Define dialog modality, sizing and governed action semantics.
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

#include "umicom/ui/design/dialog_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design dialog spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_dialog_spec_valid(const UmiDesignDialogSpec *spec) { return spec!=NULL && (spec->width_class>=UMI_DESIGN_SIZE_COMPACT && spec->width_class<=UMI_DESIGN_SIZE_WIDE && spec->action_count>0U && spec->action_count<=8U && (!spec->destructive_action || spec->action_count>=2U)) ? 1 : 0; }
/*
 * Initialise design dialog spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_dialog_spec_init(UmiDesignDialogSpec *spec, UmiDesignSizeClass width_class, uint16_t action_count, int modal, int destructive_action)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->width_class=width_class;spec->action_count=action_count;spec->modal=modal?1:0;spec->destructive_action=destructive_action?1:0;
    return umi_design_dialog_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignDialogSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf23ba11eae8f4fae);

    return schema;
}
static size_t UmiDesignDialogSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignDialogSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignDialogSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->width_class);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->action_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->modal);
    UmiArchiveWriteSigned(writer, (int64_t)value->destructive_action);
}
static void UmiDesignDialogSpecArchiveRead(UmiArchiveReader *reader, UmiDesignDialogSpec *value)
{
    value->width_class = (UmiDesignSizeClass)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->action_count = (uint16_t)UmiArchiveReadUnsigned(reader, UINT16_MAX);
    value->modal = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->destructive_action = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignDialogSpecArchiveValidate(const UmiDesignDialogSpec *value)
{
    return umi_design_dialog_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_dialog_spec_archive_encode, umi_design_dialog_spec_archive_decode,
    UmiDesignDialogSpec, UmiDesignDialogSpecArchiveSchema, UmiDesignDialogSpecArchiveBound, UmiDesignDialogSpecArchiveWrite, UmiDesignDialogSpecArchiveRead, UmiDesignDialogSpecArchiveValidate)
