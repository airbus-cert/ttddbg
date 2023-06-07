# 1.2.O

* Improve the "full run" functionality
  * Speed up the execution by stepping through 100,000 states at a time
  * Go through each thread state
* Add a "go to position" button to the toolbar (shortcut Ctrl+Shift+G)

# 1.1.0
* Add function tracing
  * Integrates with TTD's "CallRet callback" feature, allowing us to trace every `call` and `ret` instruction
  * Uses the IDA type information to pretty-print the arguments and return values
  * Can be used to quickly jump to a time position, at the time of the `call` or `ret` instruction, as well as copy the argument's address
* Add a "full run" feature, simulating a full program execution from start to finish. Useful in conjunction with function tracing.
* New bug: an issue appeared when using IDA 8, see [the workaround](https://github.com/airbus-cert/ttddbg#known-issues)