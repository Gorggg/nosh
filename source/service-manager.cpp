/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <csignal>
#include <cerrno>
#include <ctime>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/types.h>
#if !defined(__LINUX__) && !defined(__linux__)
#include <sys/event.h>
#else
#include <sys/poll.h>
#endif
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "fdutils.h"
#include "listen.h"
#include "service-manager.h"
#include "environ.h"
#include "FileDescriptorOwner.h"

static const char * prog(0);
#if !defined(__LINUX__) && !defined(__linux__)
static int queue(-1);
#else
static std::vector<pollfd> poll_table;
#endif

/* Service objects and maps *************************************************
// **************************************************************************
*/

namespace {

struct index : public std::pair<dev_t, ino_t> {
	index(const struct stat & s) : pair(s.st_dev, s.st_ino) {}
};

struct service : public index {
	service(const struct stat &);
	~service();

	void add_process(int);
	void stamp_time(const timespec &);
	void stamp_activity();
	void stamp_pending_command();
	void stamp_process_status(const unsigned int, int, const timespec &);
	void write_status();
	void reap (const sigset_t &, int, int);
	void enact_control_message(const sigset_t &);
	void enact_control_message(const sigset_t &, char);
	void add_to_input_activation_list();
	void delete_from_input_activation_list();
	void add_to_control_fifo_list();
	void delete_from_control_fifo_list();
	void set_unload() { unload_after_stop = true; }
	bool unloadable() const { return unload_after_stop && (NONE == activity) && !has_processes(); }

	int in, out, err, pipe_fds[2], lock_fd, ok_fd, control_fd, control_client_fd, status_fd, service_dir_fd;
	char name[NAME_MAX + 1];
	bool run_on_empty;
protected:
	char pending_command;
	bool paused, unload_after_stop;
	enum ActivityType {
		NONE = 0,	///< The service is not running anything.
		START = 'a',	///< The service is running the "start" program.
		RUN = 'r',	///< The service is running the "run" program.
		RESTART = 'f',	///< The service is running the "restart" program.
		STOP = 'o'	///< The service is running the "stop" program.
	} activity;
	int current_process_status;
	unsigned char status[STATUS_BLOCK_SIZE];
	std::set<int> processes;

	void change_state_if_necessary (const sigset_t &);
	void enter_state(const sigset_t &);
	void del_process(int, int);
	bool has_processes() const { return !processes.empty(); }
	void killall(int);
	void killtop(int);
	void add_input_ready_event(int);
	void delete_input_ready_event(int);
};

}

typedef std::map<int, service *> active_service_map;
static active_service_map active_services;

typedef std::map<int, service *> input_activated_service_map;
static input_activated_service_map input_activated_services;

typedef std::map<int, service *> service_control_fifo_map;
static service_control_fifo_map service_control_fifos;

// \bug FIXME: This should be a map of shared pointers.
typedef std::map<struct index, service> service_map;
static service_map services;

/* Supervision **************************************************************
// **************************************************************************
*/

static inline
void
pack_bigendian (
	unsigned char * buf,
	uint64_t v,
	std::size_t len
) {
	while (len) {
		--len;
		buf[len] = v & 0xFF;
		v >>= 8;
	}
}

static inline
void
pack_littleendian (
	unsigned char * buf,
	uint64_t v,
	std::size_t len
) {
	while (len) {
		--len;
		*buf++ = v & 0xFF;
		v >>= 8;
	}
}

static inline
bool 
is_failure (
	int wait_status	///< a wait status from which WIFCONTINED and WIFSTOPPED have already been excluded
) {
	if (WIFSIGNALED(wait_status)) 
		// Any signal at all is a failure.
		return true;
	else
		return WEXITSTATUS(wait_status) != EXIT_SUCCESS;
}

service::service(const struct stat & s) : 
	index(s),
	in(STDIN_FILENO), 
	out(STDOUT_FILENO), 
	err(STDERR_FILENO), 
	lock_fd(-1),
	ok_fd(-1),
	control_fd(-1),
	control_client_fd(-1),
	status_fd(-1),
	service_dir_fd(-1),
	run_on_empty(false),
	pending_command('\0'), 
	paused(false), 
	unload_after_stop(false),
	activity(NONE), 
	current_process_status(-1),
	processes()
{
	pipe_fds[0] = pipe_fds[1] = -1;
	name[0] = '\0';
}

service::~service() 
{
	delete_from_input_activation_list();
	delete_from_control_fifo_list();
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	close(status_fd);
	close(control_client_fd);
	close(control_fd);
	close(ok_fd);
	close(lock_fd);
	close(service_dir_fd);
	pipe_fds[0] = pipe_fds[1] = service_dir_fd = status_fd = control_client_fd = control_fd = ok_fd = lock_fd = -1;
}

void 
service::stamp_time (
	const timespec & now
) {
	const uint64_t s(time_to_tai64(now.tv_sec, false));
	const uint32_t n(now.tv_nsec);
	const uint32_t p(has_processes() ? *processes.begin() : 0);
	pack_bigendian(status +  0, s, 8);
	pack_bigendian(status +  8, n, 4);
	pack_littleendian(status + THIS_PID_OFFSET, p, 4);
}

