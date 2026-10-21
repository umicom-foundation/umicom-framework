# Exact AdamW Recovery with a Combined Native Checkpoint

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT  
**Implementation:** C23 · **Owner:** Umicom Framework

## Why an ordinary decoder checkpoint is insufficient

The existing decoder checkpoint stores model parameters and the completed
optimiser-step count. It is suitable for continuing plain SGD experiments with
a consistent training plan. AdamW also needs two arrays for **every scalar
parameter**: its first moment, `m`, and its second moment, `v`.

For a model with `P` scalar parameters, this laboratory's AdamW retains two
binary64 arrays of `P` entries each, adding `16 × P` payload bytes. Recreating
the optimiser with zeroed moments at step four would not reproduce the next
update. The framework therefore refuses creating a *fresh* AdamW optimiser on
a model with previous training steps.

A **combined checkpoint** now encloses the unchanged `UMIDEC04` model
checkpoint and the AdamW configuration, per-parameter shapes, step count, and
both moment arrays. The new envelope uses distinct `UMIADW01` magic bytes;
existing model-only checkpoint readers will not mistake it for an ordinary
model checkpoint, and the original `UMIDEC04` format is not altered.

## Native ownership

The decoder owns its trainable parameters and tape. The AdamW optimiser borrows
those handles and owns only moment state. A restored pair therefore must be
released in this order:

1. `UmiAiTrainingAdamWDestroy(optimizer)`;
2. `UmiAiTrainingDecoderDestroy(decoder)`.

`UmiAiTrainingAdamWCheckpointDecode` returns *both* owned handles only after
validation of the complete envelope. Truncated files, invalid metadata,
non-finite moments, mismatched parameter shapes, stale optimiser step counts,
and damaged checksums are rejected without modifying caller output pointers.

## Mathematical continuity

For gradient `g_t` at optimiser step `t`, with AdamW beta coefficients:

```text
m_t = beta1 * m_(t-1) + (1 - beta1) * g_t
v_t = beta2 * v_(t-1) + (1 - beta2) * g_t^2
m_hat = m_t / (1 - beta1^t)
v_hat = v_t / (1 - beta2^t)
weight_t = weight_(t-1) - rate_t * [m_hat / (sqrt(v_hat) + epsilon) + decay * weight_(t-1)]
```

Restoring exactly the same weights, `m`, `v`, `t`, hyperparameters, next
training examples, gradient accumulation, and step-specific learning rate is
necessary to reproduce an uninterrupted run. A model alone is not enough.

This package's regression test trains a model for four steps, serialises the
combined checkpoint, restores independent model/optimiser handles, and trains
both original and restored instances through four further identical steps.
It then compares the complete encoded state **byte for byte**.

## File format and integrity boundaries

The binary format is explicitly length-delimited, little-endian and portable
across platforms supporting IEEE754 binary32 and binary64. It consists of:

```text
8 bytes   UMIADW01 magic
4 bytes   format number
4 bytes   embedded UMIDEC04 model-checkpoint length
4 bytes   parameter tensor count
4 bytes   total scalar parameter count
8 bytes   completed AdamW steps
16 bytes  four binary32 hyperparameters
4 x N     element counts for N trainable tensors
variable  complete original decoder checkpoint (including its own checksum)
16 x P    each scalar's binary64 first moment + second moment
8 bytes   envelope FNV-1a64 checksum
```

This format has a strict eight-megabyte input limit. File saving exclusively
creates a staging file and publishes a **new** destination, never replacing an
existing checkpoint. A stale staging file from an interrupted process should
be reviewed manually and never blindly removed by automated cleanup.

**Security:** FNV-1a64 detects accidental changes but is not cryptographic.
It cannot prove that a checkpoint originated from Umicom or was not maliciously
modified. Only read locally reviewed checkpoints with verified provenance.
The data format is not executable and the loader does not execute file content.

## How to build and run

From PowerShell at `C:\umicom\Umicom-Applications`:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"
cmake -S ".\framework\learning\native-llm" `
  -B ".\build\windows-ucrt64-native-llm-debug" -G Ninja `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
  --output-on-failure -R "framework.ai.training.adamw_"
```

After security software has cleared the development binaries, create a *new*
local combined checkpoint deliberately:

```powershell
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-adamw-checkpoint.exe" `
  ".\framework\learning\native-llm\fixtures\manifest.txt" `
  ".\build\windows-ucrt64-native-llm-debug\reviewed-adamw-state.umi"
```

The demonstration leaves that new checkpoint in place for inspection; it does
not overwrite or automatically remove it. Future applications must verify
training-plan and dataset provenance before authorising another resumed run.

## What is NOT yet included

This checkpoint does not embed the reviewed source manifest, complete learning-
rate schedule, or a cryptographic publisher signature. The optimiser does not
change the experiment API's SGD-only schedule/resume behaviour. A later
Framework extension can integrate verified AdamW state into the experiment
planner while preserving this separate on-disk format and its existing tests.
