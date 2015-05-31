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
#include "popt.h"

/* The service shim *********************************************************
// **************************************************************************
*/

void
service ( 
	const char * & next_prog,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, 0, "Main options", "service-name command");

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

	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing service name.");
		throw EXIT_FAILURE;
	}
	const char * service(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing command name.");
		throw EXIT_FAILURE;
	}
	const char * command(args.front());
	args.erase(args.begin());
	if (!args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Unrecognized extra arguments.");
		throw EXIT_FAILURE;
	}

	args.clear();
	args.insert(args.end(), "system-control");
	args.insert(args.end(), command);
	args.insert(args.end(), service);
	next_prog = arg0_of(args);
}

void
invoke_rcd ( 
	const char * & next_prog,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, 0, "Main options", "service-name command");

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

	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing service name.");
		throw EXIT_FAILURE;
	}
	const char * service(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing command name.");
		throw EXIT_FAILURE;
	}
	const char * command(args.front());
	args.erase(args.begin());
	if (!args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Unrecognized extra arguments.");
		throw EXIT_FAILURE;
	}

	if (0 == std::strcmp("start", command))
		command = "reset";
	else
	if (0 == std::strcmp("stop", command))
		;	// Don't touch it.
	else 
	{
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, command, "Unsupported subcommand.");
		throw EXIT_FAILURE;
	}

	args.clear();
	args.insert(args.end(), "system-control");
	args.insert(args.end(), command);
	args.insert(args.end(), service);
	next_prog = arg0_of(args);
}

void
update_rcd ( 
	const char * & next_prog,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, 0, "Main options", "service-name command");

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

	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing service name.");
		throw EXIT_FAILURE;
	}
	const char * service(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing command name.");
		throw EXIT_FAILURE;
	}
	const char * command(args.front());
	args.erase(args.begin());
	if (!args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Unrecognized extra arguments.");
		throw EXIT_FAILURE;
	}

	if (0 == std::strcmp("enable", command))
		command = "preset";
	else
	if (0 == std::strcmp("disable", command))
		;	// Don't touch it.
	else 
	if (0 == std::strcmp("remove", command))
		command = "disable";
	else
	if (0 == std::strcmp("defaults", command))
		command = "preset";
	else
	{
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, command, "Unsupported subcommand.");
		throw EXIT_FAILURE;
	}

	args.clear();
	args.insert(args.end(), "system-control");
	args.insert(args.end(), command);
	args.insert(args.end(), service);
	next_prog = arg0_of(args);
}

void
chkconfig ( 
	const char * & next_prog,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, 0, "Main options", "service-name command");

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

	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing service name.");
		throw EXIT_FAILURE;
	}
	const char * service(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing command name.");
		throw EXIT_FAILURE;
	}
	const char * command(args.front());
	args.erase(args.begin());
	if (!args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Unrecognized extra arguments.");
		throw EXIT_FAILURE;
	}

	if (0 == std::strcmp("on", command))
		command = "enable";
	else
	if (0 == std::strcmp("off", command))
		command = "disable";
	else 
	{
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, command, "Unsupported subcommand.");
		throw EXIT_FAILURE;
	}

	args.clear();
	args.insert(args.end(), "system-control");
	args.insert(args.end(), command);
	args.insert(args.end(), service);
	next_prog = arg0_of(args);
}
