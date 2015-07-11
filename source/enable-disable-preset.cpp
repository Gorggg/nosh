/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <new>
#include <memory>
#include <unistd.h>
#include <dirent.h>
#include <ttyent.h>
#include <fstab.h>
#include <fcntl.h>
#include "utils.h"
#include "fdutils.h"
#include "service-manager-client.h"
#include "service-manager.h"
#include "common-manager.h"
#include "popt.h"
#include "FileStar.h"

/* Common Internals *********************************************************
// **************************************************************************
*/

static inline
bool
enable_disable ( 
	const char * prog,
	bool make,
	const std::string & path,
	const int bundle_dir_fd,
	const std::string & source,
	const std::string & target
) {
	const std::string source_dir_name(path + "/" + source);
	const std::string base(basename_of(path.c_str()));
	const int source_dir_fd(open_dir_at(bundle_dir_fd, (source + "/").c_str()));
	if (source_dir_fd < 0) {
		const int error(errno);
		if (ENOENT != error)
			std::fprintf(stderr, "%s: WARNING: %s: %s\n", prog, source_dir_name.c_str(), std::strerror(error));
		return false;
	}
	DIR * source_dir(fdopendir(source_dir_fd));
	if (!source_dir) {
		const int error(errno);
		std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, source_dir_name.c_str(), std::strerror(error));
		return false;
	}
	bool failed(false);
	for (;;) {
		errno = 0;
		const dirent * entry(readdir(source_dir));
		if (!entry) {
			const int error(errno);
			if (error) {
				std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, source_dir_name.c_str(), std::strerror(error));
				failed = true;
			}
			break;
		}
#if defined(_DIRENT_HAVE_D_TYPE)
		if (DT_DIR != entry->d_type && DT_LNK != entry->d_type) continue;
#endif
#if defined(_DIRENT_HAVE_D_NAMLEN)
		if (1 > entry->d_namlen) continue;
#endif
		if ('.' == entry->d_name[0]) continue;

		const int target_dir_fd(open_dir_at(source_dir_fd, entry->d_name));
		if (0 > target_dir_fd) {
			const int error(errno);
			std::fprintf(stderr, "%s: ERROR: %s/%s: %s\n", prog, source_dir_name.c_str(), entry->d_name, std::strerror(error));
			failed = true;
			continue;
		}
		if (make)
			mkdirat(target_dir_fd, target.c_str(), 0755);
		if (0 > unlinkat(target_dir_fd, (target + "/" + base).c_str(), 0)) {
			const int error(errno);
			if (ENOENT != error) {
				std::fprintf(stderr, "%s: ERROR: %s/%s/%s/%s: %s\n", prog, source_dir_name.c_str(), entry->d_name, target.c_str(), base.c_str(), std::strerror(error));
				failed = true;
			}
		}
		if (make) {
			if (0 > symlinkat(path.c_str(), target_dir_fd, (target + "/" + base).c_str())) {
				const int error(errno);
				std::fprintf(stderr, "%s: ERROR: %s/%s/%s/%s: %s\n", prog, source_dir_name.c_str(), entry->d_name, target.c_str(), base.c_str(), std::strerror(error));
				failed = true;
			}
		}
		close(target_dir_fd);
	}
	closedir(source_dir);
	return !failed;
}

static inline
bool
enable_disable ( 
	const char * prog,
	bool make,
	const std::string & arg,
	const std::string & path,
	const int bundle_dir_fd
) {
	bool failed(false);
	if (!enable_disable(prog, make, path, bundle_dir_fd, "wanted-by", "wants"))
		failed = true;
	if (!enable_disable(prog, make, path, bundle_dir_fd, "stopped-by", "conflicts"))
		failed = true;
	if (!enable_disable(prog, make, path, bundle_dir_fd, "stopped-by", "after"))
		failed = true;
	const int service_dir_fd(open_service_dir(bundle_dir_fd));
	if (make) {
		const int rc(unlinkat(service_dir_fd, "down", 0));
		if (0 > rc && ENOENT != errno) {
			const int error(errno);
			std::fprintf(stderr, "%s: ERROR: %s: %s/%s: %s\n", prog, arg.c_str(), path.c_str(), "down", std::strerror(error));
			failed = true;
		}
	} else {
		const int fd(open_writecreate_at(service_dir_fd, "down", 0600));
		if (0 > fd) {
			const int error(errno);
			std::fprintf(stderr, "%s: ERROR: %s: %s/%s: %s\n", prog, arg.c_str(), path.c_str(), "down", std::strerror(error));
			failed = true;
		} else
			close(fd);
	}
	close(service_dir_fd);
	return !failed;
}

