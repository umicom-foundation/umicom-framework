/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/codec.h
 *
 * PURPOSE:
 *   Save and load editable workspace drafts using a small, versioned text
 *   format that can be inspected in a text editor and stored in source control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_CODEC_H
#define UMICOM_APPLICATION_COMPONENT_CODEC_H

#include "umicom/application/component/draft.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_COMPONENT_WORKSPACE_FORMAT_VERSION 2U

/**
 * Write application component workspace in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus
umi_application_component_workspace_encode(const UmiApplicationComponentWorkspaceDraft *draft,
                                           char *buffer, size_t capacity, size_t *out_length);
/**
 * Read application component workspace into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus
umi_application_component_workspace_decode(const char *text, size_t length,
                                           UmiApplicationComponentWorkspaceDraft *out_draft);

#ifdef __cplusplus
}
#endif

#endif
