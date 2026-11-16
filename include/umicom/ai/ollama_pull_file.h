/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/ollama_pull_file.h
 *
 * PURPOSE:
 *   Expose bounded offline progress-file observations and one shared metadata-only console command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_OLLAMA_PULL_FILE_H
#define UMICOM_AI_OLLAMA_PULL_FILE_H
#include "umicom/ai/ollama_pull.h"
#include "umicom/base/sha256.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_OLLAMA_PULL_FILE_JSON_CAPACITY 1024U
typedef struct UmiOllamaPullFileReport {
    uint64_t bytes;
    size_t events;
    UmiOllamaPullEventKind provider_outcome;
    unsigned char file_sha256[UMI_SHA256_BYTES];
} UmiOllamaPullFileReport;
/* Read one selected regular file through the shared chunk visitor. Require an
 * ordinary absolute UTF-8 path and trusted existing parents; the final leaf
 * cannot be a symlink/reparse point or device. Parent aliases may be followed.
 * The same handle is checked before/after reading; this is not an immutable
 * filesystem snapshot. Maximum 16 MiB; all progress-stream bounds still apply.
 *
 * Publish only after complete read/close, protocol finish and digest success.
 * Well-formed provider error transcripts return OK with provider_outcome ERROR;
 * parsing/read/cancellation errors preserve the entire output. The SHA-256 is
 * of the observed file bytes, not a model digest or authenticated server record.
 * No endpoint is contacted or model installed. Status/error text is not retained
 * in this report. Call from a worker for GUI use; I/O is blocking and cancellation
 * is cooperative between reads and records. Inputs/output must not overlap. */
UmiStatus UmiOllamaPullFileInspect(const char *path,const UmiCancellationToken *cancel,
    UmiOllamaPullFileReport *out);
/* Validate report bounds/outcome and stage canonical metadata JSON. A caller
 * may construct a report; serialization does not verify its provenance.
 * Capacity includes NUL; out_length excludes it. Both outputs stay unchanged
 * on failure. No provider message, model ID or filesystem path is emitted. */
UmiStatus UmiOllamaPullFileJson(const UmiOllamaPullFileReport *report,
    char *out,size_t capacity,size_t *out_length);
/* Args exclude executable and optional models prefix. Accept only
 * inspect-pull ABSOLUTE_NDJSON or inspect-pull --help. Exit 0 = complete
 * provider-success observation; 3 = complete provider-error observation;
 * 2 = usage; 1 = file, protocol or output failure. No network or model effects. */
int UmiOllamaPullFileCommand(int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
