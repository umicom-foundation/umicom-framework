#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/data_quality_gate.h"

int main(void){ CHECK(umi_reg_data_quality_gate_passes(0.99,0.98)==1); CHECK(umi_reg_data_quality_gate_passes(0.90,0.98)==0); return 0; }
