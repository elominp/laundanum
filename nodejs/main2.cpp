#define NAPI_VERSION 3
#include <node_api.h>
#include <node.h>
#include <uv.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PAGE_SIZE       4096
#define STACK_SIZE      16 * PAGE_SIZE // 64 KB stack is enough
#define PATH_LENGTH     512
#define LINE_MAX_LENGTH 1024

#ifdef __i386__
# define SP "ESP"
# define BP "EBP"
#elif defined(__x86_64__)
# define SP "RSP"
# define BP "RBP"
#else
# error "CPU architecture not supported"
#endif

#define BOOTSTRAPJS "./bootstrap2.js"

#define NOT_INITIALIZED 0
#define PAUSED 1
#define CONTINUE 2
#define RUN_SCRIPT 3
#define EVAL_SCRIPT 4
#define EXIT 5

#define YIELD stage3_status = PAUSED; if (setjmp(stage3) != CONTINUE) { longjmp(stage2, PAUSED); }
#define SCRIPT stage3_status = RUN_SCRIPT; if (setjmp(stage3) != CONTINUE) { longjmp(stage2, RUN_SCRIPT); }
#define EVAL stage3_status = EVAL_SCRIPT; if (setjmp(stage3) != CONTINUE) { longjmp(stage2, EVAL_SCRIPT); }

static uint8_t *    snapshot_stack = NULL;
static uint8_t *    cur_sp = NULL;
static uint8_t *    cur_bp = NULL;
static void *       orig_sp = NULL;
static void *       orig_bp = NULL;
static int stage3_status = NOT_INITIALIZED;
static jmp_buf stage2;
static jmp_buf stage3;

static napi_env script_env;
static napi_value script[2];
static const char * c_script;
static napi_value script_this;
static napi_value script_eval;
static napi_value script_require;
static napi_value * script_result;
static napi_status script_status;

typedef struct memmap_s {
    void *  bottom;
    void *  top;
    char    rights[5];
    size_t  offset;
    uint8_t device_major;
    uint8_t device_minor;
    size_t  inode;
    char *  pathname; // pointer on buffer to fill
} memmap_t;