void 
service::stamp_activity () 
{
	status[PAUSE_FLAG_OFFSET] = has_processes() && paused;
	switch (activity) {
		case NONE:	status[ENCORE_STATUS_OFFSET] = encore_status_stopped; break;
		case START:	status[ENCORE_STATUS_OFFSET] = encore_status_starting; break;
		case RUN:	status[ENCORE_STATUS_OFFSET] = encore_status_running; break;
		case RESTART:	status[ENCORE_STATUS_OFFSET] = encore_status_failed; break;
		case STOP:	status[ENCORE_STATUS_OFFSET] = encore_status_stopping; break;
	}
}

void 
service::stamp_pending_command () 
{
	status[WANT_FLAG_OFFSET] = pending_command;
}

inline
void 
service::stamp_process_status (
	const unsigned index,
	int wait_status,
	const timespec & now
) {
	const std::size_t offset(EXIT_STATUSES_OFFSET + EXIT_STATUS_SIZE * index);
	if (-1 == wait_status) {
		status[offset] = 0;
		pack_bigendian(status + offset + 1U, 0U, 4);
	} else
	if (WIFEXITED(wait_status)) {
		const int code(WEXITSTATUS(wait_status));
		status[offset] = 1;
		pack_bigendian(status + offset + 1U, code, 4);
	} else
	{
		const int signo(WTERMSIG(wait_status));
		status[offset] = WCOREDUMP(wait_status) ? 3 : 2;
		pack_bigendian(status + offset + 1U, signo, 4);
	}
	const uint64_t s(time_to_tai64(now.tv_sec, false));
	const uint32_t n(now.tv_nsec);
	pack_bigendian(status + offset +  5U, s, 8);
	pack_bigendian(status + offset + 13U, n, 4);
}

void 
service::add_process (
	int pid
) {
	const bool affects_main_process(processes.empty());
	if (!processes.insert(pid).second) return;
	active_services.insert(active_service_map::value_type(pid, this));
#if !defined(__LINUX__) && !defined(__linux__)
	struct kevent e;
	EV_SET(&e, pid, EVFILT_PROC, EV_ADD, NOTE_EXIT|NOTE_FORK|NOTE_TRACK, 0, 0);
	kevent(queue, &e, 1, 0, 0, 0);
#endif
	if (affects_main_process) {
		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		stamp_time(now);
	}
}

void 
service::del_process (
	int pid,
	int s	///< a wait status from which WIFCONTINED and WIFSTOPPED have already been excluded
) {
	const bool affects_main_process(!processes.empty() && pid == *processes.begin());
#if !defined(__LINUX__) && !defined(__linux__)
	struct kevent e;
	EV_SET(&e, pid, EVFILT_PROC, EV_DELETE, NOTE_EXIT|NOTE_FORK|NOTE_TRACK, 0, 0);
	kevent(queue, &e, 1, 0, 0, 0);
#endif
	active_services.erase(pid);
	processes.erase(pid);
	if (affects_main_process) {
		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		stamp_time(now);
		switch (activity) {
			case START:	stamp_process_status(0, s, now); break;
			case RUN:	stamp_process_status(1, s, now); break;
			case RESTART:	stamp_process_status(2, s, now); break;
			case STOP:	stamp_process_status(3, s, now); break;
			case NONE:
			default:	break;
		}
	}
	current_process_status = s;
}

void
service::write_status()
{
	const ssize_t rc(pwrite(status_fd, status, sizeof status, 0));
	if (0 > rc) {
		const int error(errno);
		std::fprintf(stderr, "%s: WARNING: %s: %s: %s\n", prog, name, "supervise/status", std::strerror(error));
	}
}

inline
void
service::add_input_ready_event (int fd) 
{
	if (0 <= fd) {
#if !defined(__LINUX__) && !defined(__linux__)
		struct kevent e;
		EV_SET(&e, fd, EVFILT_READ, EV_ADD, 0, 0, 0);
		kevent(queue, &e, 1, 0, 0, 0);
#else
		pollfd p;
		p.fd = fd;
		p.events = POLLIN;
		poll_table.push_back(p);
#endif
	}
}

