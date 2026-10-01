/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_checkpoint/fixture.h
 * PURPOSE: Share deterministic drawing fixtures and strict field comparisons.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_CHECKPOINT_TEST_FIXTURE_H
#define UMICOM_CHART_CHECKPOINT_TEST_FIXTURE_H
#include "umicom/chart/checkpoint.h"
#include "umicom/chart/drawing_validation.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static inline UmiChartDrawingSnapshot Drawing(const char *id, const char *pane)
{
    UmiChartDrawingSnapshot drawing = {0};
    strcpy(drawing.id, id); strcpy(drawing.pane_id, pane); strcpy(drawing.tool, "trend");
    strcpy(drawing.style, "colour=blue;label=caf\xc3\xa9\nwidth=2%exact");
    drawing.time1 = 60000; drawing.time2 = 120000; drawing.value1 = 100.25; drawing.value2 = 110.125;
    drawing.selected = 1; drawing.locked = 1; drawing.revision = 17U; return drawing;
}
static inline UmiChartDocument *Document(const char *pane, double value)
{
    UmiChartDrawingSnapshot drawing = Drawing("drawing.alpha", pane); drawing.value2 = value;
/* The appended timeframe defaults to source bars. This explicit zero retains the fixture semantics when all consumers are rebuilt. The previous implementation remains for engineering review. */
#if 0
    UmiChartNavigation navigation = {20U, 120000, 1}; UmiChartDocument *document = NULL;
#endif
    UmiChartNavigation navigation = {20U, 120000, 1, 0U}; UmiChartDocument *document = NULL;
    CHECK(UmiChartDocumentCreate(pane, &navigation, &drawing, 1U, 5U, &document) == UMI_STATUS_OK);
    return document;
}
static inline void SameDrawing(const UmiChartDrawingSnapshot *left, const UmiChartDrawingSnapshot *right)
{
    CHECK(strcmp(left->id, right->id) == 0 && strcmp(left->pane_id, right->pane_id) == 0);
    CHECK(strcmp(left->tool, right->tool) == 0 && strcmp(left->style, right->style) == 0);
    CHECK(left->time1 == right->time1 && left->time2 == right->time2);
    CHECK(memcmp(&left->value1, &right->value1, sizeof(double)) == 0);
    CHECK(memcmp(&left->value2, &right->value2, sizeof(double)) == 0);
    CHECK(left->selected == right->selected && left->locked == right->locked && left->revision == right->revision);
    CHECK(left->visibility_flags == right->visibility_flags);
}
static inline double Value(const UmiChartDocument *document)
{ UmiChartDrawingSnapshot drawing; CHECK(UmiChartDocumentDrawingAt(document, 0U, &drawing) == UMI_STATUS_OK); return drawing.value2; }
#endif
