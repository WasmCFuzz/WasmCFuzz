# Ideas for AFL++

In the following, we describe a variety of ideas that could be implemented for
future AFL++ versions.

**NOTE:** Our GSoC participation is concerning [libafl](https://github.com/AFLplusplus/libafl), not AFL++.

## Analysis software

Currently analysis is done by using afl-plot, which is rather outdated. A GTK or
browser tool to create run-time analysis based on fuzzer_stats, queue/id*
information and plot_data that allows for zooming in and out, changing min/max
display values etc. and doing that for a single run, different runs and
campaigns vs. campaigns. Interesting values are execs, and execs/s, edges
discovered (total, when each edge was discovered and which other fuzzer share
finding that edge), test cases executed. It should be clickable which value is X
and Y axis, zoom factor, log scaling on-off, etc.

Mentor: vanhauser-thc

## Support other programming languages

Other programming languages also use llvm hence they could be (easily?)
supported for fuzzing, e.g., mono, swift, go, kotlin native, fortran, ...

GCC also supports: Objective-C, Fortran, Ada, Go, and D (according to
[Gcc homepage](https://gcc.gnu.org/))

LLVM is also used by: Rust, LLGo (Go), kaleidoscope (Haskell), flang (Fortran),
emscripten (JavaScript, WASM), ilwasm (CIL (C#)) (according to
[LLVM frontends](https://gist.github.com/axic/62d66fb9d8bccca6cc48fa9841db9241))

Mentor: vanhauser-thc

## Machine Learning

Something with machine learning, better than
[NEUZZ](https://github.com/dongdongshe/neuzz) :-) Either improve a single
mutator through learning of many different bugs (a bug class) or gather deep
insights about a single target beforehand (CFG, DFG, VFG, ...?) and improve
performance for a single target.

Mentor: domenukk

## Your idea!

Finally, we are open to proposals! Create an issue at
https://github.com/AFLplusplus/AFLplusplus/issues and let's discuss :-)