inline
void
service::delete_input_ready_event (int fd) 
{
	if (0 <= fd) {
#if !defined(__LINUX__) && !defined(__linux__)
		struct kevent e;
		EV_SET(&e, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
		kevent(queue, &e, 1, 0, 0, 0);
#else
		for (std::vector<pollfd>::iterator i(poll_table.begin()); i != poll_table.end(); )
			if (i->fd == fd)
				i = poll_table.erase(i);
			else
				++i;
#endif
	}
}

inline
void
service::add_to_input_activation_list () 
{
	const int fd(pipe_fds[0]);
	if (0 <= fd) {
		add_input_ready_event(fd);
		input_activated_services.insert(input_activated_service_map::value_type(fd, this));
	}
}

inline
void
service::delete_from_input_activation_list () 
{
	const int fd(pipe_fds[0]);
	if (0 <= fd) {
		input_activated_services.erase(fd);
		delete_input_ready_event(fd);
	}
}

inline
void
service::add_to_control_fifo_list () 
{
	const int fd(control_fd);
	if (0 <= fd) {
		add_input_ready_event(fd);
		service_control_fifos.insert(service_control_fifo_map::value_type(fd, this));
	}
}

inline
void
service::delete_from_control_fifo_list () 
{
	const int fd(control_fd);
	if (0 <= fd) {
		service_control_fifos.erase(fd);
		delete_input_ready_event(fd);
	}
}

void 
service::killall(
	int signo
) {
	for (std::set<int>::const_iterator i(processes.begin()); processes.end() != i; ++i) {
		const int pid(*i);
		kill(pid, signo);
	}
}

void 
service::killtop(
	int signo
) {
	if (has_processes()) {
		const int pid(*processes.begin());
		kill(pid, signo);
	}
}

inline
void 
service::enact_control_message (
	const sigset_t & original_signals,
	char command
) {
	switch (command) {
		case 'd':
			pending_command = command;
			stamp_pending_command();
			write_status();
			killall(SIGTERM);
			killall(SIGCONT);
			change_state_if_necessary(original_signals);
			break;
		case '_':
			command = 'd';
			// Fall through to:
			[[clang::fallthrough]];
		case 'u':
		case 'o':
		case 'O':
			pending_command = command;
			stamp_pending_command();
			write_status();
			change_state_if_necessary(original_signals);
			break;
		case 'H':	killtop(SIGHUP); break;
		case 'K':	killtop(SIGKILL); break;
		case 'T':	killtop(SIGTERM); break;
		case '1':	killall(SIGUSR1); break;
		case '2':	killall(SIGUSR2); break;
		case 'a':	killall(SIGALRM); break;
		case 'c':	killall(SIGCONT); break;
		case 'h':	killall(SIGHUP); break;
		case 'i':	killall(SIGINT); break;
		case 'k':	killall(SIGKILL); break;
		case 'p':	killall(SIGSTOP); break;
		case 'q':	killall(SIGQUIT); break;
		case 't':	killall(SIGTERM); break;
		case 'w':	killall(SIGWINCH); break;
		case 'z':	killall(SIGTSTP); break;
		case 'x':	set_unload(); break;
	}
}

inline
void 
service::enact_control_message (
	const sigset_t & original_signals
) {
	char command;
	const int rc(read(control_fd, &command, 1));
	if (0 > rc) return;
	enact_control_message(original_signals, command);
}

static const char * const start_args[] = { "start", 0 };
static const char * const run_args[] = { "run", 0 };
static const char * const stop_args[] = { "stop", 0 };

inline
void
service::enter_state (
	const sigset_t & original_signals
) {
	if (has_processes()) return;

	const char * const * a(0);
	const char * restart_args[] = { "restart", 0, 0, 0, 0 };
	switch (activity) {
		default:	
			sleep(1); 
			write_status(); 
			return;
		case NONE:	
			write_status(); 
			return;
		case START:
		{
			timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			a = start_args; 
			stamp_process_status(0, -1, now); 
			stamp_process_status(1, -1, now); 
			stamp_process_status(2, -1, now); 
			stamp_process_status(3, -1, now); 
			write_status(); 
			break;
		}
		case RUN:	
		{
			timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			a = run_args; 
			stamp_process_status(1, -1, now); 
			write_status(); 
			break;
		}
		case RESTART:	
		{
			timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			a = restart_args; 
			stamp_process_status(2, -1, now); 
			write_status(); 
			break;
		}
		case STOP:	
			a = stop_args; 
			break;
	}

#if !defined(__OpenBSD__)
	const FileDescriptorOwner fd(open_exec_at(service_dir_fd, *a));
	if (0 > fd.get()) {
		const int error(errno);
		std::fprintf(stderr, "%s: ERROR: %s/%s: %s\n", prog, name, *a, std::strerror(error));
		sleep(1);
		return;
	}
#endif

	std::fflush(stderr);
	const int rc(fork());
	if (0 > rc) {
		const int error(errno);
		std::fprintf(stderr, "%s: ERROR: %s/%s: %s\n", prog, name, *a, std::strerror(error));
		sleep(1);
		return;
	}
	if (0 < rc) {
		std::fprintf(stderr, "%s: INFO: %s/%s: pid %d\n", prog, name, *a, rc);
		add_process(rc);
		write_status();
		return;
	}

	// Child process only from now on.

	char codebuf[16];
	switch (activity) {
		default:	break;
		case RESTART:	
			if (WIFSIGNALED(current_process_status)) {
				const int signo(WTERMSIG(current_process_status));
				snprintf(codebuf, sizeof codebuf, "%u", signo);
				const char * sname(signame(signo));
				if (!sname) sname = codebuf;
				restart_args[1] = classify_signal(signo);
				restart_args[2] = sname;
				restart_args[3] = codebuf;
			} else {
				const int code(WEXITSTATUS(current_process_status));
				snprintf(codebuf, sizeof codebuf, "%u", code);
				restart_args[1] = "exit";
				restart_args[2] = codebuf;
			}
			break;
	}

	sigprocmask(SIG_SETMASK, &original_signals, 0);
	fchdir(service_dir_fd);

	dup2(in, STDIN_FILENO);
	dup2(out, STDOUT_FILENO);
	dup2(err, STDERR_FILENO);
	if (in != STDIN_FILENO) close(in);
	if (out != STDOUT_FILENO) close(out);
	if (err != STDERR_FILENO) close(err);

#if defined(__OpenBSD__)
	execve(*a, const_cast<char **>(a), environ);
#else
	fexecve(fd.get(), const_cast<char **>(a), environ);
#endif
	const int error(errno);
	std::fprintf(stderr, "%s: ERROR: %s/%s: %s\n", prog, name, *a, std::strerror(error));
	std::fflush(stderr);
	sleep(1);
	_exit(EXIT_TEMPORARY_FAILURE);
}

void 
service::change_state_if_necessary (
	const sigset_t & original_signals
) {
	const enum ActivityType prior_activity(activity);
	switch (activity) {
		case NONE:
			switch (pending_command) {
				case 'O': 	pending_command = '\0'; break;
				case 'o':	activity = START; break;
				case 'u':	activity = START; break;
				case 'd':	pending_command = '\0'; break;
				default:	break;
			}
			break;
		case START:	
			if (!has_processes()) 
			switch (pending_command) {
				case 'O': 	activity = RUN; break;
				case 'o':	activity = RUN; break;
				case 'u':	pending_command = '\0'; activity = RUN; break;
				case 'd':	activity = STOP; break;
				default:	activity = RUN; break;
			}
			break;
		case RUN:	
			if (has_processes())
			switch (pending_command) {
				case 'O': 	break;
				case 'o':	break;
				case 'u':	pending_command = '\0'; break;
				case 'd':	break;
				default:	break;
			}
			else if (run_on_empty)
			switch (pending_command) {
				case 'O': 	break;
				case 'o':	break;
				case 'u':	pending_command = '\0'; break;
				case 'd':	activity = RESTART; break;
				default:	break;
			}
			else 
			switch (pending_command) {
				case 'O': 	activity = RESTART; break;
				case 'o':	activity = RESTART; break;
				case 'u':	pending_command = '\0'; activity = RESTART; break;
				case 'd':	activity = RESTART; break;
				default:	activity = RESTART; break;
			}
			break;
		case RESTART:
			if (!has_processes()) 
			switch (pending_command) {
				case 'O': 	activity = STOP; break;
				case 'o':	activity = STOP; break;
				case 'u':	pending_command = '\0'; activity = is_failure(current_process_status) ? STOP : RUN; break;
				case 'd':	activity = STOP; break;
				default:	activity = is_failure(current_process_status) ? STOP : RUN; break;
			}
			break;
		case STOP:
			if (!has_processes()) 
			switch (pending_command) {
				case 'O': 	pending_command = '\0'; activity = NONE; break;
				case 'o':	pending_command = '\0'; activity = NONE; break;
				case 'u':	activity = START; break;
				case 'd':	pending_command = '\0'; activity = NONE; break;
				default:	activity = NONE; break;
			}
			break;
	}
	stamp_activity();
	stamp_pending_command();
	if (prior_activity != activity)
		enter_state(original_signals);
	else
		write_status();
}

inline
void
service::reap (
	const sigset_t & original_signals,
	int s,			///< status word from waitpid()
	int pid
) {
	// The child process might not have actually gone.
#if defined(WIFCONTINUED) && defined(WCONTINUED)
	if (WIFCONTINUED(s)) {
		paused = false;
		stamp_activity();
		write_status();
		return;
	}
#endif
	if (WIFSTOPPED(s)) {
		paused = true;
		stamp_activity();
		write_status();
		return;
	}

	// We have at this point excluded everything apart from normal exit and termination by a signal.
	del_process(pid, s);
	write_status();
	change_state_if_necessary(original_signals);
}

/* Signal handlers **********************************************************
// **************************************************************************
*/

#if defined(__LINUX__) || defined(__linux__)

static sig_atomic_t child_signalled = false;

static
void
sig_child (
	int signo
) {
	if (SIGCHLD != signo) return;
	child_signalled = true;
}

static sig_atomic_t stop_signalled = false;

static 
void 
sig_hup ( 
	int signo 
) {
	if (SIGHUP != signo) return;
	stop_signalled = true;
}

static 
void 
sig_term ( 
	int signo 
) {
	if (SIGTERM != signo) return;
	stop_signalled = true;
}

static 
void 
sig_int ( 
	int signo 
) {
	if (SIGINT != signo) return;
	stop_signalled = true;
}

static 
void 
sig_quit ( 
	int signo 
) {
	if (SIGQUIT != signo) return;
	stop_signalled = true;
}

static void sig_tstp ( int ) {}

#else

// A way to set SIG_IGN that is reset by execve().
static void sig_ignore ( int ) {}

#endif

/* Service Manager control API RPC handlers *********************************
// **************************************************************************
*/

static
void
plumb (
	int out_supervise_dir_fd,
	int in_supervise_dir_fd
) {
	struct stat in_supervise_dir_s;
	if (!is_directory(in_supervise_dir_fd, in_supervise_dir_s)) return;
	service_map::iterator in_supervise_dir_i(services.find(in_supervise_dir_s));
	if (in_supervise_dir_i == services.end()) return;
	service & in_s(in_supervise_dir_i->second);

	struct stat out_supervise_dir_s;
	if (!is_directory(out_supervise_dir_fd, out_supervise_dir_s)) return;
	service_map::iterator out_supervise_dir_i(services.find(out_supervise_dir_s));
	if (out_supervise_dir_i == services.end()) return;
	service & out_s(out_supervise_dir_i->second);

	std::fprintf(stderr, "%s: DEBUG: plumb %s to %s\n", prog, out_s.name, in_s.name);
	if (-1 != in_s.pipe_fds[1])
		out_s.err = out_s.out = in_s.pipe_fds[1];
}

static
void
load (
	const char * name,
	int supervise_dir_fd,
	int service_dir_fd
) {
	struct stat service_dir_s;
	if (!is_directory(service_dir_fd, service_dir_s)) return;
	struct stat supervise_dir_s;
	if (!is_directory(supervise_dir_fd, supervise_dir_s)) return;

	service_map::iterator i(services.find(service_dir_s));
	if (i == services.end()) {
		FileDescriptorOwner service_dir_fd2(dup(service_dir_fd));
		if (0 > service_dir_fd2.get()) return;
		set_close_on_exec(service_dir_fd2.get(), true);
		//
		// We need an explicit lock file, because we cannot lock FIFOs.
		FileDescriptorOwner lock_fd(open_lockfile_at(supervise_dir_fd, "lock"));
		if (0 > lock_fd.get()) return;
		//
		// We are allowed to open the read end of a FIFO in non-blocking mode without having to wait for a writer.
		mkfifoat(supervise_dir_fd, "control", 0600);
		FileDescriptorOwner control_fd(open_read_at(supervise_dir_fd, "control"));
		if (0 > control_fd.get()) return;
		//
		// We have to keep a client (write) end descriptor open to the control FIFO.
		// Otherwise, the first control client process triggers POLLHUP when it closes its end.
		// Opening the FIFO for read+write isn't standard, although it would work on Linux.
		FileDescriptorOwner control_client_fd(open_writeexisting_at(supervise_dir_fd, "control"));
		if (0 > control_client_fd.get()) return;
		//
		// Unlike daemontools, but like daemontools-encore, we keep the status file open continually.
		// This permits the supervise directory to be read-only.
		FileDescriptorOwner status_fd(open_writetrunc_at(supervise_dir_fd, "status", 0644));
		if (0 > status_fd.get()) return;
		//
		// The existence of a reader at this FIFO indicates that a supervisor is active.
		// We must open this after the rest of the control/status API is initialized.
		// Otherwise clients might try to issue commands/read statuses before the FIFOs are open.
		mkfifoat(supervise_dir_fd, "ok", 0666);
		FileDescriptorOwner ok_fd(open_read_at(supervise_dir_fd, "ok"));
		if (0 > ok_fd.get()) return;

		timespec now;
		clock_gettime(CLOCK_REALTIME, &now);
		std::pair<service_map::iterator, bool> it(services.insert(service_map::value_type(supervise_dir_s,supervise_dir_s)));
		service & s(it.first->second);
		s.lock_fd = lock_fd.release();
		s.ok_fd = ok_fd.release();
		s.control_fd = control_fd.release();
		s.control_client_fd = control_client_fd.release();
		s.status_fd = status_fd.release();
		s.service_dir_fd = service_dir_fd2.release();
		std::strncpy(s.name, name, sizeof s.name);
		s.stamp_time(now);
		s.stamp_activity();
		s.stamp_pending_command();
		for (unsigned state(0U); state < 4U; ++state)
			s.stamp_process_status(state, -1, now);
		s.write_status();
		s.add_to_control_fifo_list();
		std::fprintf(stderr, "%s: DEBUG: load %s\n", prog, s.name);
	}
}

static
void
make_input_activated (
	int supervise_dir_fd
) {
	struct stat supervise_dir_s;
	if (!is_directory(supervise_dir_fd, supervise_dir_s)) return;
	service_map::iterator supervise_dir_i(services.find(supervise_dir_s));
	if (supervise_dir_i == services.end()) return;
	service & s(supervise_dir_i->second);

	std::fprintf(stderr, "%s: DEBUG: make input activated %s\n", prog, s.name);
	s.add_to_input_activation_list();
}

static
void
set_unload (
	int supervise_dir_fd
) {
	struct stat supervise_dir_s;
	if (!is_directory(supervise_dir_fd, supervise_dir_s)) return;
	service_map::iterator supervise_dir_i(services.find(supervise_dir_s));
	if (supervise_dir_i == services.end()) return;

	service & s(supervise_dir_i->second);
	std::fprintf(stderr, "%s: DEBUG: set unload after stop %s\n", prog, s.name);
	s.set_unload();
	if (s.unloadable()) {
		std::fprintf(stderr, "%s: DEBUG: unloading %s\n", prog, s.name);
		services.erase(supervise_dir_i);
	}
}

static
void
make_pipe_connectable (
	int supervise_dir_fd
) {
	struct stat supervise_dir_s;
	if (!is_directory(supervise_dir_fd, supervise_dir_s)) return;
	service_map::iterator supervise_dir_i(services.find(supervise_dir_s));
	if (supervise_dir_i == services.end()) return;
	service & s(supervise_dir_i->second);

	std::fprintf(stderr, "%s: DEBUG: add pipe for %s\n", prog, s.name);
	if (-1 == s.pipe_fds[1] && -1 == s.pipe_fds[0]) {
		if (0 <= pipe_close_on_exec(s.pipe_fds))
			s.in = s.pipe_fds[0];
	}
}

static
void
make_run_on_empty (
	int supervise_dir_fd
) {
	struct stat supervise_dir_s;
	if (!is_directory(supervise_dir_fd, supervise_dir_s)) return;
	service_map::iterator supervise_dir_i(services.find(supervise_dir_s));
	if (supervise_dir_i == services.end()) return;
	service & s(supervise_dir_i->second);

	std::fprintf(stderr, "%s: DEBUG: run-on-empty set for %s\n", prog, s.name);
	s.run_on_empty = true;
}

/* Support functions ********************************************************
// **************************************************************************
*/

#if !defined(__LINUX__) && !defined(__linux__)
static inline
void
register_forked_child (
	int pid,
	int ppid
) {
	active_service_map::iterator i(active_services.find(ppid));
	if (i == active_services.end()) {
		std::fprintf(stderr, "%s: DEBUG: unable to register PID %u child of PPID %u\n", prog, pid, ppid);
		return;
	}
	service & s(*i->second);

	s.add_process(pid);
}
#endif

static inline
void
reap (
	const sigset_t & original_signals,
	int status,
	int pid
) {
	active_service_map::iterator i(active_services.find(pid));
	if (i == active_services.end()) {
		if (WIFSTOPPED(status))
			kill(pid, SIGCONT);
		return;
	}
	service & s(*i->second);

	s.reap(original_signals, status, pid);
	if (s.unloadable()) {
		service_map::iterator j(services.find(s));
		if (j != services.end()) {
			std::fprintf(stderr, "%s: DEBUG: unloading %s\n", prog, s.name);
			services.erase(j);
		}
	}
}

static inline
void
reaper (
	const sigset_t & original_signals
) {
	for (;;) {
		int status;
#if defined(WIFCONTINUED) && defined(WCONTINUED)
		const pid_t c(waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED));
#else
		const pid_t c(waitpid(-1, &status, WNOHANG|WUNTRACED));
#endif
		if (0 >= c) break;
		reap(original_signals, status, c);
	}
}
 
