# Native Model Optimisers and Reproducible Learning-Rate Schedules

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT  
**Canonical implementation:** Umicom Framework C23 source, under `src/ai/training/`

## Purpose

Training updates model parameters so that predictions better match target tokens. An optimiser transforms the gradient produced by backpropagation into a parameter update. A learning-rate schedule determines how much of that update is applied at each absolute optimiser step.

The established SGD, microbatch accumulation, dataset sampler and model checkpoint remain authoritative and unchanged. The new `optimizer.h`/`optimizer.c` component introduces a stateful **AdamW** rule and stateless **constant, linear-decay and cosine-decay learning-rate schedules**. The decoder exposes an additive AdamW training path that shares the existing forward, loss and backward graph. Scheduled source-code experiments reuse the original SGD and microbatch algorithms.

## How AdamW works

Given a gradient `g`, first and second moments `m` and `v`, decay coefficients `beta1`, `beta2`, and completed update index `t`:

```text
m = beta1 × m_previous + (1 − beta1) × g
v = beta2 × v_previous + (1 − beta2) × g²

m_corrected = m / (1 − beta1^t)
v_corrected = v / (1 − beta2^t)

w_new = w_old − learning_rate ×
        [m_corrected / (sqrt(v_corrected) + epsilon)
         + weight_decay × w_old]
```

The decoupled weight-decay term is intentionally outside the adaptive gradient calculation. The Framework performs all calculations using checked floating-point intermediates. Updates are applied using `UmiAiTrainingSgdStepWithGradients`, the existing tape-owned **transactional parameter writer**. Despite its name, that primitive safely applies any explicitly supplied finite effective gradient; it does not calculate the optimiser rule.

The optimizer stages all moment updates separately. If any input, candidate moment, effective gradient or parameter update is rejected, **weights and AdamW moment state are not published**. The public object borrows the decoder tape and parameter handles; destroy AdamW before destroying its decoder. State is not thread-safe.

### What remains deliberately limited

The native AdamW adapter currently performs **one directly bounded decoder batch per optimizer step**. Existing effective-batch accumulation and existing experiment-run/resume functions continue to use SGD. AdamW's moment state is process-local: the portable decoder checkpoint serialises *model parameters and completed steps*, **not** AdamW first and second moments. Consequently, reinitialising AdamW from a nonzero-step model is explicitly rejected by `UmiAiTrainingDecoderAdamWCreate`. Exact AdamW continuation across a process restart requires a future, separately versioned optimiser-state checkpoint and explicit plan/dataset provenance checks. Do **not** call an SGD checkpoint an AdamW resume snapshot.

The simple implementation applies weight decay to all trainable decoder parameters, including biases and layer-normalisation parameters. Per-parameter decay groups and independent optimizer-state persistence are separate engineering tasks.

## Learning-rate scheduling

`UmiAiTrainingLearningRateForStep` accepts a reviewed schedule and a **one-based absolute optimiser step**. Warmup gradually raises the rate towards the configured peak. Constant schedules hold that rate afterwards. Linear schedules decay to a chosen minimum. Cosine schedules use a smooth half-cosine curve, also ending at the minimum.

The planned total step horizon is part of the experiment definition. An interrupted run at step 8 of a 32-step schedule must be resumed with the **original 32-step schedule**, not a freshly defined 24-step schedule: changing the horizon changes the learning rates and therefore the final parameter bits.

The API rejects invalid rates, non-finite values, an invalid horizon and requests outside the configured interval without modifying caller-owned output values. Rates must be strictly positive.

## Native laboratory usage (Windows PowerShell)

```powershell
Set-Location "C:\umicom\Umicom-Applications"
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"

cmake -S ".\framework\learning\native-llm" `
    -B ".\build\windows-ucrt64-native-llm-debug" `
    -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ".\build\windows-ucrt64-native-llm-debug" --parallel 2
ctest --test-dir ".\build\windows-ucrt64-native-llm-debug" `
    --output-on-failure --no-tests=error

# Train a small native source-code model with AdamW from random weights.
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-adamw.exe" `
    ".\framework\learning\native-llm\fixtures\manifest.txt"

# Run an existing reviewed SGD experiment using a 32-step cosine schedule.
& ".\build\windows-ucrt64-native-llm-debug\umicom-native-llm-scheduled.exe" `
    ".\framework\learning\native-llm\fixtures\experiment.conf" `
    32 4 0.0025
```

The scheduled command accepts `reviewed-plan`, `planned-total-steps`, `warmup-steps`, `minimum-rate`, then optional `new-checkpoint` and `existing-checkpoint-to-resume` file paths. The original plan's `learning_rate` supplies the peak. It selects plain SGD or accumulated SGD from the existing plan. **A checkpoint is only saved when a new destination is explicitly supplied**, and the existing Framework checkpoint writer refuses to overwrite an existing file.

## Validation and proof obligations

The native regression tests check bias-corrected updates against independently calculated expectations, moment-state rollback on rejected gradients, seeded decoder-to-decoder reproducibility, rejection of an optimiser attached to a different decoder, absolute learning-rate endpoints, and byte-identical uninterrupted versus resumed **scheduled SGD** checkpoints for both direct and accumulated training.

A decreasing training loss is evidence of optimisation on a small dataset, **not** proof of source-code competence. Evaluation must retain independent source files, reproducible settings and an accurate description of what the checkpoint contains.
