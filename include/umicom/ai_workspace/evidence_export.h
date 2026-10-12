/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workspace/evidence_export.h
 * PURPOSE: Save an immutable AI source inspection to an explicitly chosen local file.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_EVIDENCE_EXPORT_H
#define UMICOM_AI_WORKSPACE_EVIDENCE_EXPORT_H
#include "umicom/ai_workspace/evidence.h"
#include "umicom/platform/output_file.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Write one captured AI inspection as plain UTF-8 text to a new file.
 * @param review Immutable capture; keep it alive throughout this synchronous call.
 * @param path Absolute destination in an existing directory controlled by the caller.
 * @param outFile Optional final file status. Unchanged until a file is created.
 * @return OK only after formatting, writing and closing all succeed.
 *
 * Formatting finishes before exclusive file creation. Existing files are never
 * replaced. After creation, outFile reports accepted bytes and close status even
 * if writing fails; a partial new file is retained for inspection. The result is
 * not a guarantee against power loss. Run on a worker if storage could block.
 * This reads only the capture: no database refresh, model request, approval,
 * tool execution or network transmission occurs. Prompt, response and source
 * passages can be private; the user chooses whether to share the resulting file.
 */
UmiStatus UmiAiEvidenceWriteNew(const UmiAiEvidenceReview *review, const char *path,
    UmiOutputFileSnapshot *outFile);
#ifdef __cplusplus
}
#endif
#endif
