#!/usr/bin/env python3

import sys
import os

ROOT = "/home/"
AFL_ROOT = os.path.join(ROOT, "AFL-WASM")
this_is_chakra = False
this_is_v8 = True

def find_engine(cmdline):
    #global this_is_chakra
    if this_is_chakra :
        return "CH"
    if this_is_v8 :
        return "V8"
    for arg in cmdline:
        if "JavaScriptCore" in arg:
            return "JSC"
        if "chakracore" in arg :
            return "CH"
        if "third_party" in arg:
            return "V8"

    return None

def is_target_jsc(cmdline):
    SIGNATURES = ["b3", "dfg", "jit", "domjit", "ftl", "bytecode", "bytecompiler", "llint"]

    # JSC unified multiple sources into one and compile them together.
    # Therfore, currently, we see the unified source's contents
    # and check if it is JIT-related.
    for i in range(len(cmdline) - 1):
        if cmdline[i] != '-c':
            continue

        source_path = cmdline[i + 1]
        try:
            data = open(source_path, "r").read()
            return any([sig in data for sig in SIGNATURES])
        except ValueError:
            # It is possible that we cannot access the source file
            return False

    # If we compile a binary
    return True

def is_target_ch(cmdline) :
    for i in range(len(cmdline) - 1):
        if cmdline[i] != '-c':
            continue

        source_path = cmdline[i + 1]   
        if "Runtime/Base/ThreadContext.cpp" in source_path :
            return False
    return True


def is_target(cmdline):
    engine = find_engine(cmdline)

    if engine is None:
        return False
    elif engine == "JSC":
        return is_target_jsc(cmdline)
    elif engine == "CH" :
        return is_target_ch(cmdline)
    elif engine == "V8" :
        return True 
def remove(l, e):
    try: 
        l.remove(e)
    except ValueError:
        pass

def rewrite(cmdline):
    new_cmdline = []
    engine = find_engine(cmdline)
    flag = True
    if engine == "V8":
        #remove(cmdline, "-fcomplete-member-pointers")
        #remove(cmdline, "-fcrash-diagnostics-dir=../../tools/clang/crashreports")
        remove(cmdline, "-Werror")
        #remove(cmdline,"-Wunused-command-line-argument") 
        for arg in cmdline:
            #if "wasm-fuzzer-common.o" in arg:
            	#flag = True
            #if flag and "-fsanitize=fuzzer-no-link" in arg:
                #new_cmdline.append("-shared")
                #new_cmdline.append("/home/AFL-WASM/libAFLExtractor.a")
               # continue
            #if "libclang_rt.builtins.a" in arg:
                #continue
            #if "-Wl,--undefined-version" in arg:
                #continue
            new_cmdline.append(arg)
    print(new_cmdline)


    return new_cmdline

if __name__ == '__main__':
    new_cmdline = sys.argv[:]
    compiler = os.path.basename(sys.argv[0])

    if is_target(new_cmdline):
        if compiler == "clang":
            new_cmdline[0] = os.path.join("/usr/lib/llvm-14/bin/clang")
        else:
            assert(compiler == "clang++")
            new_cmdline[0] = os.path.join("/usr/lib/llvm-14/bin/clang++")
    else:
        new_cmdline[0] = compiler

    new_cmdline = rewrite(new_cmdline)
    with open(os.path.join(ROOT, "proxy.log"), "a") as f:
        f.write(' '.join(new_cmdline) + "\n")
        
    os.execlp(new_cmdline[0], *new_cmdline)
