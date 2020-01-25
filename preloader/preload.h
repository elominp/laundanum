#ifndef PRELOAD_H
#define PRELOAD_H

#include <stdbool.h>

/**
 * Features to be provided by the preloader:
 * - Archiving command line passed to preloader and full command line passed to bootstraped executable
 *   + Preloader actions are listed in the "PRELOADER_CMDLINE_MODE" environment variable separated by the ':' character
 *     - Destinations are listed in the "PRELOADER_CMDLINE_OUTPUT" environment variable separated by the ':' character
 *   + Child process actions are listed in the "CHILD_CMDLINE_MODE" environment variable separated by the ':' character
 *     - Destinations are listed in the "CHILD_CMDLINE_OUTPUT" environment variable separated by the ':' character
 *   + Featured modes are:
 *     - "DEFAULT" corresponding to the "LOGFILE" mode
 *     - "FD" (same as standard streams)
 *     - "PIPE" (same as standard streams)
 * - Archiving environment passed to preloader and full command line passed to bootstraped executable
 *   + Preloader actions are listed in the "PRELOADER_ENVIRONMENT_MODE" environment variable separated by the ':' character
 *     - Destinations are listed in the "PRELOADER_ENVIRONMENT_OUTPUT" environment variable separated by the ':' character
 *   + Child process actions are listed in the "CHILD_ENVIRONMENT_MODE" environment variable separated by the ':' character
 *     - Destinations are listed in the "CHILD_ENVIRONMENT_OUTPUT" environment variable separated by the ':' character
 *   + Featured modes are the same as the the command line archiving action
 * - Archiving of standard input/outputs streams
 *   + Default mode "DEFAULT" is equal to "LOGFILE"
 *   + Multiple modes are possible by separating them by ':' character, the corresponding "OUTPUT" environnement variable will need to have the same number of arguments
 *   + By calling the standard stream to the caller of the preloader (mode "CLOSE")
 *   + By piping into log files (mode "LOGFILE")
 *   + By piping into a fd opened in the preloader (mode "FD")
 *   + By piping into a child process (mode "PIPE")
 *   + Eventually providing compression (*_COMPRESSION environment variable set)
 * - Archiving core dumps provinding the same features as standard streams
 *   + Provide additional features to be listed in the COREDUMP_ACTIONS environment variable being separated by the ':' character
 *   + Dumping a backtrace of the failed process (action "BT")
 *   + Dumping registers states at the crash (action "REGS")
 *   + Dumping disassembled function where the dump occured (action "DISASM")
 *   + Copy crashed executable (action "COPY")
 * - Preloading custom shared library before starting the bootstraped executable using the "CHILD_LD_PRELOAD" environment variable to transmit as part of the "LD_PRELOAD" environment variable of the bootstraped executable
 * - Intercepting standard libraries symbols providing several different modes (listed in the "FUNCTION_TRACE_MODE" environment variable)
 *   + Default mode "DEFAULT" is equal to mode "LTRACE"
 *   + User readable trace using ltrace on bootstraped executable (mode "LTRACE")
 *   + User readable trace using internal tracer (mode "INTERNAL")
 *   + Binary tracing to replay later calls to the standard function (mode "RECORD")
 *     - Each unique identifer passed to the function will have its own tracing file
 *     - Memory dump of arguments, return value and altered memory (destination buffer passed by pointers and errno for example)
 *     - Eventuall compress dumps
 *   + Replay of binary traces (mode "REPLAY")
 *   + Non ltrace based tracers will trace all tracable function calls if the environment variable "FUNCTION_TRACE_LIST" is not set else it will trace only symbols listed separated by the ':' in the variable
 * - Intercepting system calls provinding the same features as function interception (listed in the "SYSCALL_TRACE_MODE" environment variable)
 *   + Default mode "DEFAULT" is equal to "STRACE"
 *   + Using strace instead of ltrace for first mode (mode will be "STRACE")
 *   + Non strace based tracers will trace all tracable system calls if the environment variable "SYSCALL_TRACE_LIST" is not set else it will trace only symbols listed separated by the ':' in the variable
 * - Intercepting signals providing the same features as function interception, enabled in the "SIGNAL_TRACE_MODE" environment variable and listed in the "SIGNAL_TRACE_LIST"
 * - Snapshots of the bootstraped process
 *   + Dumps/Restore virtual memory and registers when triggered
 *   + Triggers can be
 *     - Intercepted function calls
 *     - Intercepted system calls
 *     - Intercepted signals
 *     - Specific value(s) in register(s)
 *     - Specific value(s) in memory
 * - "LOGFILE" actions used in modes environment variables open the targeted files in either binary or string mode depending on the context and will write into it using the O_APPEND mode
 */

#define MODE_DEFAULT                            "DEFAULT"
#define MODE_LOGFILE                            "LOGFILE"
#define MODE_FD                                 "FD"
#define MODE_PIPE                               "PIPE"
#define MODE_LTRACE                             "LTRACE"
#define MODE_STRACE                             "STRACE"
#define MODE_INTERNAL                           "INTERNAL"
#define MODE_RECORD                             "RECORD"
#define MODE_REPLAY                             "REPLAY"

#define COREDUMP_ACTION_BT                      "BT"
#define COREDUMP_ACTION_REGS                    "REGS"
#define COREDUMP_ACTION_DISASM                  "DISASM"
#define COREDUMP_ACTION_COPY                    "COPY"