static inline
void
input_ready_event (
	const sigset_t & original_signals,
	int fd
) {
	{
		input_activated_service_map::iterator i(input_activated_services.find(fd));
		if (i != input_activated_services.end()) {
			service & s(*i->second);

			std::fprintf(stderr, "%s: DEBUG: input activate %s\n", prog, s.name);
			s.delete_from_input_activation_list();
			s.enact_control_message(original_signals, 'u');
		}
	}
	{
		service_control_fifo_map::iterator i(service_control_fifos.find(fd));
		if (i != service_control_fifos.end()) {
			service & s(*i->second);

			s.enact_control_message(original_signals);
		}
	}
}

static inline
void
control_message (
	int socket_fd
) {
	service_manager_rpc_message m;
	struct iovec v[1] = { { &m, sizeof m } };
	char buf[CMSG_SPACE(3 * sizeof(int))];
	struct msghdr msg = {
		0, 0,
		v, sizeof v/sizeof *v,
		buf, sizeof buf,
		0
	};
	const ssize_t rc(recvmsg(socket_fd, &msg, 0));
	if (0 > rc) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "recvmsg", std::strerror(error));
		return;
	}
	for (struct cmsghdr *cmsg(CMSG_FIRSTHDR(&msg)); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
		if (SOL_SOCKET == cmsg->cmsg_level && SCM_RIGHTS == cmsg->cmsg_type) {
			const int * fds(reinterpret_cast<int*>(CMSG_DATA(cmsg)));
			const size_t count_fds((cmsg->cmsg_len - CMSG_LEN(0)) / sizeof *fds);
			for (size_t i(0); i < count_fds; ++i)
				set_close_on_exec(fds[i], true);
		}
	}
	for (struct cmsghdr *cmsg(CMSG_FIRSTHDR(&msg)); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
		if (SOL_SOCKET == cmsg->cmsg_level && SCM_RIGHTS == cmsg->cmsg_type) {
			const int * fds(reinterpret_cast<int*>(CMSG_DATA(cmsg)));
			const size_t count_fds((cmsg->cmsg_len - CMSG_LEN(0)) / sizeof *fds);
			switch (m.command) {
				case service_manager_rpc_message::PLUMB:
					plumb(fds[0], fds[1]);
					break;
				case service_manager_rpc_message::LOAD:
					load(m.name, fds[0], fds[1]);
					break;
				case service_manager_rpc_message::MAKE_INPUT_ACTIVATED:
					make_input_activated(fds[0]);
					break;
				case service_manager_rpc_message::UNLOAD:
					set_unload(fds[0]);
					break;
				case service_manager_rpc_message::MAKE_PIPE_CONNECTABLE:
					make_pipe_connectable(fds[0]);
					break;
				case service_manager_rpc_message::MAKE_RUN_ON_EMPTY:
					make_run_on_empty(fds[0]);
					break;
				default:
					std::fprintf(stderr, "%s: WARNING: unknown control message command %u with %lu file descriptors\n", prog, m.command, count_fds);
					break;
			}
			for (size_t i(0); i < count_fds; ++i)
				close(fds[i]);
		}
	}
}

