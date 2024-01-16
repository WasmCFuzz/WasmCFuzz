#include <iostream>
#include "random.h"
#include "wasm_structure.h"
#include "global_classes_list.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <memory>
using namespace std;

using namespace Wasm;

using namespace Sections;

#define INITIAL_SIZE (400)
#define MAX_FILE (0x1000)

typedef struct mutate_helper {
    uint8_t *buf;
    size_t len;
} mutate_helper_t;

static mutate_helper_t* afl_mutate_helper;

extern "C" {
void mutate_helper_init() {
    afl_mutate_helper = (mutate_helper_t *)calloc(1, sizeof(mutate_helper_t));
    afl_mutate_helper->len = INITIAL_SIZE;
    afl_mutate_helper->buf = (uint8_t *)calloc(1, afl_mutate_helper->len);
    if (!afl_mutate_helper->buf) perror("mutate_helper_init");
}

uint8_t* mutate_helper_buffer(){
    return afl_mutate_helper->buf;
}

size_t mutate_helper_buffer_size(){
    return afl_mutate_helper->len;
}

void mutate_helper_realloc_buf(size_t length) {

    if (afl_mutate_helper->len <= length) {
        // double length
        size_t old_length = afl_mutate_helper->len;
        afl_mutate_helper->len = length + 0x10;
        afl_mutate_helper->buf = (uint8_t *)realloc(afl_mutate_helper->buf, afl_mutate_helper->len);
        if (!afl_mutate_helper->buf) {
            perror("mutate_helper_realloc");
            return;
        }
    }

    memset(afl_mutate_helper->buf, 0, afl_mutate_helper->len);
}

size_t wasm_test(uint8_t *buf, size_t buf_size)
{
    Random::init_fd();
    initList();
    DataOutputStream out;
    //unique_ptr<WasmStructure> wasm = make_unique<WasmStructure>((void *)buf, buf_size);
    WasmStructure* wasm = new WasmStructure((void *)buf, buf_size);
    wasm->generate();
    wasm->getEncode(&out);
    
    vector<uint8_t> output;
    auto generatedSize = out.size();
    output.resize(generatedSize);
    memcpy(output.data(), out.buffer(), generatedSize);

    string js = "var wasm_code = new Uint8Array([";
    for(auto it:output) {
        js += to_string((int)(it));
        js += ',';
    }

    js.pop_back();
    js += "]);\n"
            "var wasm_module = new WebAssembly.Module(wasm_code);\n"
            "var wasm_instance = new WebAssembly.Instance(wasm_module);\n"
            "var f = wasm_instance.exports._start;\n"
            "f();\n\0";

    // // Ensure the buffer size
    if (js.length() >= MAX_FILE) {
      delete wasm;
      return 0;
    }
    mutate_helper_realloc_buf(js.length());
    memcpy(afl_mutate_helper->buf, (unsigned char*)js.c_str(), js.length());
    size_t mutated_size = js.length();
    delete wasm;
    return mutated_size;
}
void mutate_helper_free(){
    free(afl_mutate_helper->buf);
    afl_mutate_helper->buf = NULL;
    free(afl_mutate_helper);
    afl_mutate_helper = NULL;
}
}
#undef INITIAL_SIZE
#undef MAX_FILE