/* Preset information *******************************************************
// **************************************************************************
*/

static inline
bool
checkyesno (
	const std::string & s
) {
	if ("0" == s) return false;
	if ("1" == s) return true;
	const std::string l(tolower(s));
	if ("no" == l) return false;
	if ("yes" == l) return true;
	if ("false" == l) return false;
	if ("true" == l) return true;
	if ("off" == l) return false;
	if ("on" == l) return true;
	return false;
}

static
const char *
rcconf_files[] = {
	"/etc/rc.conf.local",
	"/etc/rc.conf",
};

static inline
bool	/// \returns setting \retval true explicit \retval false defaulted
query_rcconf_preset (
	bool & wants,	///< always set to a value
	const char * prog,
	const std::string & name
) {
	const std::string wanted(name + "_enable");
	for (size_t i(0); i < sizeof rcconf_files/sizeof *rcconf_files; ++i) {
		const std::string rcconf_file(rcconf_files[i]);
		FILE * f(std::fopen(rcconf_file.c_str(), "r"));
		if (!f) continue;
		std::vector<std::string> env_strings(read_file(prog, rcconf_file.c_str(), f));
		for (std::vector<std::string>::const_iterator j(env_strings.begin()); j != env_strings.end(); ++j) {
			const std::string & s(*j);
			const std::string::size_type p(s.find('='));
			const std::string var(s.substr(0, p));
			if (var != wanted) continue;
			const std::string val(p == std::string::npos ? std::string() : s.substr(p + 1, std::string::npos));
			wants = checkyesno(val);
			return true;
		}
	}
	wants = false;
	return false;
}

static inline
bool
initial_space (
	const std::string & s
) {
	return s.length() > 0 && std::isspace(s[0]);
}

static inline
bool
wildmat (
	std::string::const_iterator p,
	const std::string::const_iterator & pe,
	std::string::const_iterator n,
	const std::string::const_iterator & ne
) {
	for (;;) {
		if (p == pe && n == ne) return true;
		if (p == pe || n == ne) return false;
		switch (*p) {
			case '*':
				do { ++p; } while (p != pe && '*' == *p);
				for (std::string::const_iterator b(ne); b != n; --b)
					if (wildmat(p, pe, b, ne)) 
						return true;
				break;
			case '\\':
				++p;
				if (p == pe) return false;
				// Fall through to:
			default:
				if (*p != *n) return false;
				// Fall through to:
			case '?':
				++p;
				++n;
				break;
		}
	}
}

static inline
bool
wildmat (
	const std::string & pattern,
	const std::string & name
) {
	return wildmat(pattern.begin(), pattern.end(), name.begin(), name.end());
}

static inline
bool
matches (
	const std::string & pattern,
	const std::string & name,
	const std::string & suffix
) {
	std::string base;
	if (ends_in(pattern, suffix, base))
		return wildmat(base, name);
	else
		return wildmat(pattern, name);
}

static
bool
scan (
	const std::string & name,
	const std::string & suffix,
	FILE * file,
	bool & wants_enable
) {
	for (std::string line; read_line(file, line); ) {
		line = ltrim(line);
		if (line.length() < 1) continue;
		if ('#' == line[0] || ';' == line[0]) continue;
		std::string remainder;
		if (begins_with(line, "enable", remainder) && initial_space(remainder)) {
			remainder = rtrim(ltrim(remainder));
			if (matches(remainder, name, suffix)) {
				wants_enable = true;
				return true;
			}
		} else
		if (begins_with(line, "disable", remainder) && initial_space(remainder)) {
			remainder = rtrim(ltrim(remainder));
			if (matches(remainder, name, suffix)) {
				wants_enable = false;
				return true;
			}
		}
	}
	return false;
}

static
const char *
preset_directories[] = {
	"/etc/system-control/presets/",
	"/etc/systemd/system-preset/",
	"/usr/share/system-control/presets/",
	"/lib/systemd/system-preset/",
	"/usr/lib/systemd/system-preset/",
	"/usr/local/etc/system-control/presets/",
	"/usr/local/lib/systemd/system-preset/",
};

