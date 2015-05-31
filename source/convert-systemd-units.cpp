/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>	// Necessary for the SO_REUSEPORT macro.
#include <netinet/in.h>	// Necessary for the IPV6_V6ONLY macro.
#include <unistd.h>
#include <pwd.h>
#include "utils.h"
#include "fdutils.h"
#include "common-manager.h"
#include "service-manager-client.h"
#include "popt.h"
#include "FileStar.h"
#include "FileDescriptorOwner.h"

static 
const char * systemd_prefixes[] = {
	"/run/", "/etc/", "/lib/", "/usr/lib/", "/usr/local/lib/"
};

static inline
void
split_name (
	const char * s,
	std::string & dirname,
	std::string & basename
) {
	if (const char * slash = std::strrchr(s, '/')) {
		basename = slash + 1;
		dirname = std::string(s, slash + 1);
#if defined(__OS2__) || defined(__WIN32__) || defined(__NT__)
	} else if (const char * bslash = std::strrchr(s, '\\')) {
		basename = bslash + 1;
		dirname = std::string(s, bslash + 1);
	} else if (std::isalpha(s[0]) && ':' == s[1]) {
		basename = s + 2;
		dirname = std::string(s, s + 2);
#endif
	} else {
		basename = s;
		dirname = std::string();
	}
}

static inline
FILE *
find (
	const std::string & name,
	std::string & path
) {
	if (std::string::npos != name.find('/')) {
		path = name;
		FILE * f = std::fopen(path.c_str(), "r");
		if (f) return f;
	} else {
		int error(ENOENT);	// the most interesting error encountered
		for ( const char ** p(systemd_prefixes); p < systemd_prefixes + sizeof systemd_prefixes/sizeof *systemd_prefixes; ++p) {
			path = ((std::string(*p) + "systemd/") + (local_session_mode ? "user/" : "system/")) + name;
			FILE * f = std::fopen(path.c_str(), "r");
			if (f) return f;
			if (ENOENT == errno) 
				errno = error;	// Restore a more interesting error.
			else
				error = errno;	// Save this interesting error.
		}
	}
	return NULL;
}

static
bool
is_regular (
	const char * prog,
	const std::string & name,
	FILE * file
) {
	struct stat s;
	if (0 > fstat(fileno(file), &s)) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, name.c_str(), std::strerror(error));
		return false;
	}
	if (!S_ISREG(s.st_mode)) {
		std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, name.c_str(), "Not a regular file.");
		return false;
	}
	return true;
}

struct value {
	value() : used(false), d() {}
	value(const std::string & v) : used(false), d() { d.push_back(v); }
	void append(const std::string & v) { d.push_back(v); }
	std::string last_setting() const { return d.empty() ? std::string() : d.back(); }
	const std::list<std::string> & all_settings() const { return d; }
	bool used;
protected:
	std::list<std::string> d;
};

struct profile {
	typedef std::map<std::string, value> SecondLevel;
	typedef std::map<std::string, SecondLevel> FirstLevel;

	value * use ( 
		const std::string & k0, ///< must be all lowercase
		const std::string & k1 ///< must be all lowercase
	) 
	{
		FirstLevel::iterator i0(m0.find(k0));
		if (m0.end() == i0) return NULL;
		SecondLevel & m1(i0->second);
		SecondLevel::iterator i1(m1.find(k1));
		if (m1.end() == i1) return NULL;
		i1->second.used = true;
		return &i1->second;
	}
	void append ( 
		const std::string & k0, ///< must be all lowercase
		const std::string & k1, ///< must be all lowercase
		const std::string & v 
	) 
	{
		m0[k0][k1].append(v);
	}

	FirstLevel m0;
};

static
bool
strip_leading_minus (
	const std::string & s,
	std::string & r
) {
	if (s.length() < 1 || '-' != s[0]) {
		r = s;
		return false;
	} else {
		r = s.substr(1, std::string::npos);
		return true;
	}
}

static
std::string
strip_leading_minus (
	const std::string & s
) {
	if (s.length() < 1 || '-' != s[0]) return s;
	return s.substr(1, std::string::npos);
}

static
std::string
escape_newlines (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator p(s.begin()); s.end() != p; ++p) {
		const char c(*p);
		if ('\n' == c) 
			r += " \\";
		r += c;
	}
	return r;
}

static
std::string
quote (
	const std::string & s
) {
	std::string r;
	bool quote(s.empty());
	for (std::string::const_iterator p(s.begin()); s.end() != p; ++p) {
		const char c(*p);
		if (!std::isalnum(c) && '/' != c && '-' != c && '_' != c && '.' != c) {
			quote = true;
			if ('\"' == c || '\\' == c)
				r += '\\';
		}
		r += c;
	}
	if (quote) r = "\"" + r + "\"";
	return r;
}

static inline
std::string
leading_slashify (
	const std::string & s
) {
	return "/" == s.substr(0, 1) ? s : "/" + s;
}

static inline
std::string
shell_expand (
	const std::string & s
) {
	enum { NORMAL, DQUOT, SQUOT } state(NORMAL);
	for (std::string::const_iterator p(s.begin()); s.end() != p; ++p) {
		const char c(*p);
		switch (state) {
			case NORMAL:
				if ('\\' == c && p != s.end()) ++p;
				else if ('\'' == c) state = SQUOT;
				else if ('\"' == c) state = DQUOT;
				else if ('$' == c)
					return "sh -c " + quote("exec " + s);
				break;
			case DQUOT:
				if ('\\' == c && p != s.end()) ++p;
				else if ('\"' == c) state = NORMAL;
				else if ('$' == c)
					return "sh -c " + quote("exec " + s);
				break;
			case SQUOT:
				if ('\\' == c && p != s.end()) ++p;
				else if ('\'' == c) state = NORMAL;
				break;
		}
	}
	return s;
}

