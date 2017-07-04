/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <unistd.h>
#include <cstddef>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <cerrno>
#include "utils.h"
#include "common-manager.h"
#include "popt.h"

/* Utility functions ********************************************************
// **************************************************************************
*/

static inline
bool
stripfast (
	const char * & prog
) {
	if ('f' != prog[0] || 'a' != prog[1] || 's' != prog[2] || 't' != prog[3]) return false;
	prog += 4;
	return true;
}

/* Signal sending functions *************************************************
// **************************************************************************
*/

/// FIXME \bug This mechanism cannot work.
static inline
int
query_user_manager_pid()
{
	return getsid(0);
}

static
void
send_signal_to_manager_process ( 
	const char * prog,
	int signo
) {
	if (per_user_mode) {
		/// FIXME \bug This mechanism is broken.
		if (0 > kill(query_user_manager_pid(), signo)) {
			const int error(errno);
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "kill", std::strerror(error));
			throw EXIT_FAILURE;
		}
	} else {
		if (0 > kill(1, signo)) {
			const int error(errno);
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "kill", std::strerror(error));
			throw EXIT_FAILURE;
		}
	}
}

static inline
void
emergency (
	const char * prog
) {
	send_signal_to_manager_process(prog, EMERGENCY_SIGNAL) ;
}

static inline
void
rescue (
	const char * prog
) {
	send_signal_to_manager_process(prog, SYSINIT_SIGNAL);
	send_signal_to_manager_process(prog, RESCUE_SIGNAL) ;
}

static inline
void
normal (
	const char * prog
) {
	send_signal_to_manager_process(prog, SYSINIT_SIGNAL);
	send_signal_to_manager_process(prog, NORMAL_SIGNAL) ;
}

static inline
void
reboot (
	const char * prog,
	bool force
) {
	send_signal_to_manager_process(prog, force ? FORCE_REBOOT_SIGNAL : REBOOT_SIGNAL) ;
} 

#if defined(FORCE_HALT_SIGNAL)
static inline
void
halt (
	const char * prog,
	bool force
) {
	send_signal_to_manager_process(prog, force ? FORCE_HALT_SIGNAL : HALT_SIGNAL) ;
}
#endif

#if defined(FORCE_POWEROFF_SIGNAL)
static inline
void
poweroff (
	const char * prog,
	bool force
) {
	send_signal_to_manager_process(prog, force ? FORCE_POWEROFF_SIGNAL : POWEROFF_SIGNAL) ;
}
#endif

/* System control commands **************************************************
// **************************************************************************
// These are the built-in state change commands in system-control.
*/

void
reboot_poweroff_halt_command [[gnu::noreturn]] (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & /*envs*/
) {
	const char * prog(basename_of(args[0]));
	bool force(stripfast(prog));
	if (0 == std::strcmp(prog, "boot")) prog = "reboot";
	try {
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", per_user_mode);
		popt::bool_definition force_option('f', "force", "Bypass service shutdown.", force);
		popt::definition * main_table[] = {
			&user_option,
			&force_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, e.arg, e.msg);
		throw EXIT_FAILURE;
	}

	switch (prog[0]) {
		case 'r':	reboot(prog, force); break;
#if defined(FORCE_HALT_SIGNAL)
		case 'h':	halt(prog, force); break;
#endif
#if defined(FORCE_POWEROFF_SIGNAL)
		case 'p':	poweroff(prog, force); break;
#endif
		default:
			std::fprintf(stderr, "%s: FATAL: %c: %s\n", prog, prog[0], "Unknown action");
			throw EXIT_FAILURE;
	}

	throw EXIT_SUCCESS;
}

void
emergency_rescue_normal_command [[gnu::noreturn]] ( 
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & /*envs*/
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", per_user_mode);
		popt::definition * main_table[] = {
			&user_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, e.arg, e.msg);
		throw EXIT_FAILURE;
	}

	switch (prog[0]) {
		case 'e':	emergency(prog); break;
		case 's':	// single
		case 'r':	rescue(prog); break;
		case 'd':	// default
		case 'n':	normal(prog); break;
		default:
			std::fprintf(stderr, "%s: FATAL: %c: %s\n", prog, prog[0], "Unknown action");
			throw EXIT_FAILURE;
	}

	throw EXIT_SUCCESS;
}

