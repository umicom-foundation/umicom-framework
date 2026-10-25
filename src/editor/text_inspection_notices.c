/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_inspection_notices.c
 *
 * PURPOSE:
 *   Classify invisible text for human review without inferring intent or changing the source.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "text_inspection_internal.h"
UmiEditorTextNoticeKind InspectionNoticeKind(uint32_t c) {
  if (c == 0x061cU || c == 0x200eU || c == 0x200fU || (c >= 0x202aU && c <= 0x202eU) ||
      (c >= 0x2066U && c <= 0x2069U))
    return UMI_EDITOR_TEXT_BIDI_CONTROL;
  if (c == 0x200bU || c == 0x200cU || c == 0x200dU || c == 0x2060U || c == 0xfeffU)
    return UMI_EDITOR_TEXT_ZERO_WIDTH;
  if (c == 0x00a0U || c == 0x202fU)
    return UMI_EDITOR_TEXT_NONBREAKING_SPACE;
  if ((c < 0x20U && c != '\t' && c != '\n' && c != '\r') || (c >= 0x7fU && c <= 0x9fU))
    return UMI_EDITOR_TEXT_CONTROL;
  return (UmiEditorTextNoticeKind)0;
}
