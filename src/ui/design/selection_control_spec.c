/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/selection_control_spec.c
 *
 * PURPOSE:
 *   Define checkbox, radio, switch and drop-down selection semantics.
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

#include "umicom/ui/design/selection_control_spec.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Check that design selection control spec satisfies its contract before another service
 * relies on it.
 */
int umi_design_selection_control_spec_valid(const UmiDesignSelectionControlSpec *spec) { return spec!=NULL && ((spec->kind==UMI_UI_COMPONENT_CHECK_BUTTON || spec->kind==UMI_UI_COMPONENT_SWITCH || spec->kind==UMI_UI_COMPONENT_DROP_DOWN) && !(spec->kind==UMI_UI_COMPONENT_SWITCH && spec->multiple)) ? 1 : 0; }
/*
 * Initialise design selection control spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_selection_control_spec_init(UmiDesignSelectionControlSpec *spec, UmiUiComponentKind kind, int selected, int multiple, int tri_state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->kind=kind;spec->selected=selected?1:0;spec->multiple=multiple?1:0;spec->tri_state=tri_state?1:0;
    return umi_design_selection_control_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDesignSelectionControlSpecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x21110d3da31646e1);

    return schema;
}
static size_t UmiDesignSelectionControlSpecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDesignSelectionControlSpecArchiveWrite(UmiArchiveWriter *writer, const UmiDesignSelectionControlSpec *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->selected);
    UmiArchiveWriteSigned(writer, (int64_t)value->multiple);
    UmiArchiveWriteSigned(writer, (int64_t)value->tri_state);
}
static void UmiDesignSelectionControlSpecArchiveRead(UmiArchiveReader *reader, UmiDesignSelectionControlSpec *value)
{
    value->kind = (UmiUiComponentKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->selected = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->multiple = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->tri_state = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiDesignSelectionControlSpecArchiveValidate(const UmiDesignSelectionControlSpec *value)
{
    return umi_design_selection_control_spec_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_design_selection_control_spec_archive_encode, umi_design_selection_control_spec_archive_decode,
    UmiDesignSelectionControlSpec, UmiDesignSelectionControlSpecArchiveSchema, UmiDesignSelectionControlSpecArchiveBound, UmiDesignSelectionControlSpecArchiveWrite, UmiDesignSelectionControlSpecArchiveRead, UmiDesignSelectionControlSpecArchiveValidate)
