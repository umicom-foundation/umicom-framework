/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/readiness_report.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_READINESS_REPORT_H
#define UMICOM_APPLICATION_PRODUCTION_READINESS_REPORT_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"
#include "umicom/application/production/capability_readiness.h"
#include "umicom/application/runtime/readiness.h"

typedef struct UmiApplicationProductionReadinessReport {
    UmiApplicationProductionState state;
    UmiApplicationReadinessReport features;
    UmiApplicationProductionCapabilityReadiness capabilities;
    int adoption_ready;
    int layout_ready;
    int tests_ready;
    unsigned combined_percent;
} UmiApplicationProductionReadinessReport;

UmiStatus umi_application_production_readiness_report_build(
    const UmiApplicationProductionBinding *binding,
    const UmiApplicationProductionCapabilityReadiness *capabilities,
    UmiApplicationProductionReadinessReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
