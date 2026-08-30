/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/operations_catalogue.h
 *
 * PURPOSE:
 *   Publish the public operations catalogue contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_OPERATIONS_CATALOGUE_H
#define UMICOM_OBSERVABILITY_OPERATIONS_CATALOGUE_H
#include "umicom/observability/operations_centre.h"
typedef enum UmiOperationsViewKind { UMI_OPERATIONS_VIEW_OVERVIEW = 1, UMI_OPERATIONS_VIEW_METRICS, UMI_OPERATIONS_VIEW_TRACES, UMI_OPERATIONS_VIEW_PROFILER, UMI_OPERATIONS_VIEW_BENCHMARKS, UMI_OPERATIONS_VIEW_CRASHES, UMI_OPERATIONS_VIEW_HEALTH, UMI_OPERATIONS_VIEW_RESILIENCE, UMI_OPERATIONS_VIEW_EVENTS, UMI_OPERATIONS_VIEW_AUDIT } UmiOperationsViewKind;
typedef struct UmiOperationsViewDescriptor { UmiOperationsViewKind kind; char id[UMI_OPERATIONS_ID_CAPACITY]; char title[UMI_OPERATIONS_NAME_CAPACITY]; char description[UMI_OPERATIONS_TEXT_CAPACITY]; } UmiOperationsViewDescriptor;
size_t umi_operations_catalogue_count(void);
const UmiOperationsViewDescriptor *umi_operations_catalogue_at(size_t index);
const UmiOperationsViewDescriptor *umi_operations_catalogue_find(const char *id);
#endif
