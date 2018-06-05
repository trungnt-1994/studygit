#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PM_SUSPEND	"TZCS_SUSPEND"
#define PM_NORMAL	"TZCS_NORMAL"

#define PROC_POWER	"/proc/tzcs_power_state"

#define IS_EQUAL(s1, s2)	(strcmp(s1, s2) == 0)

static int fd_proc;	/* file descriptor for PROC_POWER */

/*
 * @brief Init function
 * */
__attribute__((constructor (101)))
static void __init(void)
{
	fd_proc = open(PROC_POWER, O_WRONLY, 0);
	if (fd_proc == -1) {
		fprintf(stderr, "%s:%s:%s failed to open %s: %s!\n",
				__FILE__, __LINE__, __func__, PROC_POWER, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	char buf[512];

	if (argc != 2) {
		fprintf(stderr, "Usage: write_proc_power %s|%s", PM_SUSPEND, PM_NORMAL);
		goto failed;
	}

	strcpy(buf, argv[1]);
	if (!IS_EQUAL(buf, PM_SUSPEND) && !IS_EQUAL(buf, PM_NORMAL)) {
		fprintf(stderr, "%s is invalid value!\n", buf);
		fprintf(stderr, "Usage: write_proc_power %s|%s", PM_SUSPEND, PM_NORMAL);
		goto failed;
	}

	if (write(fd_proc, buf, strlen(buf)) == -1) {
		fprintf(stderr, "%s:%s:%s failed to write to %s: %s!\n",
				__FILE__, __LINE__, __func__, PROC_POWER, strerror(errno));
		goto failed;
	}

	return EXIT_SUCCESS;

failed:
	close(fd_proc);
	return EXIT_FAILURE;
}