static
void
stop_and_unload_all (
	const sigset_t & original_signals
) {
	for (service_map::iterator supervise_dir_i(services.begin()); services.end() != supervise_dir_i; ) {
		// Pre-increment because we might be erasing.
		service_map::iterator i(supervise_dir_i++);
		service & s(i->second);
		s.enact_control_message(original_signals, 'd');
		std::fprintf(stderr, "%s: DEBUG: set unload after stop %s\n", prog, s.name);
		s.set_unload();
		if (s.unloadable()) {
			std::fprintf(stderr, "%s: DEBUG: unloading %s\n", prog, s.name);
			services.erase(i);
		}
	}
}

/* Main function ************************************************************
// **************************************************************************
*/

void
service_manager [[gnu::noreturn]] (
	const char * & /*next_prog*/,
	std::vector<const char *> & args
) {
	prog = basename_of(args[0]);

	const int dev_null_fd(openat(AT_FDCWD, "/dev/null", O_NOCTTY|O_CLOEXEC|O_RDWR|O_NONBLOCK));
	if (0 > dev_null_fd) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "/dev/null", std::strerror(error));
		throw EXIT_FAILURE;
	}
	dup2(dev_null_fd, STDIN_FILENO);

	sigset_t original_signals;
	sigprocmask(SIG_SETMASK, 0, &original_signals);

	const unsigned listen_fds(query_listen_fds());
	if (1U > listen_fds) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "LISTEN_FDS", std::strerror(error));
		throw EXIT_FAILURE;
	}

	subreaper(true);

