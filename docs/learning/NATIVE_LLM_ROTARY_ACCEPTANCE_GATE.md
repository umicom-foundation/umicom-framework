# Umicom Native LLM — Batch 93–94: acceptance evidence and next-model decision

**Owner:** Umicom Framework · **Author:** Sammy Hegab / Umicom Foundation · **Language:** C23 · **Licence:** MIT

## Why a separate evidence gate?

Batch 91–92 introduced regression executables linked against the real `Umicom::AiTraining` target. The final production pass/fail evidence must come from actually running those executables on the user's environment. Static analysis, mock substitutions and CMake registration are NOT substitutes for that run.

The new `rotary_acceptance.h` contract accepts one observation for each required regression: `rotary_real_tape_gradients`, `rotary_real_tape_budget`, and `rotary_real_tape_rewind`. It differentiates **not executed**, **failed**, and **passed**, and refuses duplicate/missing IDs and non-finite gradient errors. It does not run tests, sign results, parse log files, or establish provenance. The host should obtain test observations from the normal Framework/CTest test service after each actual execution.

This first release qualifies evidence interpretation using **synthetic test fixtures**. A fixture-only PASS does **not** mean a full production-tape PASS. Manual acceptance must inspect the real CTest output and compare with its source commit, build preset, compiler, test programme and machine.

## Acceptance criteria for native rotary integration

1. Compile all `rotary_real_tape_*` targets against the actual Framework `tape.c`, `rotary_tape.c` and `rotary_optimisation.c`, with strict warnings.
2. Run the numerical-oracle test and investigate gradient discrepancies; the previous laboratory's 0.0008 absolute threshold is experiment-specific.
3. Repeat under Release and ASan/UBSan where supported; failures must not be hidden behind the evidence evaluator.
4. Verify post-failure output remains unchanged; inspect current tape bytes and tensor counts following rollback.
5. Run 48 graph-rewind cycles and compare preserved leaves, current bytes and persistent peak high-water mark.
6. Record full CTest XML or console output externally with commit SHA, toolchain and test command.
7. Only then consider optimising the tape operation; retain original source under the established preservation policy.

## Architecture of a future fused operation (NOT yet implemented)

A genuinely fused training operator would need to own Q/K rotary forward transformations and their adjoints in the Framework's tape. Its backward pass must account for all incoming Q/K/V dependencies and appropriately accumulate repeated/aliased gradients, including multi-head partitioning, sequence boundaries, causal masks, and arbitrary valid shapes. It must retain required intermediates under the tape's memory budget and support rollback without leaking nodes or changing public handles.

Comparative performance must be evaluated **after** gradient parity and failure atomicity. The present release does not claim improved speed, reduced peak memory, a trained RoPE model, or reuse across multi-layer rollover.
