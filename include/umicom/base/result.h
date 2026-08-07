#ifndef UMICOM_BASE_RESULT_H
#define UMICOM_BASE_RESULT_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiResult {
    UmiStatus status;
    const char *message;
} UmiResult;

UmiResult umi_result_ok(void);
UmiResult umi_result_error(UmiStatus status, const char *message);
int umi_result_is_ok(UmiResult result);

#ifdef __cplusplus
}
#endif

#endif
