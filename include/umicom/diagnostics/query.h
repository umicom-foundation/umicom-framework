/* Umicom Framework diagnostic query contracts. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DIAGNOSTICS_QUERY_H
#define UMICOM_DIAGNOSTICS_QUERY_H

#include "umicom/diagnostics/filter.h"
#include "umicom/diagnostics/model.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A page remains safe for conventional Windows thread stacks; the model itself
 * retains up to 65,536 records and callers page with offset/limit. */
#define UMI_DIAGNOSTIC_QUERY_RESULT_MAX 64U

typedef enum UmiDiagnosticSort {
    UMI_DIAGNOSTIC_SORT_SEQUENCE_ASCENDING = 0,
    UMI_DIAGNOSTIC_SORT_SEQUENCE_DESCENDING = 1,
    UMI_DIAGNOSTIC_SORT_SEVERITY_DESCENDING = 2
} UmiDiagnosticSort;

typedef struct UmiDiagnosticQuery {
    UmiDiagnosticFilter filter;
    UmiDiagnosticSort sort;
    size_t offset;
    size_t limit;
} UmiDiagnosticQuery;

typedef struct UmiDiagnosticQueryResult {
    UmiDiagnosticSnapshot items[UMI_DIAGNOSTIC_QUERY_RESULT_MAX];
    size_t count;
    size_t total_matches;
    int truncated;
} UmiDiagnosticQueryResult;

void umi_diagnostic_query_init(UmiDiagnosticQuery *query);
UmiStatus umi_diagnostic_query_execute(const UmiDiagnosticModel *model,
                                       const UmiDiagnosticQuery *query,
                                       UmiDiagnosticQueryResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
