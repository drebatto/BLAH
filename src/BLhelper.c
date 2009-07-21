#include "BLhelper.h"
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>


/*  Read a line from a socket  */

ssize_t
Readline(int sockd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char    c, *buffer;

	buffer = vptr;

	for ( n = 1; n < maxlen; n++ ) {
	
		if ( (rc = read(sockd, &c, 1)) == 1 ) {
			*buffer++ = c;
		}else if ( rc == 0 ) {
			if ( n == 1 ){
				return 0;
			}else{
				break;
			}
		}else {
			if ( errno == EINTR ){
				continue;
			}
			return -1;
		}
	}

	*buffer = 0;
	return n;
}


/*  Write a line to a socket  */

ssize_t
Writeline(int sockd, const void *vptr, size_t n)
{
	size_t      nleft;
	ssize_t     nwritten;
	const char *buffer;

	buffer = vptr;
	nleft  = n;

	/* set write lock */
	pthread_mutex_lock( &writeline_mutex );

	while ( nleft > 0 ) {
		if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
			if ( errno == EINTR ){
				nwritten = 0;
			}else{
				return -1;
			}
		}
		nleft  -= nwritten;
		buffer += nwritten;
	}

	/* release write lock */
	pthread_mutex_unlock( &writeline_mutex );

	return n;
}




