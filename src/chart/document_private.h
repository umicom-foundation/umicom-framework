/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/document_private.h
 * PURPOSE: Keep immutable chart capture storage private to Framework.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DOCUMENT_PRIVATE_H
#define UMICOM_CHART_DOCUMENT_PRIVATE_H
#include "umicom/chart/document.h"
struct UmiChartDocument {
    UmiChartDocumentSummary summary;
    UmiChartDrawingSnapshot drawings[UMI_CHART_DRAWING_CAPACITY];
};
UmiStatus UmiChartDocumentValidateOwned(const UmiChartDocument *document);
#endif
