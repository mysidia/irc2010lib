//#ifdef _IRCLIB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#endif

int match(const char*, const char*);
void *oalloc(size_t);

#define SOCKBUFSIZE 1024
#define IRCBUFSIZE 512
#define IRCBUFSZ   512

#define NICKLEN		30
#define HOSTLEN		63
#define USERLEN		8

typedef struct
{
} IrcLibSocketBuf;

typedef struct
{
	int fd;
	struct in_addr addr;

	IrcLibSocketBuf buf;
} Socket;

typedef struct
{
	int topFd;

	Socket *sock;
	Socket *links;
} Listener;

