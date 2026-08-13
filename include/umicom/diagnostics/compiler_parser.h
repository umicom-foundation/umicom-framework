/* Umicom Framework compiler diagnostic parser. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_DIAGNOSTICS_COMPILER_PARSER_H
#define UMICOM_DIAGNOSTICS_COMPILER_PARSER_H

#include "umicom/diagnostics/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiDiagnosticParser umi_compiler_diagnostic_parser(void);
UmiStatus umi_compiler_diagnostic_parse(const UmiOutputRecord *output,
                                        UmiDiagnosticSnapshot *out_diagnostic,
                                        int *out_matched,
                                        void *user_data);

#ifdef __cplusplus
}
#endif
#endif
