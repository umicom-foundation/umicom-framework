#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/cross_field_validation.h"

int main(void){ UmiCrossFieldValidation r; CHECK(umi_reg_cross_field_validation_init(&r,"balance",0.01)==UMI_STATUS_OK); CHECK(umi_reg_cross_field_validation_passes(&r,10.0,10.005)==1); return 0; }