#if !defined(__LINUX__) && !defined(__linux__)
	queue = kqueue();
	if (0 > queue) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "kqueue", std::strerror(error));
		throw EXIT_FAILURE;
	}

	{
		std::vector<struct kevent> p(listen_fds + 7);
		for (unsigned i(0U); i < listen_fds; ++i)
			EV_SET(&p[i], LISTEN_SOCKET_FILENO + i, EVFILT_READ, EV_ADD, 0, 0, 0);
		EV_SET(&p[listen_fds + 0], SIGHUP, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);
		EV_SET(&p[listen_fds + 1], SIGTERM, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);
		EV_SET(&p[listen_fds + 2], SIGINT, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);
		EV_SET(&p[listen_fds + 3], SIGQUIT, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);
		EV_SET(&p[listen_fds + 4], SIGTSTP, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);
		EV_SET(&p[listen_fds + 5], SIGCHLD, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);
		EV_SET(&p[listen_fds + 6], SIGPIPE, EVFILT_SIGNAL, EV_ADD, 0, 0, 0);
		if (0 > kevent(queue, p.data(), listen_fds + 6, 0, 0, 0)) {
			const int error(errno);
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "kevent", std::strerror(error));
			throw EXIT_FAILURE;
		}

		struct sigaction sa;
		sa.sa_flags=0;
		sigemptyset(&sa.sa_mask);
		// We use a sig_ignore function rather than SIG_IGN so that all signals are automatically reset to their default actions on execve().
		sa.sa_handler=sig_ignore;
		sigaction(SIGHUP,&sa,NULL);
		sigaction(SIGTERM,&sa,NULL);
		sigaction(SIGINT,&sa,NULL);
		sigaction(SIGQUIT,&sa,NULL);
		sigaction(SIGTSTP,&sa,NULL);
		sigaction(SIGCHLD,&sa,NULL);
		sigaction(SIGPIPE,&sa,NULL);
	}
