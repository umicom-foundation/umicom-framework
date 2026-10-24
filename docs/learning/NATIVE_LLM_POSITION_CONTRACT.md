# Umicom Native LLM — Position Policy and Checkpoint Compatibility

**Learning laboratory: Batches 77–78**  
**Author:** Sammy Hegab · Umicom Foundation  
**Primary implementation:** native C23 · MIT

## 1. Why this boundary is necessary

The original Umicom training decoder learns an absolute positional embedding for
positions `0..contextLength-1`. Its weights depend on that representation. Its
traditional `UmiAiTrainingDecoderGenerateNucleus` path left-pads a short prompt
with BOS until the full context is populated. The opt-in growing-prefix path
(`UmiAiTrainingDecoderGeneratePrefixNucleus`) starts at absolute position zero
without BOS left padding. These generation policies can use the same checkpoint,
but they **must not** be described as providing identical token predictions.

When a full context rolls, surviving tokens move to different absolute
positions. The existing KV cache consequently rebuilds all retained key/value
rows. Batches 69–76 measured this behaviour without claiming a false speedup.
Changing to rotary or another relative/offset-invariant position encoding
requires an expressly compatible model architecture, trained parameters,
checkpoint format, and independent parity tests. It is not a safe flag flip.

Batch 77–78 introduces a compatibility guard **without introducing new neural
mathematics**. A separate 128-byte record binds:

- A precisely identified current-model positional strategy (`learned-absolute`).
- An explicitly selected generation context (`bos-left-padded` or
  `growing-unpadded`).
- The required rollover policy (`rebuild`).
- Decoder configuration, initial seed and completed optimiser steps.
- The SHA-256 digest of the **actual canonical encoded checkpoint bytes**.

The ordinary model checkpoint is **not changed**. A saved positional sidecar
is portable data, not a new checkpoint and not an instruction to migrate one.

## 2. Framework-owned entry points

The public header is:

`include/umicom/ai/training/position_contract.h`

| Function | Responsibility |
|---|---|
| `UmiAiTrainingPositionContractBuild` | Inspect decoder, serialize the canonical checkpoint in memory, compute its SHA-256 and stage the declaration. |
| `UmiAiTrainingPositionContractEncode` | Serialize that declaration into exactly 128 portable little-endian bytes with a record integrity digest. |
| `UmiAiTrainingPositionContractDecode` | Strictly validate a record's format, checksums and decoder shapes without loading any model. |
| `UmiAiTrainingPositionContractVerify` | Re-encode and hash the live model, then refuse mismatched weights, parameters, steps or generation context. |

There are **no** functions that turn existing learned-position checkpoints into
rotary or relative-position models. Unknown strategies, unqualified versions,
or unsupported rollover policies are rejected. The code retains the caller's
outputs intact after any failure. The caller is responsible for serialising
model access; no API may run concurrently with training or model destruction.

## 3. Portable record layout

All multi-byte integers are unsigned and explicitly encoded little-endian.
Do not write `sizeof(UmiAiTrainingPositionContract)` bytes to disk: the C
structure is a public in-memory record, **not** a portable on-disk schema.

| Byte offsets | Size | Meaning |
|---|---:|---|
| 0–7 | 8 | Literal ASCII `UMIPOS01` |
| 8–11 | 4 | Record format: `1` |
| 12–15 | 4 | Positional encoding: `1` = learned absolute |
| 16–19 | 4 | Generation context: `1` = BOS-left-padded, `2` = growing-unpadded |
| 20–23 | 4 | Rollover behaviour: `1` = rebuild |
| 24–43 | 20 | Vocabulary size, context length, channels, hidden channels, attention heads |
| 44–47 | 4 | Number of canonical checkpoint bytes |
| 48–55 | 8 | Decoder initial seed |
| 56–63 | 8 | Completed optimiser steps |
| 64–95 | 32 | SHA-256 of canonical encoded checkpoint |
| 96–127 | 32 | SHA-256 of bytes 0–95 in this positional sidecar |

The record is fully deterministic for identical model state and context policy.
The last digest detects corruption, including when there is no checkpoint file
available to inspect. It is **not an authentication mechanism**. A person able
to alter a sidecar could also recompute its unkeyed SHA-256. Verify model
provenance and file authenticity through the existing Umicom trusted publisher
and signature services before trusting external data.