void
init ( 
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & /*envs*/
) {
	const char * prog(basename_of(args[0]));
	enum Action { ///< in order of lowest to highest precedence
		NORMAL = 0,
#if defined(FORCE_HALT_SIGNAL)
		HALT,
#endif
		REBOOT,
#if defined(FORCE_POWEROFF_SIGNAL)
		POWEROFF,
#endif
		UPDATE,
		RESCUE,
		EMERGENCY
	} action = NORMAL;
	try {
		const char * z(0);
		bool rescue_mode(false), emergency_mode(false), update_mode(false);
		bool ignore;
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", per_user_mode);
#if defined(__LINUX__) || defined(__linux__)
		popt::bool_definition rescue_option('s', "single", "Start in rescue mode.", rescue_mode);
		popt::bool_definition emergency_option('b', "emergency", "Start in emergency mode.", emergency_mode);
#else
		popt::bool_definition emergency_option('s', "single", "Start in emergency mode.", emergency_mode);
#endif
		popt::bool_definition update_option('o', "update", "Start in update mode.", update_mode);
		popt::bool_definition autoboot_option('a', 0, "Compatibility option, ignored.", ignore);
		popt::bool_definition fastboot_option('f', 0, "Compatibility option, ignored.", ignore);
		popt::string_definition z_option('z', 0, "string", "Compatibility option, ignored.", z);
		popt::definition * top_table[] = {
			&user_option,
#if defined(__LINUX__) || defined(__linux__)
			&rescue_option,
			&emergency_option,
#else
			&emergency_option,
#endif
			&update_option,
			&autoboot_option,
			&fastboot_option,
			&z_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "runlevel(s)...");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, main_option, new_args);
		p.process(false /* intermix options and arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
		if (emergency_mode && action < EMERGENCY) action = EMERGENCY;
		if (rescue_mode && action < RESCUE) action = RESCUE;
		if (update_mode && action < UPDATE) action = UPDATE;
	} catch (const popt::error & e) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, e.arg, e.msg);
		throw EXIT_FAILURE;
	}

	for (std::vector<const char *>::const_iterator i(args.begin()); args.end() != i; ++i) {
		const char *p(*i);
		if (p[0] && !p[1]) switch (p[0]) {
			case 'B':
			case 'b':
				if (action < EMERGENCY) action = EMERGENCY;
				break;
			case '1':
			case 'S':
			case 's':
				if (action < RESCUE) action = RESCUE;
				break;
#if defined(FORCE_HALT_SIGNAL)
			case 'H':
			case 'h':
				if (action < HALT) action = HALT;
				break;
#endif
#if defined(FORCE_POWEROFF_SIGNAL)
			case '0':
				if (action < POWEROFF) action = POWEROFF;
				break;
#endif
			case '6':
				if (action < REBOOT) action = REBOOT;
				break;
			case '2':
			case '3':
			case '4':
			case '5':
			case 'm':
				if (action < NORMAL) action = NORMAL;
				break;
			default:
				std::fprintf(stderr, "%s: WARNING: %c: %s\n", prog, p[0], "Unknown run level ignored.");
				break;
		} else
		if (0 == std::strcmp("emergency", p)) {
			if (action < EMERGENCY) action = EMERGENCY;
		} else
		if (0 == std::strcmp("rescue", p)
		||  0 == std::strcmp("single", p)) {
			if (action < RESCUE) action = RESCUE;
		} else
		if (0 == std::strcmp("update", p)) {
			if (action < UPDATE) action = UPDATE;
		} else
		if (0 == std::strcmp("auto", p)
		||  0 == std::strcmp("default", p)
		||  0 == std::strcmp("normal", p)
		||  0 == std::strcmp("multi", p)) {
			if (action < NORMAL) action = NORMAL;
		} else
			std::fprintf(stderr, "%s: WARNING: %s: %s\n", prog, p, "Unknown run level ignored.");
	}

	switch (action) {
		case EMERGENCY:
			emergency(prog);
			break;
		case RESCUE:
			rescue(prog);
			break;
		case UPDATE:
			args.clear();
			args.insert(args.end(), "update");
			args.insert(args.end(), 0);
			next_prog = arg0_of(args);
			return;
#if defined(FORCE_POWEROFF_SIGNAL)
		case POWEROFF:
			poweroff(prog, false);
			break;
#endif
		case REBOOT:
			reboot(prog, false);
			break;
#if defined(FORCE_HALT_SIGNAL)
		case HALT:
			halt(prog, false);
			break;
#endif
		case NORMAL:
		default:
			normal(prog);
			break;
	}
	throw EXIT_SUCCESS;
}
