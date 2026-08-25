#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include <math.h>
#include "umicom/finance/quant/random_sequence.h"

int main(void)
{
    UmiQuantRandomSequence a,b; double x,y; CHECK(umi_quant_random_sequence_init(&a,42U)==UMI_STATUS_OK); CHECK(umi_quant_random_sequence_init(&b,42U)==UMI_STATUS_OK); x=umi_quant_random_sequence_next(&a); y=umi_quant_random_sequence_next(&b); CHECK(x==y); CHECK(x>=0.0&&x<1.0);
    return 0;
}
