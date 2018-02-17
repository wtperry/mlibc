
#include <bits/ensure.h>
#include <sys/socket.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

int accept(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int bind(int, const struct sockaddr *, socklen_t) {
	frigg::infoLogger() << "\e[31mmlibc: bind() is a no-op\e[39m" << frigg::endLog;
	return 0;
}
// connect() is provided by the platform

int getpeername(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getsockname(int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
	if(layer == SOL_SOCKET && number == SO_PEERCRED) {
		__ensure(*size == sizeof(struct ucred));
		frigg::infoLogger() << "\e[31mmlibc: getsockopt(SO_PEERCRED) returns all zeros\e[39m"
				<< frigg::endLog;

		struct ucred creds;
		creds.pid = 0;
		creds.uid = 0;
		creds.gid = 0;
		memcpy(buffer, &creds, sizeof(struct ucred));
		return 0;
	}else{
		frigg::panicLogger() << "mlibc: Unexpected getsockopt call" << frigg::endLog;
	}
}

int listen(int, int) {
	frigg::infoLogger() << "\e[31mmlibc: listen() is a no-op\e[39m" << frigg::endLog;
	return 0;
}

ssize_t recv(int, void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t recvfrom(int, void *__restrict, size_t, int, struct sockaddr *__restrict, socklen_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t recvmsg(int fd, struct msghdr *msgh, int flags) {
	__ensure(msgh->msg_iovlen);

	frigg::infoLogger() << "mlibc: recvmsg() control length: " << msgh->msg_controllen
			<< frigg::endLog;

	ssize_t size;
	if(mlibc::sys_read(fd, msgh->msg_iov[0].iov_base, msgh->msg_iov[0].iov_len, &size))
		return -1;
	
	msgh->msg_controllen = 0;

	return size;
}

ssize_t send(int, const void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t sendmsg(int fd, const struct msghdr *msgh, int flags) {
	__ensure(msgh->msg_iovlen);

	frigg::infoLogger() << "mlibc: sendmsg() control length: " << msgh->msg_controllen
			<< frigg::endLog;
	__ensure(!msgh->msg_controllen);

	ssize_t size;
	if(mlibc::sys_write(fd, msgh->msg_iov[0].iov_base, msgh->msg_iov[0].iov_len, &size))
		return -1;
	return size;
}

ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setsockopt(int, int, int, const void *, socklen_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int shutdown(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sockatmark(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int socket(int family, int type, int protocol) {
	int fd;
	if(mlibc::sys_socket(family, type, protocol, &fd))
		return -1;
	return fd;
}

// connectpair() is provided by the platform