struct names {
	names(const char * a) : arg_name(a) { split_name(a, unit_dirname, unit_basename); }
	void set_prefix(const std::string & v, bool esc, bool alt) { set(esc, alt, escaped_prefix, prefix, v); }
	void set_instance(const std::string & v, bool esc, bool alt) { set(esc, alt, escaped_instance, instance, v); }
	void set_bundle_basename(const std::string & v) { bundle_basename = v; }
	void set_bundle_dirname(const std::string & v) { bundle_dirname = v; }
	const std::string & query_arg_name () const { return arg_name; }
	const std::string & query_unit_dirname () const { return unit_dirname; }
	const std::string & query_unit_basename () const { return unit_basename; }
	const std::string & query_escaped_unit_basename () const { return escaped_unit_basename; }
	const std::string & query_prefix () const { return prefix; }
	const std::string & query_escaped_prefix () const { return escaped_prefix; }
	const std::string & query_instance () const { return instance; }
	const std::string & query_escaped_instance () const { return escaped_instance; }
	const std::string & query_bundle_basename () const { return bundle_basename; }
	const std::string & query_bundle_dirname () const { return bundle_dirname; }
	std::string substitute ( const std::string & );
	std::list<std::string> substitute ( const std::list<std::string> & );
protected:
	std::string arg_name, unit_dirname, unit_basename, escaped_unit_basename, prefix, escaped_prefix, instance, escaped_instance, bundle_basename, bundle_dirname;
	static std::string unescape ( bool, const std::string & );
	static std::string escape ( bool, const std::string & );
	void set ( bool esc, bool at, std::string & escaped, std::string & normal, const std::string & value ) {
		if (esc)
			normal = unescape(at, escaped = value);
		else
			escaped = escape(at, normal = value);
	}
};

std::string 
names::unescape ( 
	bool alt,
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		char c(*q++);
		if (!alt && '-' == c) {
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

std::string 
names::escape ( 
	bool alt,
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		char c(*q++);
		if (!alt && '/' == c) {
			r += '-';
		} else
		if (!alt ? '-' == c : '@' == c || '/' == c || '\\' == c || ':' == c || ',' == c) {
			char buf[6];
			snprintf(buf, sizeof buf, "\\x%02x", c);
			r += std::string(buf);
		} else
			r += c;
	}
	return r;
}

std::string
names::substitute (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ++q) {
		char c(*q);
		if ('%' != c) {
			r += c;
			continue;
		}
		++q;
		if (e == q) {
			r += c;
			--q;
			continue;
		}
		switch (c = *q) {
			case 'p': r += query_escaped_prefix(); break;
			case 'P': r += query_prefix(); break;
			case 'i': r += query_escaped_instance(); break;
			case 'f': r += leading_slashify(query_instance()); break;
			case 'I': r += query_instance(); break;
			case 'n': r += query_escaped_unit_basename(); break;
			case 'N': r += query_unit_basename(); break;
			case '%': default:	r += '%'; r += c; break;
		}
	}
	return r;
}

std::list<std::string> 
names::substitute ( 
	const std::list<std::string> & l
) {
	std::list<std::string> r;
	for (std::list<std::string>::const_iterator i(l.begin()); l.end() != i; ++i)
		r.push_back(substitute(*i));
	return r;
}

static
bool
is_bool_true (
	const value * v,
	bool def
) {
	if (!v) return def;
	const std::string r(tolower(v->last_setting()));
	return "yes" == r || "on" == r || "true" == r;
}

static
bool
is_local_socket_name (
	const std::string & s
) {
	return s.length() > 0 && '/' == s[0];
}

static inline
bool
bracketed (
	const std::string & s
) {
	return s.length() > 1 && '[' == s[0] && ']' == s[s.length() - 1];
}

static
void
split_ip_socket_name (
	const std::string & s, 
	std::string & listenaddress, 
	std::string & listenport
) {
	const std::string::size_type colon(s.rfind(':'));
	if (std::string::npos == colon) {
		listenaddress = "::0";
		listenport = s;
	} else {
		listenaddress = s.substr(0, colon);
		listenport = s.substr(colon + 1, std::string::npos);
		if (bracketed(listenaddress))
			listenaddress = listenaddress.substr(1, colon - 2);
	}
}

static inline
bool
is_section_heading (
	std::string line,	///< line, already left trimmed by the caller
	std::string & section	///< overwritten only if we successfully parse the heading
) {
	line = rtrim(line);
	if (!bracketed(line)) return false;
	section = tolower(line.substr(1, line.length() - 2));
	return true;
}

static
void
load (
	profile & p,
	FILE * file
) {
	for (std::string line, section; read_line(file, line); ) {
		line = ltrim(line);
		if (line.length() < 1) continue;
		if ('#' == line[0] || ';' == line[0]) continue;
		if (is_section_heading(line, section)) continue;
		const std::string::size_type eq(line.find('='));
		const std::string var(line.substr(0, eq));
		const std::string val(eq == std::string::npos ? std::string() : line.substr(eq + 1, std::string::npos));
		p.append(section, tolower(var), val);
	}
}

static
void
create_link (
	const char * prog,
	int bundle_dir_fd,
	const std::string & target,
	const std::string & link
) {
	if (0 > unlinkat(bundle_dir_fd, link.c_str(), 0)) {
		const int error(errno);
		if (ENOENT != error)
			std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, link.c_str(), std::strerror(error));
	}
	if (0 > symlinkat(target.c_str(), bundle_dir_fd, link.c_str())) {
		const int error(errno);
		std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, link.c_str(), std::strerror(error));
	}
}

static
void
create_links (
	const char * prog,
	const bool is_target,
	const bool etc_service,
	int bundle_dir_fd,
	const std::string & names,
	const std::string & subdir
) {
	const std::list<std::string> list(split_list(names));
	for (std::list<std::string>::const_iterator i(list.begin()); list.end() != i; ++i) {
		const std::string & name(*i);
		std::string base, link, target;
		if (ends_in(name, ".target", base)) {
			if (is_target)
				target = "../../" + base;
			else
			if (etc_service)
				target = "../../../system-manager/targets/" + base;
			else
				target = "/etc/system-manager/targets/" + base;
			link = subdir + base;
		} else
		if (ends_in(name, ".service", base)) {
			if (is_target||etc_service)
				target = "/var/sv/" + base;
			else
				target = "../../" + base;
			link = subdir + base;
		} else 
		{
			if (is_target||etc_service)
				target = "/var/sv/" + name;
			else
				target = "../../" + name;
			link = subdir + name;
		}
		create_link(prog, bundle_dir_fd, target, link);
	}
}

