ifeq "" "$(LLVM_CONFIG)"
  LLVM_CONFIG=llvm-config-14
endif

ifeq "$(shell uname -s)" "Darwin"
  # On some odd MacOS system configurations, the Xcode sdk path is not set correctly
  SDK_LD = -L$(shell xcrun --show-sdk-path)/usr/lib
  LDFLAGS += $(SDK_LD)
endif

CC := clang
CFLAGS := -O3 -funroll-loops -g -fPIC

all:    v8_reproducer zxw

v8_reproducer:
	$(CC) -I../ -I../../include -Wl -funroll-loops -lstdc++ -o v8_reproducer /home/chromium/src/out/libfuzzer/v8_wasm_compile_fuzzer  ../reproducer.c
zxw:
	$(CC) -I../ -I../../../include -Wl -g -funroll-loops -lstdc++ -shared -o zxw /home/chromium/src/out/libfuzzer/v8_wasm_compile_fuzzer  ../wasm_generate.c
clean:
	rm -f *.o *.a *~ v8_reproducer zxw