static inline
bool	/// \returns setting \retval true explicit \retval false defaulted
query_systemd_preset (
	bool & wants,	///< always set to a value
	const std::string & name,
	const std::string & suffix
) {
	wants = true;
	std::string earliest;
	for (size_t i(0); i < sizeof preset_directories/sizeof *preset_directories; ++i) {
		const std::string preset_dir_name(preset_directories[i]);
		const int preset_dir_fd(open_dir_at(AT_FDCWD, preset_dir_name.c_str()));
		if (preset_dir_fd < 0) continue;
		DIR * preset_dir(fdopendir(preset_dir_fd));
		if (!preset_dir) continue;
		for (;;) {
			errno = 0;
			const dirent * entry(readdir(preset_dir));
			if (!entry) break;
#if defined(_DIRENT_HAVE_D_TYPE)
			if (DT_REG != entry->d_type && DT_LNK != entry->d_type) continue;
#endif
#if defined(_DIRENT_HAVE_D_NAMLEN)
			if (1 > entry->d_namlen) continue;
#endif
			if ('.' == entry->d_name[0]) continue;
			const std::string d_name(entry->d_name);
			if (!earliest.empty() && earliest <= d_name) continue;
			const std::string p(preset_dir_name + d_name);
			const int f(open_read_at(preset_dir_fd, entry->d_name));
			if (0 > f) continue;
			FileStar preset_file(fdopen(f, "rt"));
			if (!preset_file) continue;
			if (scan(name, suffix, preset_file, wants))
				earliest = d_name;
		}
		closedir(preset_dir);
	}
	return !earliest.empty();
}

#if defined(TTY_ONIFCONSOLE)
static inline
bool
is_current_console (
	const struct ttyent & entry
) {
#if !defined(__LINUX__) && !defined(__linux__)
	int oid[CTL_MAXNAME];
	std::size_t len(sizeof oid/sizeof *oid);
	const int r(sysctlnametomib("kern.console", oid, &len));
	if (0 > r) return false;
	std::size_t siz;
	const int s(sysctl(oid, len, 0, &siz, 0, 0));
	if (0 > s) return false;
	std::auto_ptr<char> buf(new(std::nothrow) char[siz]);
	const int t(sysctl(oid, len, buf, &siz, 0, 0));
	if (0 > t) return false;
	const char * avail(std::strchr(buf, '/'));
	if (!avail) return false;
	*avail++ = '\0';
	for (const char * p(buf), * e(0); *p; p = e) {
		e = std::strchr(p, ',');
		if (e) *e++ = '\0'; else e = std::strchr(p, '\0');
		if (0 == std::strcmp(p, entry.ty_name)) return true;
	}
#endif
	return false;
}
#endif

static inline
bool
is_on (
	const struct ttyent & entry
) {
	return (entry.ty_status & TTY_ON) 
#if defined(TTY_ONIFCONSOLE)
		|| ((entry.ty_status & TTY_ONIFCONSOLE) && is_current_console(entry))
#endif
	;
}

static inline
bool	/// \returns setting \retval true explicit \retval false defaulted
query_ttys_preset (
	bool & wants,	///< always set to a value
	const std::string & name
) {
	if (!setttyent()) {
		wants = true;
		return false;
	}
	const struct ttyent *entry(getttynam(name.c_str()));
	wants = entry && is_on(*entry);
	endttyent();
	return entry != 0;
}

static inline
std::string 
unescape ( 
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		char c(*q++);
		if ('-' == c) {
			r += '/';
		} else
		if ('\\' != c) {
			r += c;
		} else
		if (e == q) {
			r += c;
		} else
		{
			c = *q++;
			if ('X' != c && 'x' != c)
				r += c;
			else {
				unsigned v(0U);
				for (unsigned n(0U);n < 2U; ++n) {
					if (e == q) break;
					c = *q;
					if (!std::isxdigit(c)) break;
					++q;
					c = std::isdigit(c) ? (c - '0') : (std::tolower(c) - 'a' + 10);
					v = (v << 4) | c;
				}
				r += char(v);
			}
		}
	}
	return r;
}

static inline
bool
is_auto (
	const struct fstab & entry
) {
	const std::list<std::string> options(split_fstab_options(entry.fs_mntops));
	return !has_option(options, "noauto");
}

static inline
bool	/// \returns setting \retval true explicit \retval false defaulted
query_fstab_preset (
	bool & wants,	///< always set to a value
	const std::string & escaped_name
) {
	if (!setfsent()) {
		wants = true;
		return false;
	}
	const std::string name(unescape(escaped_name));
	const struct fstab *entry(getfsfile(name.c_str()));
	if (!entry) entry = getfsspec(name.c_str());
	wants = entry && is_auto(*entry);
	endfsent();
	return entry != 0;
}

