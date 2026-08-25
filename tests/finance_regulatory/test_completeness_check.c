#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/completeness_check.h"

int main(void){ double s=0.0; CHECK(umi_reg_completeness_check(10U,9U,&s)==UMI_STATUS_OK); CHECK(s>0.89&&s<0.91); return 0; }
