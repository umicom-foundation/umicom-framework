/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_appearance/fixture.h
 * PURPOSE: Share small drawing inputs and assertion helpers for appearance contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPEARANCE_TEST_FIXTURE_H
#define UMICOM_APPEARANCE_TEST_FIXTURE_H
#include "umicom/chart/drawing_appearance.h"
#include "umicom/chart/drawing_edit.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
#define NEAR(a,b) CHECK(fabs((a)-(b)) < 1e-9)
static inline UmiChartDrawingSnapshot Drawing(UmiChartDrawingKind kind)
{
    UmiChartDrawingSnapshot drawing;
    OK(UmiChartDrawingInitialize("drawing", "pane", kind, (UmiChartPoint){120, 25}, (UmiChartPoint){180, 75}, &drawing));
    return drawing;
}
#endif