static
void
open (
	const char * prog,
	std::ofstream & file,
	const std::string & name
) {
	file.open(name.c_str(), std::ios::trunc|std::ios::out);
	if (!file) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, name.c_str(), std::strerror(error));
		throw EXIT_FAILURE;
	}
	chmod(name.c_str(), 0755);
}

static
void
report_unused (
	const char * prog,
	profile & p,
	const std::string & name
) {
	for (profile::FirstLevel::const_iterator i0(p.m0.begin()); p.m0.end() != i0; ++i0) {
		const std::string & section(i0->first);
		const profile::SecondLevel & m1(i0->second);
		for (profile::SecondLevel::const_iterator i1(m1.begin()); m1.end() != i1; ++i1) {
			const std::string & var(i1->first);
			const value & v(i1->second);
			if (!v.used) {
				for (std::list<std::string>::const_iterator i(v.all_settings().begin()); v.all_settings().end() != i; ++i) {
				       const std::string & val(*i);
			       	       std::fprintf(stderr, "%s: WARNING: %s: Unused setting: [%s] %s = %s\n", prog, name.c_str(), section.c_str(), var.c_str(), val.c_str());
				}
			}
		}
	}
}

/* Main function ************************************************************
// **************************************************************************
*/

void
convert_systemd_units (
	const char * & /*next_prog*/,
	std::vector<const char *> & args
) {
	const char * prog(basename_of(args[0]));
	std::string bundle_root;
	bool unescape_instance(false), unescape_prefix(false), alt_escape(false), etc_service(false);
	try {
		const char * bundle_root_str(0);
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", local_session_mode);
		popt::string_definition bundle_option('\0', "bundle-root", "directory", "Root directory for bundles.", bundle_root_str);
		popt::bool_definition unescape_instance_option('\0', "unescape-instance", "Unescape the instance part of a template instantiation.", unescape_instance);
		popt::bool_definition alt_escape_option('\0', "alt-escape", "Use an alternative escape algorithm.", alt_escape);
		popt::bool_definition etc_service_option('\0', "etc-service", "Consider this service to live away from the normal service bundle group.", etc_service);
		popt::definition * main_table[] = {
			&user_option,
			&bundle_option,
			&unescape_instance_option,
			&alt_escape_option,
			&etc_service_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		if (p.stopped()) throw EXIT_SUCCESS;
		if (bundle_root_str) bundle_root = bundle_root_str + std::string("/");
	} catch (const popt::error & e) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, e.arg, e.msg);
		throw static_cast<int>(EXIT_USAGE);
	}

	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing argument(s).");
		throw EXIT_FAILURE;
	}

	struct names names(args.front());

	args.erase(args.begin());
	if (!args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Unrecognized argument(s).");
		throw EXIT_FAILURE;
	}

	bool is_socket_activated(false), is_target(false), is_socket_accept(false);

	std::string socket_filename;
	profile socket_profile;
	std::string service_filename;
	profile service_profile;

	{
		std::string bundle_basename;
		if (ends_in(names.query_unit_basename(), ".target", bundle_basename)) {
			is_target = true;
		} else
		if (ends_in(names.query_unit_basename(), ".socket", bundle_basename)) {
			is_socket_activated = true;
		} else
		if (ends_in(names.query_unit_basename(), ".service", bundle_basename)) {
		} else
		{
			bundle_basename = names.query_unit_basename();
		}
		names.set_bundle_basename(bundle_basename);
		names.set_bundle_dirname(bundle_root + names.query_bundle_basename());
	}

	if (is_socket_activated) {
		FileStar socket_file(find(names.query_arg_name(), socket_filename));
		if (!socket_file) {
			const int error(errno);
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, names.query_arg_name().c_str(), std::strerror(error));
			throw EXIT_FAILURE;
		}
		if (!is_regular(prog, socket_filename, socket_file))
			throw EXIT_FAILURE;
		load(socket_profile, socket_file);
		socket_file = NULL;
	}

	{
		FileStar service_file;
		std::string service_unit_name;
		if (is_socket_activated) {
			value * accept(socket_profile.use("socket", "accept"));
			names.set_prefix(names.query_bundle_basename(), unescape_prefix, alt_escape);
			is_socket_accept = is_bool_true(accept, false);
			service_unit_name = names.query_unit_dirname() + names.query_prefix() + (is_socket_accept ? "@" : "") + ".service";
			service_file = find(service_unit_name, service_filename);
		} else {
			names.set_prefix(names.query_bundle_basename(), unescape_prefix, alt_escape);
			service_unit_name = names.query_arg_name();
			service_file = find(service_unit_name, service_filename);
			if (!service_file && ENOENT == errno) {
				std::string::size_type atc(names.query_bundle_basename().find('@'));
				if (names.query_bundle_basename().npos != atc) {
					names.set_prefix(names.query_unit_basename().substr(0, atc), unescape_prefix, alt_escape);
					++atc;
					names.set_instance(names.query_bundle_basename().substr(atc, names.query_bundle_basename().npos), unescape_instance, alt_escape);
					service_unit_name = names.query_unit_dirname() + names.query_prefix() + "@.service";
					service_file = find(service_unit_name, service_filename);
				}
			}
		}
				
		if (!service_file) {
			const int error(errno);
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, service_unit_name.c_str(), std::strerror(error));
			throw EXIT_FAILURE;
		}

		if (!is_regular(prog, service_filename, service_file))
			throw EXIT_FAILURE;

		load(service_profile, service_file);

		service_file = NULL;
	}

	value * listenstream(socket_profile.use("socket", "listenstream"));
	value * listenfifo(socket_profile.use("socket", "listenfifo"));
	value * listendatagram(socket_profile.use("socket", "listendatagram"));
#if defined(IPV6_V6ONLY)
	value * bindipv6only(socket_profile.use("socket", "bindipv6only"));
#endif
#if defined(SO_REUSEPORT)
	value * reuseport(socket_profile.use("socket", "reuseport"));
