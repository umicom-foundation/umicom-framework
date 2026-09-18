# Umicom source reference

Use the [Framework Wiki](../../wiki/index.html) for worked tasks and explanations.
This index is generated from the selected source checkout. A function or component
appearing here is not by itself evidence that every adapter is complete.

Public application contracts are in `include/umicom`. Private implementation
sources are included to make locations and relationships inspectable, not to
encourage applications to depend on private definitions.

Start with `umi_build_parse_diagnostic_line` for compiler messages, the document
coordinator for working copies, and `UmiUiComponentSpec` for component descriptions.
Use the [source map](../../source-map.html) to locate a header family, and the
[application introductions](../../products.html) to follow a user task.

Generated API coverage must be reviewed together with the comments and tests.
A declaration is not a guarantee of thread safety, live-provider readiness or
successful native execution. Missing explanations need author review rather than
an inferred promise added by a generator.
