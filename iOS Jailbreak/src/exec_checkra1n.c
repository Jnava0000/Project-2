#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <spawn.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <sys/utsname.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

extern char **environ;
extern char* gOverrideLibcheckra1nHelper;

#include <palerain.h>
#ifdef TUI
#include <tui.h>
#endif
#include <xxd-embedded.h>
char* pongo_path = NULL;
bool external_pongo = false;
char* ext_checkra1n = NULL;

#define tmpdir getenv("TMPDIR") == NULL ? "/tmp" : getenv("TMPDIR")

int exec_checkra1n(void) {
	LOG(LOG_INFO, "About to execute checkra1n");
	int fd_checkra1n, fd_pongo, ret;
	char* checkra1n_path = NULL;
#ifndef NO_CUSTOM_PONGO
	if (pongo_path == NULL) {
		pongo_path = malloc(strlen(tmpdir) + 20);
		if (pongo_path == NULL) {
			LOG(LOG_FATAL, "memory allocation failed\n");
			return -1;
		}
		snprintf(pongo_path, strlen(tmpdir) + 20, "%s/Pongo.bin.XXXXXX", tmpdir);
		fd_pongo = mkstemp(pongo_path);
		if (fd_pongo == -1) {
			LOG(LOG_FATAL, "Cannot open temporary file %s: %d (%s)", pongo_path, errno, strerror(errno));
			free(pongo_path);
			return -1;
		}
		ssize_t didWrite_Pongo_bin = write(fd_pongo, Pongo_bin, Pongo_bin_len);
		if (didWrite_Pongo_bin != (ssize_t)Pongo_bin_len) {
			LOG(LOG_FATAL, "Size written to %s does not match expected: %zd != %" PRIu32 ": %d (%s)", pongo_path, didWrite_Pongo_bin, Pongo_bin_len, errno, strerror(errno));
			close(fd_pongo);
			unlink(pongo_path);
			free(pongo_path);
			pongo_path = NULL;
			return -1;
		}
	} else external_pongo = true;
#endif
	if (ext_checkra1n != NULL) {
		checkra1n_path = ext_checkra1n;
	} else {
		checkra1n_path = malloc(strlen(tmpdir) + 20);
		if (checkra1n_path == NULL) {
			LOG(LOG_FATAL, "memory allocation failed\n");
			return -1;
		}
		snprintf(checkra1n_path, strlen(tmpdir) + 20, "%s/checkra1n.XXXXXX", tmpdir);
		fd_checkra1n = mkstemp(checkra1n_path);
		if (fd_checkra1n == -1) {
			LOG(LOG_FATAL, "Cannot open temporary file %s: %d (%s)", checkra1n_path, errno, strerror(errno));
			free(checkra1n_path);
			checkra1n_path = NULL;
			return -1;
		}
		ssize_t didWrite_checkra1n = write(fd_checkra1n, checkra1n, checkra1n_len);
		if (didWrite_checkra1n != (ssize_t)checkra1n_len) {
			LOG(LOG_FATAL, "Size written to %s does not match expected: %zd != %" PRIu32 ": %d (%s)", checkra1n_path, didWrite_checkra1n, checkra1n_len, errno, strerror(errno));
			close(fd_checkra1n);
			unlink(checkra1n_path);
			free(checkra1n_path);
			checkra1n_path = NULL;
			return -1;
		}
		close(fd_checkra1n);
		ret = chmod(checkra1n_path, 0700);
		if (ret) {
			LOG(LOG_FATAL, "Cannot chmod %s: %d (%s)", checkra1n_path, errno, strerror(errno));
			unlink(checkra1n_path);
			free(checkra1n_path);
			checkra1n_path = NULL;
			return -1;
		}
	}
#if defined(__APPLE__) && defined(__arm64__) && (TARGET_OS_IPHONE || defined(FORCE_HELPER))
	char* libcheckra1nhelper_dylib_path = NULL;
	{
		struct utsname name;
		uname(&name);
		unsigned long darwinMajor = strtoul(name.release, NULL, 10);
		assert(darwinMajor != 0);
#if !defined(FORCE_HELPER)
		if (darwinMajor < 20) {
#endif
		if (gOverrideLibcheckra1nHelper) {
			libcheckra1nhelper_dylib_path = gOverrideLibcheckra1nHelper;
			goto setenv_ra1n;
		}
			libcheckra1nhelper_dylib_path = malloc(strlen(tmpdir) + 40);
			if (libcheckra1nhelper_dylib_path == NULL) {
				LOG(LOG_FATAL, "memory allocation failed\n");
				return -1;
			}
			snprintf(libcheckra1nhelper_dylib_path, strlen(tmpdir) + 40, "%s/libcheckra1nhelper.dylib.XXXXXX", tmpdir);
			int helper_fd = mkstemp(libcheckra1nhelper_dylib_path);
			if (helper_fd == -1) {
				LOG(LOG_FATAL, "Cannot open temporary file: %d (%s)", errno, strerror(errno));
				return -1;
			}
			ssize_t didWrite_libcheckra1nhelper = write(helper_fd, libcheckra1nhelper_dylib, libcheckra1nhelper_dylib_len);
			if ((unsigned int)didWrite_libcheckra1nhelper != libcheckra1nhelper_dylib_len) {
				LOG(LOG_FATAL, "Size written does not match expected: %zd != %" PRIu32 ": %d (%s)", didWrite_libcheckra1nhelper, libcheckra1nhelper_dylib_len, errno, strerror(errno));
				close(helper_fd);
				unlink(libcheckra1nhelper_dylib_path);
				return -1;
			}
			close(helper_fd);
			ret = chmod(libcheckra1nhelper_dylib_path, 0700);
			if (ret) {
				LOG(LOG_FATAL, "Cannot chmod %s: %d (%s)", libcheckra1nhelper_dylib_path, errno, strerror(errno));
				unlink(libcheckra1nhelper_dylib_path);
				return -1;
			}
setenv_ra1n:
			setenv("DYLD_INSERT_LIBRARIES", libcheckra1nhelper_dylib_path, 1);
#if !defined(FORCE_HELPER)
		}
#endif
	}
#endif
	char args[0x10] = "-E";
	if ((palerain_flags & palerain_option_demote)) strncat(args, "d", 0xf);
	if (!(palerain_flags & palerain_option_checkrain_is_clone)) {
		strncat(args, "p", 0xf);
		if (verbose >= 2) strncat(args, "v", 0xf);
		if ((palerain_flags & palerain_option_no_colors)) strncat(args, "n", 0xf);
	} else {
		strncat(args, "R", 0xf);
	}
	LOG(LOG_VERBOSE5, args);
	if (pongo_path != NULL) LOG(LOG_VERBOSE5, pongo_path);
	pid_t pid;
	posix_spawn_file_actions_t action;
    posix_spawn_file_actions_init(&action);
#ifdef TUI
	if (tui_is_jailbreaking) {
		// silence checkra1n
		posix_spawn_file_actions_addopen(&action, 1, "/dev/null", O_WRONLY, 0);
		posix_spawn_file_actions_addopen(&action, 2, "/dev/null", O_WRONLY, 0);
	}
#endif
	ret = posix_spawn(&pid, checkra1n_path, &action, NULL, (char* []){
		checkra1n_path,
		args,
		"-k",
		pongo_path,
		NULL
	}, environ);
	posix_spawn_file_actions_destroy(&action);
	if (ret) {
		LOG(LOG_FATAL, "Cannot posix spawn %s: %d (%s)", checkra1n_path, errno, strerror(errno));
		if (ext_checkra1n != NULL) unlink(checkra1n_path);
		return -1;
	}
	LOG(LOG_VERBOSE2, "%s spawned successfully", checkra1n_path);
	sleep(2);
	if (ext_checkra1n == NULL) {
		unlink(checkra1n_path);
		free(checkra1n_path);
		checkra1n_path = NULL;
	}
	waitpid(pid, NULL, 0);
		if (!external_pongo && pongo_path != NULL) {
		unlink(pongo_path);
	}
	if (pongo_path != NULL) free(pongo_path);
	pongo_path = NULL;
#if defined(__APPLE__) && defined(__arm64__) && (TARGET_OS_IPHONE || defined(FORCE_HELPER))
	if (libcheckra1nhelper_dylib_path != NULL) {
		if (!gOverrideLibcheckra1nHelper) unlink(libcheckra1nhelper_dylib_path);
		unsetenv("DYLD_INSERT_LIBRARIES");
		unsetenv("DYLD_FORCE_FLAT_NAMESPACE");
		if (!gOverrideLibcheckra1nHelper) free(libcheckra1nhelper_dylib_path);
		libcheckra1nhelper_dylib_path = NULL;
	}
#endif
	return 0;
}
