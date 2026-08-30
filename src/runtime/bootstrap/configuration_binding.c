/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_binding.c
 *
 * PURPOSE:
 *   Implement the configuration binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_binding.c
 *
 * PURPOSE:
 *   Convert validated textual configuration into primitive C values.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_binding.h"


#include <errno.h>
#include <stdlib.h>
#include <string.h>
UmiStatus umi_bootstrap_configuration_bind_bool(const char *value,
                                                bool *out_value) {
    if (value == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0 ||
        strcmp(value, "yes") == 0 || strcmp(value, "on") == 0) {
        *out_value = true;
        return UMI_STATUS_OK;
    }
    if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0 ||
        strcmp(value, "no") == 0 || strcmp(value, "off") == 0) {
        *out_value = false;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}
UmiStatus umi_bootstrap_configuration_bind_i64(const char *value,
                                               int64_t *out_value) {
    char *end = NULL;
    long long parsed;
    if (value == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    parsed = strtoll(value, &end, 10);
    if (errno != 0 || end == value || end == NULL || *end != '\0')
        return UMI_STATUS_PARSE_ERROR;
    *out_value = (int64_t)parsed;
    return UMI_STATUS_OK;
}
