#!/bin/sh -e
objects="CompositeFont.o KeyboardIO.o FileDescriptorOwner.o FramebufferIO.o GraphicsInterface.o SoftTerm.o UnicodeClassification.o basename.o begins_with.o comment.o ends_in.o iovec.o is_jail.o is_set_hostname_allowed.o keycode_to_map.o listen.o nmount.o open_exec.o open_lockfile.o open_lockfile_or_wait.o pipe_close_on_exec.o popt-bool.o popt-compound.o popt-named.o popt.o popt-signed.o popt-simple.o popt-string-list.o popt-string.o popt-table.o popt-top-table.o popt-unsigned.o posix_clearenv.o process_env_dir.o raw.o read_env_file.o read_line.o read-file.o sane.o service-manager-client.o service-manager-socket.o socket_close_on_exec.o split_list.o tcgetattr.o tcgetwinsz.o tcsetattr.o tcsetwinsz.o tolower.o trim.o ttyname.o unpack.o val.o"
redo-ifchange ./archive ${objects}
./archive "$3" ${objects}
