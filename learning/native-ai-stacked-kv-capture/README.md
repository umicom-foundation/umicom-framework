# Observe canonical Transformer projections in native C23

**Sammy Hegab · Umicom Foundation · MIT**

This native Framework laboratory demonstrates that the established stacked decoder's own key/value projections can be captured and copied into the existing layer-major K/V owner. The decoder's original attention, embedding, checkpoint, tape and optimiser implementations remain authoritative.

Run CMake against this directory, build, and execute CTest. The tests compare captured logits against the uninstrumented original decoder, check retained key/value parity as a prefix grows, require complete per-layer reconstruction after an absolute-position rollover, and reject stale decoder checkpoints.

These operations still calculate the **complete Transformer context** at every observed step. Accepted storage rows and actually evaluated token-layer rows are reported separately. This is a correctness-and-integration milestone and **does not yet provide or demonstrate accelerated incremental attention**. The test fixtures use synthetic token identifiers rather than private training data.