#endif
	value * backlog(socket_profile.use("socket", "backlog"));
	value * maxconnections(socket_profile.use("socket", "maxconnections"));
	value * keepalive(socket_profile.use("socket", "keepalive"));
	value * socketmode(socket_profile.use("socket", "socketmode"));
	value * socketuser(socket_profile.use("socket", "socketuser"));
	value * socketgroup(socket_profile.use("socket", "socketgroup"));
	value * passcredentials(socket_profile.use("socket", "passcredentials"));
	value * passsecurity(socket_profile.use("socket", "passsecurity"));
	value * nodelay(socket_profile.use("socket", "nodelay"));
	value * ucspirules(socket_profile.use("socket", "ucspirules"));
	value * freebind(socket_profile.use("socket", "freebind"));
	value * socket_before(socket_profile.use("unit", "before"));
	value * socket_after(socket_profile.use("unit", "after"));
	value * socket_conflicts(socket_profile.use("unit", "conflicts"));
	value * socket_wants(socket_profile.use("unit", "wants"));
	value * socket_requires(socket_profile.use("unit", "requires"));
	value * socket_requisite(socket_profile.use("unit", "requisite"));
	value * socket_description(socket_profile.use("unit", "description"));
	value * socket_defaultdependencies(socket_profile.use("unit", "defaultdependencies"));
	value * socket_earlysupervise(socket_profile.use("unit", "earlysupervise"));	// This is an extension to systemd.
	value * socket_wantedby(socket_profile.use("install", "wantedby"));
	value * socket_requiredby(socket_profile.use("install", "requiredby"));
	value * socket_stoppedby(socket_profile.use("install", "stoppedby"));	// This is an extension to systemd.

	value * type(service_profile.use("service", "type"));
	value * workingdirectory(service_profile.use("service", "workingdirectory"));
	value * rootdirectory(service_profile.use("service", "rootdirectory"));
	value * systemdworkingdirectory(service_profile.use("service", "systemdworkingdirectory"));	// This is an extension to systemd.
	value * systemduserenvironment(service_profile.use("service", "systemduserenvironment"));	// This is an extension to systemd.
	value * execstart(service_profile.use("service", "execstart"));
	value * execstartpre(service_profile.use("service", "execstartpre"));
	value * execrestartpre(service_profile.use("service", "execrestartpre"));	// This is an extension to systemd.
	value * execstoppost(service_profile.use("service", "execstoppost"));
	value * limitnofile(service_profile.use("service", "limitnofile"));
	value * limitcpu(service_profile.use("service", "limitcpu"));
	value * limitcore(service_profile.use("service", "limitcore"));
	value * limitnproc(service_profile.use("service", "limitnproc"));
	value * limitfsize(service_profile.use("service", "limitfsize"));
	value * limitas(service_profile.use("service", "limitas"));
	value * limitdata(service_profile.use("service", "limitdata"));
	value * limitmemory(service_profile.use("service", "limitmemory"));	// This is an extension to systemd.
	value * killmode(service_profile.use("service", "killmode"));
	value * rootdirectorystartonly(service_profile.use("service", "rootdirectorystartonly"));
	value * permissionsstartonly(service_profile.use("service", "permissionsstartonly"));
	value * standardinput(service_profile.use("service", "standardinput"));
	value * standardoutput(service_profile.use("service", "standardoutput"));
	value * standarderror(service_profile.use("service", "standarderror"));
	value * user(service_profile.use("service", "user"));
	value * group(service_profile.use("service", "group"));
	value * umask(service_profile.use("service", "umask"));
	value * environment(service_profile.use("service", "environment"));
	value * environmentfile(service_profile.use("service", "environmentfile"));
	value * environmentdirectory(service_profile.use("service", "environmentdirectory"));	// This is an extension to systemd.
	value * environmentuser(service_profile.use("service", "environmentuser"));	// This is an extension to systemd.
	value * environmentappendpath(service_profile.use("service", "environmentappendpath"));	// This is an extension to systemd.
#if defined(__LINUX__) || defined(__linux__)
	value * utmpidentifier(service_profile.use("service", "utmpidentifier"));
#endif
	value * ttypath(service_profile.use("service", "ttypath"));
	value * ttyreset(service_profile.use("service", "ttyreset"));
	value * ttyprompt(service_profile.use("service", "ttyprompt"));	// This is an extension to systemd.
	value * ttyvhangup(service_profile.use("service", "ttyvhangup"));
//	value * ttyvtdisallocate(service_profile.use("service", "ttyvtdisallocate"));
	value * remainafterexit(service_profile.use("service", "remainafterexit"));
	value * processgroupleader(service_profile.use("service", "processgroupleader"));	// This is an extension to systemd.
	value * sessionleader(service_profile.use("service", "sessionleader"));	// This is an extension to systemd.
	value * restart(service_profile.use("service", "restart"));
	value * restartsec(service_profile.use("service", "restartsec"));
//	value * ignoresigpipe(service_profile.use("service", "ignoresigpipe"));
#if defined(__LINUX__) || defined(__linux__)
	value * privatetmp(service_profile.use("service", "privatetmp"));
	value * privatedevices(service_profile.use("service", "privatedevices"));
	value * privatenetwork(service_profile.use("service", "privatenetwork"));
	value * mountflags(service_profile.use("service", "mountflags"));
