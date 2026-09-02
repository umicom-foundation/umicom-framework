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
/**
 * List the named operations view kind values accepted by this public contract.
 */
typedef enum UmiOperationsViewKind { UMI_OPERATIONS_VIEW_OVERVIEW = 1, UMI_OPERATIONS_VIEW_METRICS, UMI_OPERATIONS_VIEW_TRACES, UMI_OPERATIONS_VIEW_PROFILER, UMI_OPERATIONS_VIEW_BENCHMARKS, UMI_OPERATIONS_VIEW_CRASHES, UMI_OPERATIONS_VIEW_HEALTH, UMI_OPERATIONS_VIEW_RESILIENCE, UMI_OPERATIONS_VIEW_EVENTS, UMI_OPERATIONS_VIEW_AUDIT } UmiOperationsViewKind;
/**
 * Represent the operations view descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsViewDescriptor { UmiOperationsViewKind kind; char id[UMI_OPERATIONS_ID_CAPACITY]; char title[UMI_OPERATIONS_NAME_CAPACITY]; char description[UMI_OPERATIONS_TEXT_CAPACITY]; } UmiOperationsViewDescriptor;
/**
 * Return the number of records represented by operations catalogue without changing their
 * state.
 */
size_t umi_operations_catalogue_count(void);
/**
 * Find operations catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiOperationsViewDescriptor *umi_operations_catalogue_at(size_t index);
/**
 * Find operations catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiOperationsViewDescriptor *umi_operations_catalogue_find(const char *id);
#endif
