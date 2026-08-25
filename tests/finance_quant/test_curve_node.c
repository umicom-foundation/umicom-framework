#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/curve_node.h"

int main(void)
{
    UmiQuantCurveNode value;
    CHECK(umi_quant_curve_node_init(&value, 365, 0.04) == UMI_STATUS_OK);
    CHECK(umi_quant_curve_node_quoted_value(&value) > 0.039 && umi_quant_curve_node_quoted_value(&value) < 0.041);
    return 0;
}
