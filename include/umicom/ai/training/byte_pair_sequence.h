/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/byte_pair_sequence.h
 *
 * PURPOSE:
 *   Prepare one bounded native decoder example with explicit shifting, padding and completion-only supervision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_BYTE_PAIR_SEQUENCE_H
#define UMICOM_AI_TRAINING_BYTE_PAIR_SEQUENCE_H
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/decoder.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Geometry can be inspected before copying text or constructing token arrays.
 * Counts are native byte-token rows, not Unicode characters or provider tokens. */
typedef struct UmiAiTrainingBytePairShape {
    size_t validRows;
    size_t selectedRows;
} UmiAiTrainingBytePairShape;

/* Measure promptBytes + completionBytes + one EOS prediction, with overflow
 * checks. A completion must be nonempty. This is independent of a chosen
 * context size: callers compare validRows with their admitted context bound.
 * No text is read, memory allocated or model run. Every refusal preserves
 * outShape. A successful measurement does not admit source pointers or prove
 * that the resulting example fits a model or memory budget. */
UmiStatus UmiAiTrainingBytePairMeasure(size_t promptBytes, size_t completionBytes,
    UmiAiTrainingBytePairShape *outShape);

/* One example, not a packed batch. Arrays are fixed to the native teaching
 * decoder context bound. No pointer into the original text is retained. */
typedef struct UmiAiTrainingBytePairSequence {
    size_t inputs[UMI_AI_DECODER_MAX_CONTEXT];
    size_t targets[UMI_AI_DECODER_MAX_CONTEXT];
    unsigned char attention[UMI_AI_DECODER_MAX_CONTEXT];
    unsigned char selected[UMI_AI_DECODER_MAX_CONTEXT];
    size_t context_length;
    size_t valid_rows;
    size_t selected_rows;
} UmiAiTrainingBytePairSequence;

/* Explicit educational format: [BOS, prompt bytes, completion bytes, EOS].
 * Input/target rows are adjacent elements of that stream; targets are already
 * shifted once. Byte IDs use UmiAiByteEncode. No separator, role marker, chat
 * template, text normalization or UTF-8 validation is invented. Include any
 * desired separators in the supplied prompt. This is NOT a pretrained model's
 * tokenizer or template. Use a matching 260-entry native decoder vocabulary.
 *
 * Require contextLength in 1..MAX_CONTEXT and a nonempty completion. Prompt
 * may be empty (and NULL only when empty). The valid row count is
 * promptBytes + completionBytes + 1, including the EOS prediction. Refuse
 * overlong input without truncation. Attention is 1 for valid rows, while
 * selected is 1 only for completion-byte targets and EOS. Right padding has
 * PAD inputs/targets and zero masks. Every slot beyond contextLength is also
 * initialized to PAD/zero, but only contextLength rows belong to the example.
 *
 * Read the exact byte lengths, including embedded NUL. Caller spans are valid
 * for those sizes, stable during this synchronous call, and disjoint from the
 * writable output. Source spans may overlap each other. No heap allocation,
 * I/O, model execution or retained pointers. Every refusal preserves output.
 * Masks describe the objective and causal context separately; excluded prompt
 * targets do not prevent prompt embeddings from receiving later gradients.
 */
UmiStatus UmiAiTrainingBytePairPrepare(const uint8_t *prompt, size_t promptBytes,
    const uint8_t *completion, size_t completionBytes, size_t contextLength,
    UmiAiTrainingBytePairSequence *outSequence);
#ifdef __cplusplus
}
#endif
#endif
