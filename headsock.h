#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define MYUDP_PORT 5351
#define MAXSIZE 50
#define BUFSIZE 60000
#define DATALEN 500

struct ack_so
{
	uint8_t num;
	uint8_t len;
};