#endif
	value * service_defaultdependencies(service_profile.use("unit", "defaultdependencies"));
	value * service_earlysupervise(service_profile.use("unit", "earlysupervise"));	// This is an extension to systemd.
	value * service_after(service_profile.use("unit", "after"));
	value * service_before(service_profile.use("unit", "before"));
	value * service_conflicts(service_profile.use("unit", "conflicts"));
	value * service_wants(service_profile.use("unit", "wants"));
	value * service_requires(service_profile.use("unit", "requires"));
	value * service_requisite(service_profile.use("unit", "requisite"));
	value * service_description(service_profile.use("unit", "description"));
	value * service_wantedby(service_profile.use("install", "wantedby"));
	value * service_requiredby(service_profile.use("install", "requiredby"));
	value * service_stoppedby(service_profile.use("install", "stoppedby"));	// This is an extension to systemd.

	// Actively prevent certain unsupported combinations.

	if (type && "simple" != tolower(type->last_setting()) && "forking" != tolower(type->last_setting()) && "oneshot" != tolower(type->last_setting())) {
		std::fprintf(stderr, "%s: FATAL: %s: %s: %s\n", prog, service_filename.c_str(), type->last_setting().c_str(), "Only simple services are supported.");
		throw EXIT_FAILURE;
	}
	const bool is_oneshot(type && "oneshot" == tolower(type->last_setting()));
	if (!execstart && !is_target && !is_oneshot) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, service_filename.c_str(), "Missing mandatory ExecStart entry.");
		throw EXIT_FAILURE;
	}
	if (is_socket_activated) {
		if (!listenstream && !listendatagram && !listenfifo) {
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, socket_filename.c_str(), "Missing mandatory ListenStream/ListenDatagram/ListenFIFO entry.");
			throw EXIT_FAILURE;
		}
		if (listendatagram) {
			if (is_socket_accept) {
				std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, socket_filename.c_str(), "ListenDatagram sockets may not have Accept=yes.");
				throw EXIT_FAILURE;
			}
		}
		if (listenfifo) {
			if (is_socket_accept) {
				std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, socket_filename.c_str(), "ListenFIFO sockets may not have Accept=yes.");
				throw EXIT_FAILURE;
			}
		}
	}
	// We silently set "process" as the default killmode, not "control-group".
	if (killmode && "process" != tolower(killmode->last_setting())) {
		std::fprintf(stderr, "%s: FATAL: %s: %s: %s\n", prog, service_filename.c_str(), killmode->last_setting().c_str(), "Unsupported service stop mechanism.");
		throw EXIT_FAILURE;
	}

	// Make the directories.

	const std::string service_dirname(names.query_bundle_dirname() + "/service");

	mkdirat(AT_FDCWD, names.query_bundle_dirname().c_str(), 0755);
	const FileDescriptorOwner bundle_dir_fd(open_dir_at(AT_FDCWD, (names.query_bundle_dirname() + "/").c_str()));
	if (0 > bundle_dir_fd.get()) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, names.query_bundle_dirname().c_str(), std::strerror(error));
		throw EXIT_FAILURE;
	}
	make_service(bundle_dir_fd.get());
	make_orderings_and_relations(bundle_dir_fd.get());
	const FileDescriptorOwner service_dir_fd(open_service_dir(bundle_dir_fd.get()));
	if (0 > service_dir_fd.get()) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s/%s: %s\n", prog, names.query_bundle_dirname().c_str(), "service", std::strerror(error));
		throw EXIT_FAILURE;
	}

#define CREATE_LINKS(l,s) (l ? create_links (prog,is_target,etc_service,bundle_dir_fd.get(),names.substitute((l)->last_setting()),(s)) : static_cast<void>(0))

	// Construct various common command strings.

	std::string chroot;
	if (rootdirectory) chroot += "chroot " + quote(names.substitute(rootdirectory->last_setting())) + "\n";
#if defined(__LINUX__) || defined(__linux__)
	const bool is_private_tmp(is_bool_true(privatetmp, false));
	const bool is_private_network(is_bool_true(privatenetwork, false));
	const bool is_private_devices(is_bool_true(privatedevices, false));
	if (is_private_tmp||is_private_network||is_private_devices) {
		chroot += "unshare ";
		if (is_private_tmp||is_private_devices) chroot += "--mount ";
		if (is_private_network) chroot += "--network ";
		chroot += "\n";
		if (is_private_tmp||is_private_devices) {
			chroot += "set-mount-object --recursive slave /\n";
			chroot += "make-private-fs ";
			if (is_private_tmp) chroot += "--temp ";
			if (is_private_devices) chroot += "--devices ";
			chroot += "\n";
		}
	}
	if (mountflags) 
		chroot += "set-mount-object --recursive " + quote(mountflags->last_setting()) + " /\n";
	else if (is_private_tmp||is_private_devices)
		chroot += "set-mount-object --recursive shared /\n";
