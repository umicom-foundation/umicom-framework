/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/kpi_spec.h
 *
 * PURPOSE:
 *   Define dashboard KPI value, trend and semantic status presentation.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_KPI_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_KPI_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignKpiSpec {
    char label[UMI_DESIGN_TEXT_CAPACITY];
    double value;
    double change;
    UmiDesignSemanticRole role;
    int percentage;
} UmiDesignKpiSpec;

/* Initialise the semantic kpi spec specification. */
UmiStatus umi_design_kpi_spec_init(UmiDesignKpiSpec *spec, const char *label, double value, double change, UmiDesignSemanticRole role, int percentage);
/* Return one when the semantic specification is internally consistent. */
int umi_design_kpi_spec_valid(const UmiDesignKpiSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
