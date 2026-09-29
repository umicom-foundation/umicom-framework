# Notes background-worker lesson

Run `umicom-thread-lifecycle --self-test` to index a fixed Notes sentence, or
`umicom-thread-lifecycle cancel` to demonstrate a queued cooperative cancellation.
No files, GUI, database or network are used. The controller joins before its stack
context leaves scope. The complete learning guide is in
`../../docs/learning/background-work-and-memory.html`.

This directory is also a focused CMake SDK host. Its platform library is compiled
from the real `src/platform/threading.c`. Do not install the focused SDK over a full
Framework SDK; choose a separate installation prefix.
