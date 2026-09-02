/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/calibration_graph.h
 *
 * PURPOSE:
 *   Maintain a bounded acyclic dependency graph for calibration workflows.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_GRAPH_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_GRAPH_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/calibration_job.h"
#include "umicom/finance/enterprise/calibration_dependency.h"

/**
 * Represent the enterprise calibration graph data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseCalibrationGraph { UmiEnterpriseCalibrationJob jobs[32U]; size_t job_count; UmiEnterpriseCalibrationDependency dependencies[64U]; size_t dependency_count; } UmiEnterpriseCalibrationGraph;
/* Initialise an empty calibration dependency graph. */
void umi_enterprise_calibration_graph_init(UmiEnterpriseCalibrationGraph *graph);
/* Add a unique job to the graph. */
UmiStatus umi_enterprise_calibration_graph_add_job(UmiEnterpriseCalibrationGraph *graph,const UmiEnterpriseCalibrationJob *job);
/* Add a dependency only when both jobs exist and the edge does not introduce a cycle. */
UmiStatus umi_enterprise_calibration_graph_add_dependency(UmiEnterpriseCalibrationGraph *graph,const UmiEnterpriseCalibrationDependency *dependency);

#ifdef __cplusplus
}
#endif

#endif