#else
	for (unsigned i(0U); i < listen_fds; ++i) {
		pollfd p;
		p.fd = LISTEN_SOCKET_FILENO + i;
		p.events = POLLIN;
		poll_table.push_back(p);
	}

	{
		struct sigaction sa;
		sa.sa_flags=0;
		sigemptyset(&sa.sa_mask);
		sa.sa_handler=sig_hup;
		sigaction(SIGHUP,&sa,NULL);
		sa.sa_handler=sig_term;
		sigaction(SIGTERM,&sa,NULL);
		sa.sa_handler=sig_int;
		sigaction(SIGINT,&sa,NULL);
		sa.sa_handler=sig_tstp;
		sigaction(SIGTSTP,&sa,NULL);
		sa.sa_handler=sig_child;
		sigaction(SIGCHLD,&sa,NULL);
		sa.sa_handler=sig_quit;
		sigaction(SIGQUIT,&sa,NULL);
	}

	sigset_t masked_signals(original_signals);
	sigaddset(&masked_signals, SIGHUP);
	sigaddset(&masked_signals, SIGTERM);
	sigaddset(&masked_signals, SIGINT);
	sigaddset(&masked_signals, SIGTSTP);
	sigaddset(&masked_signals, SIGCHLD);
	sigaddset(&masked_signals, SIGPIPE);
	sigprocmask(SIG_SETMASK, &masked_signals, 0);

	sigset_t masked_signals_during_poll(masked_signals);
	sigdelset(&masked_signals_during_poll, SIGHUP);
	sigdelset(&masked_signals_during_poll, SIGTERM);
	sigdelset(&masked_signals_during_poll, SIGINT);
	sigdelset(&masked_signals_during_poll, SIGTSTP);
	sigdelset(&masked_signals_during_poll, SIGCHLD);
	sigdelset(&masked_signals_during_poll, SIGPIPE);
