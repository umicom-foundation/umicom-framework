#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/report_validation_result.h"

int main(void){ UmiReportValidationResult r; CHECK(umi_reg_report_validation_result_evaluate(&r,"rule",5.0,0.0,10.0,UMI_REG_SEVERITY_ERROR)==UMI_STATUS_OK); CHECK(r.passed==1); return 0; }
