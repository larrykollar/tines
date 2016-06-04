/* Code by caf, scarfed from stackoverflow.com
 * 'cause I'm lazy^H^H^H^H not into reinventing the wheel.
 * I would have rather used copyfile(), but it's not on Linux.
 * At least this is portable. -LK
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/param.h>
#include <string.h>
#include "file_copy.h"
#include "config.h"
#include "prefs.h"


/* to, from: source & destination files
 * returns: 0 if OK, -1 if problem (check errno)
 */
int cp_file(const char *to, const char *from)
{
    int fd_to, fd_from;
    char *buf = malloc( 4096 );
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0) {
		free( buf );
        return -1;
	}

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
		free( buf );
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
	free( buf );
    return -1;
}