#endif
	const bool chrootall(!is_bool_true(rootdirectorystartonly, false));
	std::string setsid;
	if (is_bool_true(sessionleader, false)) setsid += "setsid\n";
	if (is_bool_true(processgroupleader, false)) setsid += "setpgrp\n";
	std::string envuidgid, setuidgid;
	if (user) {
		const std::string u(names.substitute(user->last_setting()));
		if (rootdirectory) {
			envuidgid += "envuidgid " + quote(u) + "\n";
			setuidgid += "setuidgid-fromenv\n";
		} else
			setuidgid += "setuidgid " + quote(u) + "\n";
		if (is_bool_true(systemduserenvironment, true))
			// This replicates systemd useless features.
			setuidgid += "userenv\n";
	} else {
		if (group)
			setuidgid += "setgid " + quote(names.substitute(group->last_setting())) + "\n";
	}
	const bool setuidgidall(!is_bool_true(permissionsstartonly, false));
	// systemd always runs services in / by default; daemontools runs them in the service directory.
	std::string chdir;
	if (workingdirectory)
		chdir += "chdir " + quote(names.substitute(workingdirectory->last_setting())) + "\n";
	else if (rootdirectory || is_bool_true(systemdworkingdirectory, true))
		chdir += "chdir /\n";
	std::string softlimit;
	if (limitnofile||limitcpu||limitcore||limitnproc||limitfsize||limitas||limitdata||limitmemory) {
		softlimit += "softlimit";
		if (limitnofile) softlimit += " -o " + quote(limitnofile->last_setting());
		if (limitcpu) softlimit += " -t " + quote(limitcpu->last_setting());
		if (limitcore) softlimit += " -c " + quote(limitcore->last_setting());
		if (limitnproc) softlimit += " -p " + quote(limitnproc->last_setting());
		if (limitfsize) softlimit += " -f " + quote(limitfsize->last_setting());
		if (limitas) softlimit += " -a " + quote(limitas->last_setting());
		if (limitdata) softlimit += " -d " + quote(limitdata->last_setting());
		if (limitmemory) softlimit += " -m " + quote(limitmemory->last_setting());
		softlimit += "\n";
	}
	std::string env;
	if (environmentfile) {
		for (std::list<std::string>::const_iterator i(environmentfile->all_settings().begin()); environmentfile->all_settings().end() != i; ++i) {
			std::string val;
			const bool minus(strip_leading_minus(*i, val));
			env += "read-conf " + ((minus ? "--oknofile " : "") + quote(names.substitute(val))) + "\n";
		}
	}
	if (environmentdirectory) {
		for (std::list<std::string>::const_iterator i(environmentdirectory->all_settings().begin()); environmentdirectory->all_settings().end() != i; ++i) {
			const std::string & val(*i);
			env += "envdir " + quote(names.substitute(val)) + "\n";
		}
	}
	if (environmentuser) env += "envuidgid " + quote(names.substitute(environmentuser->last_setting())) + "\n";
	if (environment) {
		for (std::list<std::string>::const_iterator i(environment->all_settings().begin()); environment->all_settings().end() != i; ++i) {
			const std::string & datum(*i);
			const std::list<std::string> list(names.substitute(split_list(datum)));
			for (std::list<std::string>::const_iterator j(list.begin()); list.end() != j; ++j) {
				const std::string & s(*j);
				const std::string::size_type eq(s.find('='));
				const std::string var(s.substr(0, eq));
				const std::string val(eq == std::string::npos ? std::string() : s.substr(eq + 1, std::string::npos));
				env += "setenv " + quote(var) + " " + quote(val) + "\n";
			}
		}
	}
	if (environmentappendpath) {
		for (std::list<std::string>::const_iterator i(environmentappendpath->all_settings().begin()); environmentappendpath->all_settings().end() != i; ++i) {
			const std::string & datum(*i);
			const std::list<std::string> list(names.substitute(split_list(datum)));
			for (std::list<std::string>::const_iterator j(list.begin()); list.end() != j; ++j) {
				const std::string & s(*j);
				const std::string::size_type eq(s.find('='));
				const std::string var(s.substr(0, eq));
				const std::string val(eq == std::string::npos ? std::string() : s.substr(eq + 1, std::string::npos));
				env += "appendpath " + quote(var) + " " + quote(val) + "\n";
			}
		}
	}
	std::string um;
	if (umask) um += "umask " + quote(umask->last_setting()) + "\n";
	const bool is_remain(is_bool_true(remainafterexit, false));
	const bool stdin_socket(standardinput && "socket" == tolower(standardinput->last_setting()));
	const bool stdin_tty(standardinput && ("tty" == tolower(standardinput->last_setting()) || "tty-force" == tolower(standardinput->last_setting())));
	const bool stdout_inherit(standardoutput && "inherit" == tolower(standardoutput->last_setting()));
	const bool stderr_inherit(standarderror && "inherit" == tolower(standarderror->last_setting()));
	// We "un-use" anything that isn't "inherit"/"socket".
	if (standardinput && !stdin_socket && !stdin_tty) standardinput->used = false;
	if (standardoutput && !stdout_inherit) standardoutput->used = false;
	if (standarderror && !stderr_inherit) standarderror->used = false;
	std::string tty(ttypath ? names.substitute(ttypath->last_setting()) : "/dev/console");
	std::string redirect, login_prompt;
	if (ttypath || stdin_tty) redirect += "vc-get-tty " + quote(tty) + "\n";
	if (stdin_tty) {
		redirect += "open-controlling-tty";
		if (is_bool_true(ttyvhangup, false)) 
#if defined(__LINUX__) || defined(__linux__)
			redirect += " --vhangup";
#else
			redirect += " --revoke";
#endif
		redirect += "\n";
	}
	if (stdout_inherit && !stdin_tty) redirect += "fdmove -c 1 0\n";
	if (stderr_inherit && !stdin_tty) redirect += "fdmove -c 2 1\n";
	if (stdin_tty) {
		if (is_bool_true(ttyreset, false)) login_prompt += "vc-reset-tty\n";
		if (is_bool_true(ttyprompt, false)) login_prompt += "login-prompt\n";
#if defined(__LINUX__) || defined(__linux__)
		if (utmpidentifier)
			login_prompt += "login-process --id " + quote(names.substitute(utmpidentifier->last_setting())) + "\n";
#endif
	}

	// Open the service script files.

	std::ofstream start, stop, restart_script, real_run, real_service, remain;
	open(prog, start, service_dirname + "/start");
	open(prog, stop, service_dirname + "/stop");
	open(prog, restart_script, service_dirname + "/restart");
	open(prog, real_run, service_dirname + "/run");
	if (is_socket_activated)
		open(prog, real_service, service_dirname + "/service");
	std::ostream & run(is_oneshot ? start : real_run);
	std::ostream & service(is_socket_activated ? real_service : run);
	if (is_remain)
		open(prog, remain, service_dirname + "/remain");

	// Write the script files.

	if (!is_oneshot) {
		start << "#!/bin/nosh\n" << multi_line_comment("Start file generated from " + service_filename);
		if (execstartpre) {
			if (setuidgidall) start << envuidgid;
			start << redirect;
			start << softlimit;
			if (chrootall) start << chroot;
			start << chdir;
			if (setuidgidall) start << setuidgid;
			start << env;
			start << um;
			for (std::list<std::string>::const_iterator i(execstartpre->all_settings().begin()); execstartpre->all_settings().end() != i; ) {
				std::list<std::string>::const_iterator j(i++);
				if (execstartpre->all_settings().begin() != j) start << " ;\n"; 
				if (execstartpre->all_settings().end() != i) start << "foreground "; 
				const std::string & val(*j);
				start << names.substitute(shell_expand(strip_leading_minus(val)));
			}
			start << "\n";
		} else 
			start << "true\n";
	} else {
		real_run << "#!/bin/nosh\n" << multi_line_comment("Run file generated from " + service_filename);
		if (!is_remain)
			real_run << "pause\n";
		else
			real_run << "true\n";
	}

	if (is_socket_activated) {
		run << "#!/bin/nosh\n" << multi_line_comment("Run file generated from " + socket_filename);
		if (socket_description) {
			for (std::list<std::string>::const_iterator i(socket_description->all_settings().begin()); socket_description->all_settings().end() != i; ++i) {
				const std::string & val(*i);
				run << multi_line_comment(names.substitute(val));
			}
		}
		if (listenstream) {
			if (is_local_socket_name(listenstream->last_setting())) {
				run << "local-stream-socket-listen --systemd-compatibility ";
				if (backlog) run << "--backlog " << quote(backlog->last_setting()) << " ";
				if (socketmode) run << "--mode " << quote(socketmode->last_setting()) << " ";
				if (socketuser) run << "--uid " << quote(socketuser->last_setting()) << " ";
				if (socketgroup) run << "--gid " << quote(socketgroup->last_setting()) << " ";
				if (passcredentials) run << "--pass-credentials ";
				if (passsecurity) run << "--pass-security ";
				run << quote(listenstream->last_setting()) << "\n";
				run << envuidgid;
				run << setsid;
				run << redirect;
				run << softlimit;
				run << chroot;
				run << chdir;
				run << setuidgid;
				run << env;
				run << um;
				if (is_socket_accept) {
					run << "local-stream-socket-accept ";
					if (maxconnections) run << "--connection-limit " << quote(maxconnections->last_setting()) << " ";
					run << "\n";
				}
			} else {
				std::string listenaddress, listenport;
				split_ip_socket_name(listenstream->last_setting(), listenaddress, listenport);
				run << "tcp-socket-listen --systemd-compatibility ";
				if (backlog) run << "--backlog " << quote(backlog->last_setting()) << " ";
#if defined(IPV6_V6ONLY)
				if (bindipv6only && "both" == tolower(bindipv6only->last_setting())) run << "--combine4and6 ";
#endif
#if defined(SO_REUSEPORT)
				if (is_bool_true(reuseport, false)) run << "--reuse-port ";
#endif
				if (is_bool_true(freebind, false)) run << "--bind-to-any ";
				run << quote(listenaddress) << " " << quote(listenport) << "\n";
				run << envuidgid;
				run << setsid;
				run << redirect;
				run << softlimit;
				run << chroot;
				run << chdir;
				run << setuidgid;
				run << env;
				run << um;
				if (is_socket_accept) {
					run << "tcp-socket-accept ";
					if (maxconnections) run << "--connection-limit " << quote(maxconnections->last_setting()) << " ";
					if (is_bool_true(keepalive, false)) run << "--keepalives ";
					if (!is_bool_true(nodelay, true)) run << "--no-delay ";
					run << "\n";
				}
			}
		}
		if (listendatagram) {
			if (is_local_socket_name(listendatagram->last_setting())) {
				run << "local-datagram-socket-listen --systemd-compatibility ";
				if (backlog) run << "--backlog " << quote(backlog->last_setting()) << " ";
				if (socketmode) run << "--mode " << quote(socketmode->last_setting()) << " ";
				if (socketuser) run << "--uid " << quote(socketuser->last_setting()) << " ";
				if (socketgroup) run << "--gid " << quote(socketgroup->last_setting()) << " ";
				if (passcredentials) run << "--pass-credentials ";
				if (passsecurity) run << "--pass-security ";
				run << quote(listendatagram->last_setting()) << "\n";
				run << envuidgid;
				run << setsid;
				run << redirect;
				run << softlimit;
				run << chroot;
				run << chdir;
				run << setuidgid;
				run << env;
				run << um;
			} else {
				std::string listenaddress, listenport;
				split_ip_socket_name(listendatagram->last_setting(), listenaddress, listenport);
				run << "udp-socket-listen --systemd-compatibility ";
#if defined(IPV6_V6ONLY)
				if (bindipv6only && "both" == tolower(bindipv6only->last_setting())) run << "--combine4and6 ";
#endif
#if defined(SO_REUSEPORT)
				if (is_bool_true(reuseport, false)) run << "--reuse-port ";
#endif
				run << quote(listenaddress) << " " << quote(listenport) << "\n";
				run << envuidgid;
				run << setsid;
				run << redirect;
				run << softlimit;
				run << chroot;
				run << chdir;
				run << setuidgid;
				run << env;
				run << um;
			}
		}
		if (listenfifo) {
			run << "fifo-listen --systemd-compatibility ";
			if (backlog) run << "--backlog " << quote(backlog->last_setting()) << " ";
			if (socketmode) run << "--mode " << quote(socketmode->last_setting()) << " ";
			if (socketuser) run << "--uid " << quote(socketuser->last_setting()) << " ";
			if (socketgroup) run << "--gid " << quote(socketgroup->last_setting()) << " ";
			if (passcredentials) run << "--pass-credentials ";
			if (passsecurity) run << "--pass-security ";
			run << quote(listenfifo->last_setting()) << "\n";
			run << envuidgid;
			run << setsid;
			run << redirect;
			run << softlimit;
			run << chroot;
			run << chdir;
			run << setuidgid;
			run << env;
			run << um;
		}
		if (is_bool_true(ucspirules, false)) run << "ucspi-socket-rules-check\n";
		run << "./service\n";
		service << "#!/bin/nosh\n" << multi_line_comment("Service file generated from " + service_filename);
	} else
		run << "#!/bin/nosh\n" << multi_line_comment("Run file generated from " + service_filename);
	if (service_description) {
		for (std::list<std::string>::const_iterator i(service_description->all_settings().begin()); service_description->all_settings().end() != i; ++i) {
			const std::string & val(*i);
			service << multi_line_comment(names.substitute(val));
		}
	}
	if (is_socket_activated) {
		if (!is_socket_accept) {
			if (stdin_socket) service << "fdmove -c 0 3\n";
		}
	} else {
		service << envuidgid;
		service << setsid;
		service << redirect;
		service << softlimit;
		service << chroot;
		service << chdir;
		service << setuidgid;
		service << env;
		service << um;
		service << login_prompt;
	}
	if (is_oneshot) {
		if (execstartpre) {
			for (std::list<std::string>::const_iterator i(execstartpre->all_settings().begin()); execstartpre->all_settings().end() != i; ++i ) {
				service << "foreground "; 
				service << names.substitute(shell_expand(strip_leading_minus(*i)));
				service << " ;\n"; 
			}
		}
	}
	service << (execstart ? names.substitute(shell_expand(strip_leading_minus(execstart->last_setting()))) : is_remain ? "true" : "pause") << "\n";

	// nosh is not suitable here, since the restart script is passed arguments.
	restart_script << "#!/bin/sh\n" << multi_line_comment("Restart file generated from " + service_filename);
	if (restartsec) restart_script << "sleep " << restartsec->last_setting() << "\n";
	if (execrestartpre) {
		std::stringstream s;
		if (setuidgidall) s << envuidgid;
		s << redirect;
		s << softlimit;
		if (chrootall) s << chroot;
		s << chdir;
		if (setuidgidall) s << setuidgid;
		s << env;
		s << um;
		for (std::list<std::string>::const_iterator i(execstartpre->all_settings().begin()); execstartpre->all_settings().end() != i; ) {
			std::list<std::string>::const_iterator j(i++);
			if (execstartpre->all_settings().begin() != j) s << " ;\n"; 
			if (execstartpre->all_settings().end() != i) s << "foreground "; 
			const std::string & val(*j);
			s << names.substitute(shell_expand(strip_leading_minus(val)));
		}
		restart_script << escape_newlines(s.str()) << "\n";
	}
	if (restart && "always" == tolower(restart->last_setting())) {
		restart_script << "exec true\t# ignore script arguments\n";
	} else
	if ((!restart || "no" == tolower(restart->last_setting()))) {
		restart_script << "exec false\t# ignore script arguments\n";
	} else 
	{
		const bool 
			on_true (restart &&  ("on-success" == tolower(restart->last_setting()))),
			on_false(restart &&  ("on-failure" == tolower(restart->last_setting()))),
			on_term (restart && (("on-failure" == tolower(restart->last_setting())) || ("on-abort" == tolower(restart->last_setting())))), 
			on_abort(restart && (("on-failure" == tolower(restart->last_setting())) || ("on-abort" == tolower(restart->last_setting())) || ("on-abnormal" == tolower(restart->last_setting())))), 
			on_crash(restart && (("on-failure" == tolower(restart->last_setting())) || ("on-abort" == tolower(restart->last_setting())) || ("on-abnormal" == tolower(restart->last_setting())))), 
			on_kill (restart && (("on-failure" == tolower(restart->last_setting())) || ("on-abort" == tolower(restart->last_setting())) || ("on-abnormal" == tolower(restart->last_setting())))); 
		restart_script << 
			"case \"$1\" in\n"
			"\te*)\n"
			"\t\tif [ \"$2\" -ne 0 ]\n"
			"\t\tthen\n"
			"\t\t\texec " << (on_false ? "true" : "false") << "\n"
			"\t\telse\n"
			"\t\t\texec " << (on_true  ? "true" : "false") << "\n"
			"\t\tfi\n"
			"\t\t;;\n"
			"\tt*)\n"
			"\t\texec " << (on_term  ? "true" : "false") << "\n"
			"\t\t;;\n"
			"\tk*)\n"
			"\t\texec " << (on_kill  ? "true" : "false") << "\n"
			"\t\t;;\n"
			"\ta*)\n"
			"\t\texec " << (on_abort ? "true" : "false") << "\n"
			"\t\t;;\n"
			"\tc*|*)\n"
			"\t\texec " << (on_crash ? "true" : "false") << "\n"
			"\t\t;;\n"
			"esac\n"
			"exec false\n";
	}

	stop << "#!/bin/nosh\n" << multi_line_comment("Stop file generated from " + service_filename);
	if (execstoppost) {
		if (setuidgidall) stop << envuidgid;
		stop << softlimit;
		if (chrootall) stop << chroot;
		stop << chdir;
		if (setuidgidall) stop << setuidgid;
		stop << env;
		stop << um;
		for (std::list<std::string>::const_iterator i(execstoppost->all_settings().begin()); execstoppost->all_settings().end() != i; ) {
			std::list<std::string>::const_iterator j(i++);
			if (execstoppost->all_settings().begin() != j) stop << " ;\n"; 
			if (execstoppost->all_settings().end() != i) stop << "foreground "; 
			const std::string & val(*j);
			stop << names.substitute(strip_leading_minus(val));
		}
		stop << "\n";
	} else 
		stop << "true\n";

	// Set the dependency and installation information.

	CREATE_LINKS(socket_after, "after/");
	CREATE_LINKS(service_after, "after/");
	CREATE_LINKS(socket_before, "before/");
	CREATE_LINKS(service_before, "before/");
	CREATE_LINKS(socket_wants, "wants/");
	CREATE_LINKS(service_wants, "wants/");
	CREATE_LINKS(socket_requires, "wants/");
	CREATE_LINKS(service_requires, "wants/");
	CREATE_LINKS(socket_requisite, "wants/");
	CREATE_LINKS(service_requisite, "wants/");
	CREATE_LINKS(socket_conflicts, "conflicts/");
	CREATE_LINKS(service_conflicts, "conflicts/");
	CREATE_LINKS(socket_wantedby, "wanted-by/");
	CREATE_LINKS(service_wantedby, "wanted-by/");
	CREATE_LINKS(socket_requiredby, "wanted-by/");
	CREATE_LINKS(service_requiredby, "wanted-by/");
	CREATE_LINKS(socket_stoppedby, "stopped-by/");
	CREATE_LINKS(service_stoppedby, "stopped-by/");
	const bool defaultdependencies(
			(is_socket_activated && is_bool_true(socket_defaultdependencies, true)) || 
			is_bool_true(service_defaultdependencies, true)
	);
	const bool earlysupervise(
			(is_socket_activated && is_bool_true(socket_earlysupervise, false)) || 
			is_bool_true(service_earlysupervise, false)
	);
	if (defaultdependencies) {
		if (is_socket_activated)
			create_links(prog, is_target, etc_service, bundle_dir_fd.get(), "sockets.target", "wanted-by/");
		create_links(prog, is_target, etc_service, bundle_dir_fd.get(), "basic.target", "after/");
		create_links(prog, is_target, etc_service, bundle_dir_fd.get(), "basic.target", "wants/");
		create_links(prog, is_target, etc_service, bundle_dir_fd.get(), "shutdown.target", "before/");
		create_links(prog, is_target, etc_service, bundle_dir_fd.get(), "shutdown.target", "stopped-by/");
	}
	if (earlysupervise) {
		create_link(prog, bundle_dir_fd.get(), "/run/system-manager/early-supervise/" + names.query_bundle_basename(), "supervise");
	}

	// Issue the final reports.

	report_unused(prog, socket_profile, socket_filename);
	report_unused(prog, service_profile, service_filename);

	throw EXIT_SUCCESS;
}
