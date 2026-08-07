#include "umicom/base/result.h"

UmiResult umi_result_ok(void)
{
    UmiResult result = {UMI_STATUS_OK, "OK"};
    return result;
}

UmiResult umi_result_error(UmiStatus status, const char *message)
{
    UmiResult result = {status, message != 0 ? message : umi_status_text(status)};
    return result;
}

int umi_result_is_ok(UmiResult result)
{
    return result.status == UMI_STATUS_OK;
}
