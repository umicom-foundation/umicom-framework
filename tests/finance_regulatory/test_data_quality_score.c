#include <stdio.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/data_quality_score.h"

int main(void){ double s[2]={1.0,0.5},w[2]={1.0,1.0},o=0.0; CHECK(umi_reg_data_quality_score_compute(s,w,2U,&o)==UMI_STATUS_OK); CHECK(o>0.74&&o<0.76); return 0; }
