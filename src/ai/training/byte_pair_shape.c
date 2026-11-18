/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/byte_pair_shape.c
 *
 * PURPOSE:
 *   Measure native byte-pair prediction and supervision rows without reading or retaining text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/byte_pair_sequence.h"
#include <stdint.h>

UmiStatus UmiAiTrainingBytePairMeasure(size_t promptBytes, size_t completionBytes,
    UmiAiTrainingBytePairShape *outShape)
{
    if(outShape==NULL || completionBytes==0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Reserve the EOS row before addition. This same measurement serves the
     * concrete sequence preparer and dataset audit, preventing formula drift. */
    if(promptBytes>SIZE_MAX-1U || completionBytes>SIZE_MAX-1U-promptBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiTrainingBytePairShape candidate={promptBytes+completionBytes+1U,
        completionBytes+1U};
    *outShape=candidate;
    return UMI_STATUS_OK;
}
