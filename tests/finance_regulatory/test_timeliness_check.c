#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/timeliness_check.h"

int main(void){ CHECK(umi_reg_timeliness_check(900,1000,100)==1); CHECK(umi_reg_timeliness_check(800,1000,100)==0); return 0; }
