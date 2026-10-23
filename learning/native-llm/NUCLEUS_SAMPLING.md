# Top-p nucleus sampling with the native C23 decoder

**Umicom Framework — Native LLM learning laboratory**  
**Author and project originator:** Sammy Hegab · Umicom Foundation  
**Licence:** MIT

## What nucleus sampling changes

A decoder predicts a *logit* (an unnormalised score) for every token. The
existing `UmiAiTrainingSampleLogits` service optionally selects among the
highest `topK` candidates. Nucleus sampling adds a *probability-mass* cutoff:
select the shortest sorted prefix that accounts for at least `topP` of the
probability distributed over those `topK` candidates.

This is an inference-time choice. It does **not** change training, model
weights, the byte-token vocabulary, AdamW moments or checkpoint files.
`UmiAiTrainingSamplingOptions` and the original generation functions remain
valid and unchanged for existing consumers.

### A worked example

Suppose the allowed tokens are A, B, C and D, with logits `3, 2, 0, 0`,
`temperature=1`, and `topK=4`. Subtracting the largest logit before applying
`exp` produces unnormalised weights approximately
`1.0000, 0.3679, 0.0498, 0.0498`. The sum is `1.4675` and probabilities are
approximately `68.14%, 25.07%, 3.39%, 3.39%`.

| `topP` | Smallest retained prefix | Reason |
|---|---|---|
| 0.50 | A | A alone contributes more than 50%. |
| 0.90 | A, B | The first two contribute more than 90%. |
| 1.00 | A, B, C, D | Preserve all four `topK` candidates. |

Within the retained nucleus, the usual Framework top-K sampler performs the
seeded random draw after renormalising the retained weights. Ties are ordered
by ascending token ID. A single retained candidate is selected without
advancing the random state.

## Native C23 interface

The Framework exposes `UmiAiTrainingNucleusOptions` and three explicitly
opt-in interfaces in `umicom/ai/training/generation.h`:

- `UmiAiTrainingSampleLogitsNucleus`: select one token and optionally report the
  number of retained candidates.
- `UmiAiTrainingDecoderGenerateNucleus`: generate bounded token IDs from a
  decoder using the existing rolling-context inference path.
- `UmiAiTrainingDecoderGenerateBytesNucleus`: generate bytes with the existing
  260-token reversible byte vocabulary.

The policy requires `0 < topP <= 1`, `topK` between 1 and the number of
allowed tokens, and finite positive temperature. Every allowed logit must be
finite. The new selector reuses the original Framework sampler for the final
random draw, including the exact seeded-RNG behaviour when `topP=1`.

```c
#include "umicom/ai/training/generation.h"
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    const float logits[8] = {0, 0, 0, 0, 3, 2, 0, 0};
    UmiAiTrainingNucleusOptions policy = {1.0F, 4U, 0.90F};
    UmiAiTrainingRandomState random;
    size_t selected = 0U, retained = 0U;

    if (UmiAiTrainingRandomSeed(&random, UINT64_C(20261023)) != UMI_STATUS_OK)
        return 1;
    if (UmiAiTrainingSampleLogitsNucleus(logits, 8U, 4U, 8U,
          &policy, &random, &selected, &retained) != UMI_STATUS_OK)
        return 1;
    printf("selected token: %zu; retained candidates: %zu\n",
           selected, retained);
    return 0;
}
```

The numerical example intentionally limits the allowed token range to
`[4, 8)` to exclude reserved byte-token IDs. Applications should never assume
that an arbitrary decoder vocabulary has the same reserved-token convention.

## Compatibility and validation

Setting `topP=1.0f` delegates to the legacy sampler; its selected token, RNG
state and generated token sequence must agree for identical inputs and seeds.
Invalid policies must not modify the output token, retained-count report or
random state. The decoder-level APIs also preserve the existing failure-atomic
output and checkpoint contracts.

These claims are covered by the existing registered CTest cases
`framework.ai.training.generation` and
`framework.ai.training.generation_bytes`. Build the Framework's standalone
native-LLM laboratory with CMake and run both tests to exercise production
model generation. Running the entire native laboratory additionally checks
that other inference, training and checkpoint behaviours remain compatible.

Top-p is a sampling control, **not** a measure of model intelligence, safety,
programming correctness, or training-data rights. To evaluate a model, use a
held-out dataset and the independent model-quality and provenance services.
