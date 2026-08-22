/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/instrument_selection.h
 *
 * PURPOSE:
 *   Define the reusable instrument selection snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_INSTRUMENT_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_INSTRUMENT_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceInstrumentSelection {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceInstrumentSelection;

void umi_workbench_context_source_instrument_selection_init(
    UmiWorkbenchContextSourceInstrumentSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_instrument_selection_validate(
    const UmiWorkbenchContextSourceInstrumentSelection *record);
UmiStatus umi_workbench_context_source_instrument_selection_set_source(
    UmiWorkbenchContextSourceInstrumentSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_instrument_selection_set_panel(
    UmiWorkbenchContextSourceInstrumentSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_instrument_selection_set_subject(
    UmiWorkbenchContextSourceInstrumentSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_instrument_selection_set_group(
    UmiWorkbenchContextSourceInstrumentSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_instrument_selection_set_label(
    UmiWorkbenchContextSourceInstrumentSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_instrument_selection_hash(
    const UmiWorkbenchContextSourceInstrumentSelection *record);
void umi_workbench_context_source_instrument_selection_touch(
    UmiWorkbenchContextSourceInstrumentSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
