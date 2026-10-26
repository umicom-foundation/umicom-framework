# Native multi-layer K/V storage laboratory

**Umicom Framework · Sammy Hegab · Umicom Foundation · MIT**

This optional learning target builds the native C23 K/V **row-storage owner** for the existing bounded stacked-decoder architecture. It composes the original Framework model-scaling and conditional prefix-work planners. It does **not** implement attention, produce logits, sample tokens, decode checkpoint bytes or accelerate generation.

The owner accepts caller-supplied already-computed, finite key/value rows in documented layer-major shape. A short prefix can grow without moving earlier token positions; once full, the owner refuses simple appends and accepts only a complete freshly calculated shifted-window replacement. Full replacement is necessary with the existing learned absolute positions and potentially history-dependent upper-layer states. A checkpoint identity label can be checked, but the caller must independently verify the actual model with the existing Framework identity service.

Build with native C23 using the established toolchain:

```text
cmake -S framework/learning/native-ai-stacked-kv -B build/native-ai-stacked-kv -G Ninja -DBUILD_TESTING=ON
cmake --build build/native-ai-stacked-kv --parallel 2
ctest --test-dir build/native-ai-stacked-kv --output-on-failure --no-tests=error
```

The test suite verifies ordinary prefill/append, full-window replacement, layer separation, wrong-shift and nonfinite-value rejection, read capacities, identity mismatch, real row-counter agreement with the existing planner and 500 consecutive safe rollovers. The example demonstration uses **synthetic K/V rows** only.

Before this cache owner could back an accelerated Umicom native decoder, a producer must use the authoritative model's trained projections, a future attention consumer must match the original decoder's logits under the same positional/context policies, and end-to-end timings must be measured. A successful store test does not satisfy those additional gates.
