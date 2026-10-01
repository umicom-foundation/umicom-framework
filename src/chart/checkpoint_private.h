/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/checkpoint_private.h
 * PURPOSE: Share chart checkpoint encoding and metadata within the storage service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_CHECKPOINT_PRIVATE_H
#define UMICOM_CHART_CHECKPOINT_PRIVATE_H
#include "document_private.h"
#include "umicom/chart/checkpoint.h"
#include "umicom/workbench_layout_data/value_codec.h"
#define UMI_CHART_CHECKPOINT_VALUE 3900U
typedef struct ChartCheckpointMetadata {
    char scope[128];
    UmiChartDocumentSummary summary;
    uint64_t storageRevision, savedAtMs;
} ChartCheckpointMetadata;
int UmiChartCheckpointIdentityValid(const char *identity);
UmiStatus UmiChartCheckpointEncodeMetadata(UmiWorkbenchLayoutDataFieldSet *fields,
    const ChartCheckpointMetadata *metadata, char output[UMI_CHART_CHECKPOINT_VALUE]);
UmiStatus UmiChartCheckpointDecodeMetadata(UmiWorkbenchLayoutDataFieldSet *fields,
    const char *text, ChartCheckpointMetadata *outMetadata);
UmiStatus UmiChartCheckpointEncodeDrawing(UmiWorkbenchLayoutDataFieldSet *fields,
    const UmiChartDrawingSnapshot *drawing, char output[UMI_CHART_CHECKPOINT_VALUE]);
UmiStatus UmiChartCheckpointDecodeDrawing(UmiWorkbenchLayoutDataFieldSet *fields,
    const char *text, UmiChartDrawingSnapshot *outDrawing);
#endif