static int init_altstack(void) {
    int fd = open("/dev/zero", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open /dev/zero, unable to get a new fd\n");
        return -1;
    }
    // fd = -1 to avoid opening /dev/zero and STACK_GROWSDOWN doesn't work on my linux kernel :'(
    if ((snapshot_stack = (uint8_t *)mmap(NULL, STACK_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        close(fd);
        fprintf(stderr, "Failed to allocate memory for the new stack, errno = %s\n", strerror(errno));
        return -1;
    }
    close(fd);
    snapshot_stack += STACK_SIZE;
    printf("Allocated new stack memory at %p\n", snapshot_stack);
    return 0;
}

static napi_status run_script(napi_env env, const char * script) {
  script_env = env;
  c_script = script;
  SCRIPT;
  return script_status;
}

static napi_status eval_script(napi_env env, napi_value js_this, napi_value eval, napi_value require, const char * script) {
  script_env = env;
  c_script = script;
  script_this = js_this;
  script_eval = eval;
  script_require = require;
  EVAL;
  return script_status;
}

static const char * long_script = "\"use strict\";\n"
"console.log('Beginning request script');\n"
"var https_1 = require(\"https\");\n"
"var agent_options = {\n"
"    enableTrace: false,\n"
"    keepAlive: true,\n"
"    keepAliveMsecs: 5000,\n"
"    timeout: 10000,\n"
"    minVersion: 'TLSv1.2',\n"
"};\n"
"var agent = new https_1.Agent(agent_options);\n"
"var my_request = https_1.request('https://www.google.fr/', {\n"
"    agent: agent,\n"
"    timeout: 10000\n"
"}, function (response) {\n"
"    console.log(\"HTTP status code: \" + response.statusCode); // Get HTTP status code of the response\n"
"    console.log(\"HTTP \" + response.statusMessage); // Get HTTP status message of the response\n"
"});\n"
"my_request.on('error', (e) => { console.error(`request failed: ${e}`); });"
"my_request.end(); // Send the request and return immediately, when the server answer, the callback will be called\n"
"console.log('Request sent :)');\n";

static int main_stage_3(napi_env env, napi_value js_this, napi_value eval, napi_value require, int argc, char **argv) {
  static const char * inline_script = "console.log(\"Now, let's do some fun :p\");";
  std::cout << "Hello stage 2!" << std::endl;
  std::cout << "Received " << argc << " args" << std::endl;
  for (size_t i = 0; i < argc; i++) {
    std::cout << argv[i] << std::endl;
  }
  YIELD
  napi_value script;
  napi_value result;
  run_script(env, inline_script);
  YIELD
  run_script(env, inline_script);
  YIELD
  eval_script(env, js_this, eval, require, long_script);
  std::cout << "Called script requesting Google" << std::endl;
  YIELD
  std::cout << "Blocking main thread for 2s" << std::endl;
  sleep(2);
  YIELD
  sleep(2);
  YIELD
  run_script(env, inline_script);
  YIELD
  sleep(2);
  YIELD
  exit(0);
  //stage3_status = EXIT;
}

static int main_stage_2(napi_env env, napi_value js_this, napi_value eval, napi_value require, int argc, char **argv) {
  static napi_env bkp_env;
  static napi_value bkp_this;
  static napi_value bkp_eval;
  static napi_value bkp_require;
  static int bkp_argc;
  static char **bkp_argv;
  static int sig;
  sig = 0;
  if (stage3_status != EXIT) {
    // Backup original stack pointers
    //if (stage3_status == NOT_INITIALIZED) {
      bkp_env = env;
      bkp_this = js_this;
      bkp_eval = eval;
      bkp_require = require;
      bkp_argc = argc;
      bkp_argv = argv;
    //}
    asm volatile ("\tmov %%"SP", %0\n" : "=r"(orig_sp) ::);
    asm volatile ("\tmov %%"BP", %0\n" : "=r"(orig_bp) ::);
    if (stage3_status == NOT_INITIALIZED) {
      asm volatile ("\tmov %0, %%"SP"\n" :: "r"(snapshot_stack) :);
      asm volatile ("\tmov %0, %%"BP"\n" :: "r"(snapshot_stack) :);
      std::cout << "First call" << std::endl;
      if ((sig = setjmp(stage2)) == 0) {
        stage3_status = main_stage_3(bkp_env, bkp_this, bkp_eval, bkp_require, bkp_argc, bkp_argv);
      }
      // Go back to previous stack
      asm volatile ("\tmov %0, %%"SP"\n" :: "r"(orig_sp) :);
      asm volatile ("\tmov %0, %%"BP"\n" :: "r"(orig_bp) :);
    } else if (stage3_status != EXIT) {
      std::cout << "Resuming C++ main thread" << std::endl;
      longjmp(stage3, CONTINUE);
    }
    if (sig == RUN_SCRIPT) {
        std::cout << "Run js script on main thread" << std::endl;
        napi_value result;
        script_status = napi_create_string_utf8(bkp_env, c_script, NAPI_AUTO_LENGTH, &script[0]);
        script_status = napi_run_script(bkp_env, script[0], &result);
        longjmp(stage3, CONTINUE);
    } else if (sig == EVAL_SCRIPT) {
      std::cout << "Eval script on main thread" << std::endl;
      napi_value result;
      script_status = napi_create_string_utf8(bkp_env, c_script, NAPI_AUTO_LENGTH, &script[0]);
      script[1] = bkp_require;
      script_status = napi_call_function(bkp_env, bkp_this, bkp_eval, 2, script, &result);
      longjmp(stage3, CONTINUE);
    }
  } else {
    std::cout << "exiting" << std::endl;
  }
  return 0;
}

int main(int argc, char **argv) {
  static char stage2address[64];
  init_altstack();
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
