#define NAPI_VERSION 3
#include <node_api.h>
#include <node.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define BOOTSTRAPJS "./bootstrap.js"

static int main_stage_2(napi_env env, int argc, char **argv) {
  std::cout << "Hello stage 2!" << std::endl;
  std::cout << "Received " << argc << " args" << std::endl;
  for (size_t i = 0; i < argc; i++) {
    std::cout << argv[i] << std::endl;
  }
  napi_value script;
  napi_value script_result;
  napi_create_string_utf8(env, "console.log(\"Now, let's do some fun :p\");", NAPI_AUTO_LENGTH, &script);
  napi_run_script(env, script, &script_result);
  return 0;
}

int main(int argc, char **argv) {
  static char stage2address[64];
  size_t args_length = strlen(argv[0]) + strlen(BOOTSTRAPJS) + 2;
  std::cout << "Writing stage2 address" << std::endl;
  sprintf(stage2address, "%x", &main_stage_2);
  std::cout << "Wrote " << stage2address << std::endl;
  args_length += strlen(stage2address) + 1;
  for (size_t i = 1; i < argc; i++) {
    args_length += strlen(argv[i]) + 1;
  }
  int new_argc = argc + 2;
  char new_flat_argv[args_length];
  char * new_argv[new_argc];
  char * ptr = new_flat_argv;
  new_argv[0] = ptr;
  strcpy(ptr, argv[0]);
  ptr += strlen(ptr) + 1;
  new_argv[1] = ptr;
  strcpy(ptr, BOOTSTRAPJS);
  ptr += strlen(ptr) + 1;
  new_argv[2] = ptr;
  strcpy(ptr, stage2address);
  ptr += strlen(ptr) + 1;
  for (size_t i = 3; i < new_argc; i++) {
    new_argv[i] = ptr;
    strcpy(ptr, argv[i - 2]);
    ptr += strlen(ptr) + 1;
  }
  for (size_t i = 0; i < new_argc; i++) {
    std::cout << "Wrote arg[" << i << "] = " << new_argv[i] << std::endl;
  }
  std::cout << "Jumping to node!" << std::endl;
  return node::Start(new_argc, new_argv);
}