#endif

	bool in_shutdown(false);
	for (;;) {
		try {
			if (in_shutdown) {
				if (services.empty()) break;
				std::fprintf(stderr, "%s: INFO: %s %lu %s\n", prog, "Shutdown requested but there are", services.size(), "services still active.");
			}
#if !defined(__LINUX__) && !defined(__linux__)
			struct kevent p[1024];
			const int rc(kevent(queue, 0, 0, p, sizeof p/sizeof *p, 0));
			if (0 > rc) {
				const int error(errno);
				if (EINTR == error) continue;
				std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "kevent", std::strerror(error));
				throw EXIT_FAILURE;
			}
			for (std::size_t i(0); i < static_cast<std::size_t>(rc); ++i) {
				const struct kevent & e(p[i]);
				switch (e.filter) {
					case EVFILT_READ:
						if (LISTEN_SOCKET_FILENO <= static_cast<int>(e.ident) && LISTEN_SOCKET_FILENO + static_cast<int>(listen_fds) > static_cast<int>(e.ident))
							control_message(e.ident);
						else
							input_ready_event(original_signals, e.ident);
						break;
					case EVFILT_SIGNAL:
						switch (e.ident) {
							case SIGTERM:
							case SIGINT:
							case SIGQUIT:
							case SIGHUP:
								std::fprintf(stderr, "%s: DEBUG: stop signalled\n", prog);
								stop_and_unload_all(original_signals);
								std::fprintf(stderr, "%s: DEBUG: setting in_shutdown to true\n", prog);
								in_shutdown = true;
								break;
							case SIGCHLD:
								reaper(original_signals);
								break;
							case SIGPIPE:
							case SIGTSTP:
							default:
								std::fprintf(stderr, "%s: DEBUG: signal event ident %lu fflags %x\n", prog, e.ident, e.fflags);
								break;
						}
						break;
					case EVFILT_VNODE:
#if defined(DEBUG)
						std::fprintf(stderr, "%s: DEBUG: vnode event ident %lu fflags %x\n", prog, e.ident, e.fflags);
#endif
						break;
					case EVFILT_PROC:
						if (e.fflags & NOTE_FORK) {
#if defined(DEBUG)
							std::fprintf(stderr, "%s: DEBUG: proc event PID %lu forked\n", prog, e.ident);
#endif
						}
						if (e.fflags & NOTE_CHILD)
							register_forked_child(e.ident, e.data);
						if (e.fflags & NOTE_EXIT)
							reap(original_signals, e.data, e.ident);
						break;
					default:
#if defined(DEBUG)
						std::fprintf(stderr, "%s: DEBUG: event filter %hd ident %lu fflags %x\n", prog, e.filter, e.ident, e.fflags);
#endif
						break;
				}
			}
#else
			if (child_signalled) {
				reaper(original_signals);
				child_signalled = false;
				continue;
			}
			if (stop_signalled) {
				stop_and_unload_all(original_signals);
				stop_signalled = false;
				in_shutdown = true;
				continue;
			}
			if (0 > ppoll(poll_table.data(), poll_table.size(), 0, &masked_signals_during_poll)) {
				const int error(errno);
				if (EINTR == error) continue;
				std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "ppoll", std::strerror(error));
				throw EXIT_FAILURE;
			}
			for (std::vector<pollfd>::iterator i(poll_table.begin()); i != poll_table.end(); ++i) {
				if (i->revents & POLLIN) {
					if (LISTEN_SOCKET_FILENO <= i->fd && LISTEN_SOCKET_FILENO + static_cast<int>(listen_fds) > i->fd)
						control_message(i->fd);
					else
						input_ready_event(original_signals, i->fd);
					break;
				} else {
					if (i->revents) std::fprintf(stderr, "%s: DEBUG: fd %d ignore revents %x\n", prog, i->fd, i->revents);
				}
			}
#endif
		} catch (const std::exception & e) {
			std::fprintf(stderr, "%s: ERROR: exception: %s\n", prog, e.what());
		}
	}
	std::fprintf(stderr, "%s: DEBUG: all engines stop\n", prog);
	throw EXIT_SUCCESS;
}
