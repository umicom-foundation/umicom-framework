# Retain a reviewed Kanban decision

This C23 example uses installed public headers and libraries. It creates a synthetic
board in memory, moves one example card to Testing, records a review, writes a new
private review file, reloads it and explicitly completes the card. It does not open
your development board, execute product tests or publish a roadmap.

The example requires an installed SDK containing kanban_storage and its dependencies.
Use an existing SDK prefix and a private evidence directory. Keep that evidence outside
product repositories.

1. Configure this directory with CMake. Set UMICOM_SDK_PREFIX to the absolute installed
   Framework prefix, for example C:/umicom/installed/applications.
2. Build the kanban-completion-consumer target. A missing component or unresolved symbol
   is an SDK installation problem; do not add source-tree include paths to hide it.
3. Create a private directory, then run the consumer with a new absolute destination:

~~~powershell
kanban-completion-consumer.exe normal C:/umicom/_local/reviews/example.umicompletion
~~~

The expected message starts with "Synthetic card completed". The file contains
synthetic private attribution and evidence metadata. Existing files are refused.
Choose a new name for another run; do not overwrite evidence to hide a failure.

Run the same executable with blocked or stale instead of normal to see refused
completion. A blocked example creates no review file. A stale example retains the
historical review, but its card remains Testing because the criteria changed.

CTest registers five consumer cases. It creates isolated directories, retains logs,
checks that rejected completion does not create a file, and compares the original
digest after a refused overwrite. Configure, build and run these cases against the
installed SDK to check the installed dependency closure.

In a real host, the reviewer supplies source/configuration identity, the SHA-256 of
retained acceptance evidence and a rationale. This example's fixed synthetic text is
not qualification evidence. The core service neither authenticates the reviewer nor
verifies the evidence. Board saving and review retention remain separate operations.