## 4. Worked example: two policies, one checkpoint

Suppose the model has an eight-token context and is given three prompt tokens
representing `int`. The traditional generator first evaluates:

`BOS BOS BOS BOS BOS  i  n  t`

The growing-prefix session evaluates:

`i  n  t`

The model parameters are identical, so their checkpoint hashes must match;
however, the token-position assignments differ. It is **incorrect** to feed a
BOS-left-padded policy declaration to the growing-prefix session, even if the
model checksum matches. The admission check rejects that mismatch before a
benchmark or generation request.

The usage pattern is:

```c
#include "umicom/ai/training/position_contract.h"

UmiAiTrainingPositionContract approved = {0};
UmiStatus status = UmiAiTrainingPositionContractBuild(model,
    UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED, &approved);
if (status == UMI_STATUS_OK) {
    status = UmiAiTrainingPositionContractVerify(model, &approved,
        UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED);
}
```

To persist it independently:

```c
uint8_t record[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
size_t written = 0U;
UmiStatus status = UmiAiTrainingPositionContractEncode(
    &approved, record, sizeof(record), &written);
/* Save the 128 bytes only through a trusted, approved storage boundary. */
```

The Framework has no hidden file write here. The caller decides which path,
application and publisher may store or load the sidecar. Decoding an approved
sidecar never decodes a checkpoint or executes generated source.

## 5. Error and security contracts

| Condition | Result | Caller output |
|---|---|---|
| Null input, invalid mode | `UMI_STATUS_INVALID_ARGUMENT` | Unchanged |
| Insufficient destination capacity | `UMI_STATUS_CAPACITY_EXCEEDED` | Unchanged |
| Wrong record length, digest, magic or unsupported encoding | `UMI_STATUS_PARSE_ERROR` from Decode | Unchanged |
| Mode differs from declaration or live checkpoint differs | `UMI_STATUS_INVALID_STATE` from Verify | No output |
| Allocation failure | `UMI_STATUS_OUT_OF_MEMORY` | Unchanged |
| Interrupted/failed underlying Framework operation | Underlying `UmiStatus` | Unchanged |

A valid metadata digest **does not** imply the checkpoint is trusted, nor does
it prevent an attacker from forging both files. Before loading a third-party
checkpoint, enforce a trusted source, a size limit, the current signed-release
approval policy and the decoder's own checkpoint validity requirements.

## 6. Integration and regression coverage

The module is additive and included after the existing native training,
prefix reuse and benchmark modules in the isolated `prefix-work` laboratory.
It does **not** rewrite the Framework's global CMakeLists. It does not modify
`decoder.c`, `kv_cache.c`, `generation.c`, `prefix_generation.c`, sampling,
trainer, training tape, checkpoints or any existing model weights.

Focused tests:

- `position_contract_roundtrip`: real decoder checkpoint identity, exact
  serialization, two policies and independent checkpoint decoding.
- `position_contract_rejections`: corrupted records, rehashed unsupported
  strategies, invalid context and output preservation.
- `position_contract_benchmark`: preflight then the existing paired benchmark,
  followed by the same hash-bound verification.
- `position_contract_demo`: a learner-facing native C23 end-to-end example.

These checks must run against your **real Framework decoder** on Windows
UCRT64 before treating this laboratory as integrated. A separate isolated
contract test may validate the new code with API-compatible substitutes; that
alone is not real-decoder numerical parity or a production release.

## 7. Next steps

The next meaningful optimisation is a separately identified positional model
family, not an in-place interpretation of learned weights. A proposed rotary
implementation should first introduce an explicit model configuration and
checkpoint family, then independent forward-logit and gradient references,
training/evaluation regressions, rolling-window parity, checkpoint-format
roundtrips, changed-shape refusal, and measured cache re-use. This sidecar
would then need a reviewed format revision or a new compatible schema.

**Key engineering rule:** A model's trained positional mathematics, its
generation context convention and its on-disk identity are three related but
distinct contracts. Never allow an optimisation to silently rewrite them.