static inline
bool
determine_preset (
	const char * prog,
	bool system,
	bool rcconf,
	bool ttys,
	bool fstab,
	const std::string & prefix,
	const std::string & name,
	const std::string & suffix
) {
	bool wants(false);
	// systemd (and system-manager) settings take precedence over compatibility ones.
	if (system && query_systemd_preset(wants, prefix + name, suffix))
		return wants;
	// The newer BSD rc.conf takes precedence over the older Sixth Edition ttys .
	if (rcconf && query_rcconf_preset(wants, prog, name))
		return wants;
	if (ttys && query_ttys_preset(wants, name))
		return wants;
	if (fstab && query_fstab_preset(wants, name))
		return wants;
	return wants;
}

/* System control subcommands ***********************************************
// **************************************************************************
*/

void
enable ( 
	const char * & next_prog,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", local_session_mode);
		popt::definition * main_table[] = {
			&user_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "service(s)...");

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

	bool failed(false);
	for (std::vector<const char *>::const_iterator i(args.begin()); args.end() != i; ++i) {
		std::string path, name, suffix;
		const int bundle_dir_fd(open_bundle_directory("", *i, path, name, suffix));
		if (0 > bundle_dir_fd) {
			const int error(errno);
			std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, *i, std::strerror(error));
			continue;
		}
		const std::string p(path + name);
		if (!enable_disable(prog, true, *i, p, bundle_dir_fd))
			failed = true;
		close(bundle_dir_fd);
	}

	throw failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

void
disable ( 
	const char * & next_prog,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", local_session_mode);
		popt::definition * main_table[] = {
			&user_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "service(s)...");

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

	bool failed(false);
	for (std::vector<const char *>::const_iterator i(args.begin()); args.end() != i; ++i) {
		std::string path, name, suffix;
		const int bundle_dir_fd(open_bundle_directory("", *i, path, name, suffix));
		if (0 > bundle_dir_fd) {
			const int error(errno);
			std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, *i, std::strerror(error));
			continue;
		}
		const std::string p(path + name);
		if (!enable_disable(prog, false, *i, p, bundle_dir_fd))
			failed = true;
		close(bundle_dir_fd);
	}

	throw failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

void
preset ( 
	const char * & next_prog,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	const char * prefix("");
	bool no_rcconf(false), no_system(false), ttys(false), fstab(false), dry_run(false);
	try {
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", local_session_mode);
		popt::bool_definition no_system_option('\0', "no-systemd", "Do not process system-manager/systemd preset files.", no_system);
		popt::bool_definition no_rcconf_option('\0', "no-rcconf", "Do not process /etc/rc.conf presets.", no_rcconf);
		popt::bool_definition ttys_option('\0', "ttys", "Process /etc/ttys presets.", ttys);
		popt::bool_definition fstab_option('\0', "fstab", "Process /etc/fstab presets.", fstab);
		popt::bool_definition dry_run_option('n', "dry-run", "Don't actually enact the enable/disable.", dry_run);
		popt::string_definition prefix_option('p', "prefix", "string", "Prefix each name with this (template) name.", prefix);
		popt::definition * main_table[] = {
			&user_option,
			&no_system_option,
			&no_rcconf_option,
			&ttys_option,
			&fstab_option,
			&dry_run_option,
			&prefix_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "service(s)...");

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

	bool failed(false);
	const std::string p(prefix);
	for (std::vector<const char *>::const_iterator i(args.begin()); args.end() != i; ++i) {
		std::string path, name, suffix;
		const int bundle_dir_fd(open_bundle_directory(prefix, *i, path, name, suffix));
		if (0 > bundle_dir_fd) {
			const int error(errno);
			std::fprintf(stderr, "%s: ERROR: %s%s: %s\n", prog, prefix, name.c_str(), std::strerror(error));
			failed = true;
			continue;
		}
		const bool make(determine_preset(prog, !no_system, !no_rcconf, ttys, fstab, p, name, suffix));
		if (dry_run)
			std::fprintf(stdout, "%s %s\n", make ? "enable" : "disable", (path + p + name).c_str());
		else
		if (!enable_disable(prog, make, p + *i, path + p + name, bundle_dir_fd))
			failed = true;
		close(bundle_dir_fd);
	}

	throw failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