#define ENV_STDIN_MODE                          "STDIN_MODE"
#define ENV_STDOUT_MODE                         "STDOUT_MODE"
#define ENV_STDERR_MODE                         "STDERR_MODE"
#define ENV_COREDUMP_MODE                       "COREDUMP_MODE"
#define ENV_PRELOADER_CMDLINE_MODE              "PRELOADER_CMDLINE_MODE"
#define ENV_PRELOADER_ENVIRONMENT_MODE          "PRELOADER_ENVIRONMENT_MODE"
#define ENV_CHILD_CMDLINE_MODE                  "CHILD_CMDLINE_MODE"
#define ENV_CHILD_ENVIRONMENT_MODE              "CHILD_ENVIRONMENT_MODE"
#define ENV_FUNCTION_TRACE_MODE                 "FUNCTION_TRACE_MODE"
#define ENV_SYSCALL_TRACE_MODE                  "SYSCALL_TRACE_MODE"
#define ENV_SIGNAL_TRACE_MODE                   "SIGNAL_TRACE_MODE"
#define ENV_SNAPSHOT_TRACE_MODE                 "SNAPSHOT_TRACE_MODE"

#define ENV_STDIN_OUTPUT                        "STDIN_OUTPUT"
#define ENV_STDOUT_OUTPUT                       "STDOUT_OUTPUT"
#define ENV_STDERR_OUTPUT                       "STDERR_OUTPUT"
#define ENV_COREDUMP_OUTPUT                     "COREDUMP_OUTPUT"
#define ENV_PRELOADER_CMDLINE_OUTPUT            "PRELOADER_CMDLINE_OUTPUT"
#define ENV_PRELOADER_ENVIRONMENT_OUTPUT        "PRELOADER_ENVIRONMENT_OUTPUT"
#define ENV_CHILD_CMDLINE_OUTPUT                "CHILD_CMDLINE_OUTPUT"
#define ENV_CHILD_ENVIRONMENT_OUTPUT            "CHILD_ENVIRONMENT_OUTPUT"
#define ENV_FUNCTION_TRACE_OUTPUT               "FUNCTION_TRACE_OUTPUT"
#define ENV_SYSCALL_TRACE_OUTPUT                "SYSCALL_TRACE_OUTPUT"
#define ENV_SIGNAL_TRACE_OUTPUT                 "SIGNAL_TRACE_OUTPUT"
#define ENV_SNAPSHOT_TRACE_OUTPUT               "SNAPSHOT_TRACE_OUTPUT"

#define ENV_STDIN_COMPRESSION                   "STDIN_COMPRESSION"
#define ENV_STDOUT_COMPRESSION                  "STDOUT_COMPRESSION"
#define ENV_STDERR_COMPRESSION                  "STDERR_COMPRESSION"
#define ENV_COREDUMP_COMPRESSION                "COREDUMP_COMPRESSION"
#define ENV_FUNCTION_TRACE_COMPRESSION          "FUNCTION_TRACE_COMPRESSION"
#define ENV_SYSCALL_TRACE_COMPRESSION           "SYSCALL_TRACE_COMPRESSION"
#define ENV_SIGNAL_TRACE_COMPRESSION            "SIGNAL_TRACE_COMPRESSION"
#define ENV_SNAPSHOT_TRACE_COMPRESSION          "SNAPSHOT_TRACE_COMPRESSION"

#define ENV_CHILD_LDPRELOAD                     "CHILD_LDPRELOAD"

#define ENV_COREDUMP_ACTIONS                    "COREDUMP_ACTIONS"

#define ENV_FUNCTION_TRACE_LIST                 "FUNCTION_TRACE_LIST"
#define ENV_SYSCALL_TRACE_LIST                  "SYSCALL_TRACE_LIST"

#define ENV_SNAPSHOT_FUNC_DUMP_TRIGGER          "SNAPSHOT_FUNC_DUMP_TRIGGER"
#define ENV_SNAPSHOT_SYSCALL_DUMP_TRIGGER       "SNAPSHOT_SYSCALL_DUMP_TRIGGER"
#define ENV_SNAPSHOT_SIGNAL_DUMP_TRIGGER        "SNAPSHOT_SIGNAL_DUMP_TRIGGER"
#define ENV_SNAPSHOT_REG_DUMP_TRIGGER           "SNAPSHOT_REG_DUMP_TRIGGER"
#define ENV_SNAPSHOT_MEM_DUMP_TRIGGER           "SNAPSHOT_MEM_DUMP_TRIGGER"

#define ENV_SNAPSHOT_FUNC_RESTORE_TRIGGER       "SNAPSHOT_FUNC_RESTORE_TRIGGER"
#define ENV_SNAPSHOT_SYSCALL_RESTORE_TRIGGER    "SNAPSHOT_SYSCALL_RESTORE_TRIGGER"
#define ENV_SNAPSHOT_SIGNAL_RESTORE_TRIGGER     "SNAPSHOT_SIGNAL_RESTORE_TRIGGER"
#define ENV_SNAPSHOT_REG_RESTORE_TRIGGER        "SNAPSHOT_REG_RESTORE_TRIGGER"
#define ENV_SNAPSHOT_MEM_RESTORE_TRIGGER        "SNAPSHOT_MEM_RESTORE_TRIGGER"

typedef struct modes {
    bool logfile : 1;
    bool fd : 1;
    bool pipe : 1;
    bool ltrace : 1;
    bool strace : 1;
    bool internal : 1;
    bool record : 1;
    bool replay : 1;
} s_modes;

#endif /* PRELOAD_H */
