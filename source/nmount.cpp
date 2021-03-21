/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include "nmount.h"

#if defined(__LINUX__) || defined(__linux__)

#include <unistd.h>
#include <sys/uio.h>
#include <sys/mount.h>
#include "utils.h"
#include <string>

extern "C"
int
nmount (
	struct iovec * iov,
	unsigned int ioc,
	int flags
) {
	std::string path, type, from, data;
	// NULL data is different from empty data in Linux mount() for at least the cgroup2 filesystem.
	bool hasdata(false);

	for (unsigned int u(0U); u + 1U < ioc; u += 2U) {
		const std::string var(convert(iov[u]));
		if ("from" == var)
			from = convert(iov[u + 1U]);
		else
		if ("fstype" == var)
			type = convert(iov[u + 1U]);
		else
		if ("fspath" == var)
			path = convert(iov[u + 1U]);
		else 
		{
			hasdata = true;
			if (!data.empty())
				data += ",";
			data += var;
			if (iov[u + 1U].iov_base && iov[u + 1U].iov_len)
				data += "=" + convert(iov[u + 1U]);
		}
	}

	return mount(from.c_str(), path.c_str(), type.c_str(), static_cast<unsigned long>(flags), hasdata ? data.c_str() : 0);
}

#elif defined(__OpenBSD__) || defined(__NetBSD__)

#include <cerrno>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/mount.h>
#include "utils.h"
#include "environ.h"
#include "ProcessEnvironment.h"
#include <string>
#include <vector>
#include <cerrno>
#include <cstring>

struct mount_flag {
	const unsigned int flag;
	const char * argument;
};

static const struct mount_flag mount_flagtable[] = {
	{	MNT_RDONLY,		"ro"		},
	{	MNT_NOATIME,		"noatime"	},
	{	MNT_NOEXEC,		"noexec"	},
	{	MNT_NOSUID,		"nosuid"	},
	{	MNT_NODEV,		"nodev"		},
	{	MNT_SYNCHRONOUS,	"sync"		},
	{	MNT_ASYNC,		"async"		},
	{	MNT_UPDATE,		"update"	},
	{	MNT_SOFTDEP,		"softdep"	},
	{	MNT_FORCE,		"force"		},
#if defined(__OpenBSD__)
	{	MNT_WXALLOWED,		"wxallowed"	},
	{	MNT_NOPERM,		"noperm"	},
#elif defined(__NetBSD__)
	{	MNT_UNION,		"union"		},
	{	MNT_IGNORE,		"hidden"	},
	{	MNT_NOCOREDUMP,		"nocoredump"	},
	{	MNT_RELATIME,		"relatime"	},
	{	MNT_NODEVMTIME,		"nodevmtime"	},
	{	MNT_SYMPERM,		"symperm"	},
	{	MNT_LOG,		"log"		},
	{	MNT_EXTATTR,		"extattr"	},
	{	MNT_DISCARD,		"discard"	},
	{	MNT_RELOAD,		"reload"	},
#endif
};

// Iovecs passed here are converted to arguments passed to an external mount process

extern "C"
int
nmount (
	struct iovec * iov,
	unsigned int ioc,
	int flags
) {
	std::string path, type, from, data;
	std::vector<const char *> args;
	ProcessEnvironment envs(environ);

	for (unsigned int u(0U); u + 1U < ioc; u += 2U) {
		const std::string var(convert(iov[u]));
		if ("from" == var)
			from = convert(iov[u + 1U]);
		else
		if ("fstype" == var)
			type = convert(iov[u + 1U]);
		else
		if ("fspath" == var)
			path = convert(iov[u + 1U]);
		else 
		{
			if (!data.empty())
				data += ",";
			data += "-"; // Indicate a filesystem-specific option
			data += var;
			if (iov[u + 1U].iov_base && iov[u + 1U].iov_len)
				data += "=" + convert(iov[u + 1U]);
		}
	}

	for (unsigned int i(0); i < (sizeof mount_flagtable / sizeof *mount_flagtable); ++i) {
		if (flags & mount_flagtable[i].flag) {
			if (!data.empty())
				data += ",";
			data += mount_flagtable[i].argument;
		}
	}

	if (!data.empty()) {
		args.push_back("-o");
		args.push_back(data.c_str());
	}
	if (!type.empty()) {
		args.push_back("-t");
		args.push_back(type.c_str());
	}
	if (!from.empty()) {
		args.push_back(from.c_str());
	}
	if (!path.empty()) {
		args.push_back(path.c_str());
	}

	const pid_t pid(fork());
	if (0 > pid) {
		return 1;
	} else if (0 < pid) {
		int status, code;
		if (0 <= wait_blocking_for_exit_of(pid, status, code))
			return 1;
		return code;
	}

	// Child process only from now on.

	safe_execvp("mount", args.data(), envs);

	const int error(errno);
	std::fprintf(stderr, "ERROR: %s\n", std::strerror(error));
	std::fflush(stderr);
	_exit(1);
}

#endif
