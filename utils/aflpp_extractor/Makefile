ifeq "" "$(LLVM_CONFIG)"
  LLVM_CONFIG=llvm-config
endif

ifeq "$(shell uname -s)" "Darwin"
  # On some odd MacOS system configurations, the Xcode sdk path is not set correctly
  SDK_LD = -L$(shell xcrun --show-sdk-path)/usr/lib
  LDFLAGS += $(SDK_LD)
endif

CC := clang
CFLAGS := -O3 -funroll-loops -g -fPIC

all:    libAFLExtractor.a  extractor_test_fuzzer reproducer

extractor.o: extractor.c
	-$(CC) -I. -I../../include $(CFLAGS) -c extractor.c

libAFLExtractor.a:      extractor.o
	@ar rc libAFLExtractor.a extractor.o
	@cp -vf libAFLExtractor.a ../../

extractor_test_fuzzer: extractor.o 
	../../afl-clang-fast -fsanitize=fuzzer -I. -o extractor_test_fuzzer  extractor_test.c

reproducer: extractor_test_fuzzer
	../../afl-clang-fast -I../../include -Wl -funroll-loops -o reproducer ./extractor_test_fuzzer ./libAFLExtractor.a reproducer.c

clean:
	rm -f *.o *.a *~ core reproducer extractor_test_fuzzer
