#!/bin/sh -e
objects="builtins.o appendpath.o chdir.o chroot.o clearenv.o console-clear.o console-convert-kbdmap.o console-fb-realizer.o console-multiplexor.o console-ncurses-realizer.o console-resize.o convert-fstab-services.o convert-systemd-units.o cyclog.o delegate-control-group-to.o detach-controlling-tty.o emergency-login.o envdir.o envuidgid.o exec.o export-to-rsyslog.o false.o fdmove.o fdredir.o fifo-listen.o follow-log-directories.o foreground-background.o get-mount.o initctl-read.o klog-read.o kmod.o line-banner.o local-datagram-socket-listen.o local-reaper.o local-stream-socket-accept.o local-stream-socket-listen.o login-banner.o login-process.o login-prompt.o machineenv.o make-private-fs.o monitor-fsck-progress.o monitored-fsck.o move-to-control-group.o nagios-check.o netlink-datagram-socket-listen.o nosh.o oom-kill-protect.o open-controlling-tty.o pause.o pipe.o plug-and-play-event-handler.o prependpath.o pty-get-tty.o pty-run.o read-conf.o recordio.o service-control.o service-dt-scanner.o service-is-enabled.o service-is-ok.o service-is-up.o service-manager.o service-show.o service-status.o service.o set-control-group-knob.o set-dynamic-hostname.o set-mount-object.o setenv.o setlock.o setpgrp.o setsid.o setuidgid-fromenv.o setuidgid.o setup-machine-id.o syslog-read.o system-version.o tai64n.o tai64nlocal.o tcp-socket-accept.o tcp-socket-connect.o tcp-socket-listen.o tcpserver.o true.o console-terminal-emulator.o ttylogin-starter.o ucspi-socket-rules-check.o udp-socket-connect.o udp-socket-listen.o ulimit.o unload-when-stopped.o umask.o unsetenv.o unshare.o userenv.o vc-get-tty.o vc-reset-tty.o"
redo-ifchange ./archive ${objects} ${extra}
./archive "$3" ${objects} ${extra}
