#define NAPI_VERSION 3
#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef int (*stage2_main)(napi_env env, int argc, char ** argv);

static napi_value stage2(napi_env env, napi_callback_info info) {
  static char property_name_buffer[512];
  stage2_main cb;
  napi_value argv[2];
  size_t argc = 2;
  size_t nb_args = 0;
  printf("Stage2 addon called\n");
  napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  napi_get_value_int64(env, argv[0], (int64_t *)&cb);
  printf("Retrieved callback\n");
  napi_get_array_length(env, argv[1], (uint32_t *)&nb_args);
  char * new_args[nb_args];
  napi_value arg;
  size_t arg_length;
  size_t ignore;
  printf("Retrieving %d args\n", nb_args);
  for (size_t i = 0; i < nb_args; i++) {
    sprintf(property_name_buffer, "%lu", i);
    napi_get_named_property(env, argv[1], property_name_buffer, &arg);
    napi_get_value_string_utf8(env, arg, NULL, 0, &arg_length);
    new_args[i] = malloc(arg_length + 2);
    napi_get_value_string_utf8(env, arg, new_args[i], arg_length + 1, &ignore);
    new_args[i][arg_length + 1] = '\0';
  }
  printf("Calling C++\n");
  cb(env, nb_args, new_args);
  for (size_t i = 0; i < nb_args; i++) {
    free(new_args[i]);
  }
  return 0;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_value wrapper;
  napi_create_function(env, "stage2", NAPI_AUTO_LENGTH, stage2, NULL, &wrapper);
  napi_set_named_property(env, exports, "stage2", wrapper);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
