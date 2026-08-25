#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/quant/calibration_target.h"

int main(void)
{
    UmiQuantCalibrationTarget value;
    CHECK(umi_quant_calibration_target_init(&value, 0.20, 2.0, 0.0001) == UMI_STATUS_OK);
    CHECK(umi_quant_calibration_target_weighted_target(&value) > 0.399 && umi_quant_calibration_target_weighted_target(&value) < 0.401);
    return 0;
}
