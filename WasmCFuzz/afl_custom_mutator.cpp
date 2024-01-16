#include "afl-fuzz.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "random.h"
#include "wasm_structure.h"
#include "global_classes_list.h"
#include <unistd.h>
#include <fcntl.h>

using namespace Wasm;
using namespace Sections;
#define DATA_SIZE (10000)


typedef struct my_mutator {

  afl_state_t *afl;


  unsigned char* mutated_out;

} my_mutator_t;
extern "C" {
my_mutator_t *afl_custom_init(afl_state_t *afl, unsigned int seed);
size_t afl_custom_fuzz(my_mutator_t *data, uint8_t *buf, size_t buf_size,
u8 **out_buf, uint8_t *add_buf,
size_t add_buf_size, size_t max_size);
void afl_custom_deinit(my_mutator_t *data);
}

my_mutator_t *afl_custom_init(afl_state_t *afl, unsigned int seed) {
  srand(seed);
  Random::init_fd();
  initList();
  my_mutator_t *data = (my_mutator_t *)calloc(1, sizeof(my_mutator_t));
  if (!data) {
    perror("afl_custom_init alloc");
    return NULL;
  }
 
  if ((data->mutated_out = (u8 *)malloc(MAX_FILE)) == NULL) {
    perror("afl_custom_init malloc");
    free(data);  // We should free previously allocated memory before returning
    return NULL;
  }

  data->afl = afl;
  return data;
}

size_t afl_custom_fuzz(my_mutator_t *data, uint8_t *buf, size_t buf_size, u8 **out_buf, uint8_t *add_buf, size_t add_buf_size, size_t max_size) {

  DataOutputStream out;
  Wasm::WasmStructure *wasm = new WasmStructure((void *)buf, buf_size);

  wasm->generate();
  wasm->getEncode(&out);
 
  std::vector<uint8_t> output;
  auto generatedSize = out.size();
  output.resize(generatedSize);
  memcpy(output.data(), out.buffer(), generatedSize);

  std::string js = "var wasm_code = new Uint8Array([";
  for(auto it:output) {
    js += std::to_string((int)(it));
    js += ',';
  }

  js.pop_back();
  js += "]);\n"
        "var wasm_module = new WebAssembly.Module(wasm_code);\n"
        "var wasm_instance = new WebAssembly.Instance(wasm_module);\n"
        "var f = wasm_instance.exports.main;\n"
        "f();\n";

  // // Ensure the buffer size
  // if (js.length() >= MAX_FILE) {
  //   delete wasm;
  //   return 0;
  // }

  memcpy(data->mutated_out, (unsigned char*)js.c_str(), js.length());
  size_t mutated_size = js.length();

  //if (mutated_size > max_size) { mutated_size = max_size; }

  *out_buf = (u8*)data->mutated_out;
 
  return mutated_size;
}

void afl_custom_deinit(my_mutator_t *data) {
  free(data->mutated_out);
  free(data);
}