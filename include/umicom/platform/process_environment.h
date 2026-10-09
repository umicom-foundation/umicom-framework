/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/process_environment.h
 * PURPOSE: Own reviewed child-environment overrides without mutating the host process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_PROCESS_ENVIRONMENT_H
#define UMICOM_PLATFORM_PROCESS_ENVIRONMENT_H
#include "umicom/platform/process.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PROCESS_ENVIRONMENT_TEXT_CAPACITY 1024U
#define UMI_PROCESS_ENVIRONMENT_ENTRY_CAPACITY 16U
    typedef struct UmiProcessEnvironmentPlan UmiProcessEnvironmentPlan;
    /** Check a terminated list of NAME=VALUE arguments. Quotes group spaces; no
 * shell expansion occurs. Input is bounded to 1,023 bytes and 16 entries.
 * Names use ASCII letters, digits and underscore, start with a letter or
 * underscore, and fit 63 bytes. Duplicate names, including case variants, are
 * refused for portability. Values must be valid UTF-8 without control bytes.
 * Empty values override with an empty string; no unset syntax is implied.
 * NULL is an empty list. This performs no environment read or process launch. */
    UmiStatus UmiProcessEnvironmentValidate(const char *definitions);
    /** Parse and own the complete override list. An optional absolute tools folder
 * is prepended to an explicit PATH value, or to a copied inherited PATH when no
 * PATH override was supplied. All other inherited variables remain inherited.
 * Failure clears out. Serialize host environment mutation with this capture.
 * The plan is ordinary configuration storage, not a secret vault. */
    UmiStatus UmiProcessEnvironmentPlanCreate(const char *definitions, const char *tool_directory,
                                              UmiProcessEnvironmentPlan **out);
    /** Borrow stable name/value pointers until plan destruction. Both outputs are
 * required and cleared on failure. Values are suitable for UmiProcessRequest;
 * callers must keep the plan alive until native launch consumes the request. */
    UmiStatus UmiProcessEnvironmentPlanRead(const UmiProcessEnvironmentPlan *plan,
                                            const UmiEnvironmentVariable **out_variables,
                                            size_t *out_count);
    /** Release the owned plan. NULL is accepted. No process or host state changes. */
    void UmiProcessEnvironmentPlanDestroy(UmiProcessEnvironmentPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
