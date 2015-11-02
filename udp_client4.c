#include "headsock.h"

void str_cli1(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, int *len);                

int main(int argc, char *argv[])
{
	int len, sockfd;
	struct sockaddr_in ser_addr;
	char **pptr;
	struct hostent *sh;
	struct in_addr **addrs;
	FILE *fp;

	if (argc!= 2)
	{
		printf("parameters not match.");
		exit(0);
	}

	if ((sh=gethostbyname(argv[1]))==NULL) {             //get host's information
		printf("error when gethostbyname");
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);             //create socket
	if (sockfd<0)
	{
		printf("error in socket");
		exit(1);
	}

	addrs = (struct in_addr **)sh->h_addr_list;
	printf("canonical name: %s\n", sh->h_name);
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);			//printf socket information
	switch(sh->h_addrtype)
	{
		case AF_INET:
			printf("AF_INET\n");
		break;
		default:
			printf("unknown addrtype\n");
		break;
	}

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(MYUDP_PORT);
	//copy the first address
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
	bzero(&(ser_addr.sin_zero), 8);
	
	
	if((fp = fopen ("myfile.txt","r+t")) == NULL)
	{
		printf("File doesn't exit\n");
		exit(0);
	}
	
	//while(1){
		str_cli1(fp, sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in), &len);   // receive and send
	//}
	
	close(sockfd);
	exit(0);
}

void str_cli1(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, int *len)
{
	
	char *buf;
	long lsize, ci;
	char sends[DATALEN+1];
	struct ack_so ack;
	int n, slen;
	
	struct sockaddr_in addrR;
	socklen_t lenR;
	
	ci = 0;

	char identifier = '1';

	fseek (fp , 0 , SEEK_END);
	lsize = ftell (fp);
	rewind (fp);
	printf("The file length is %d bytes\n", (int)lsize);
	printf("the packet length is %d bytes\n",DATALEN);

	// allocate memory to contain the whole file.
	buf = (char *) malloc (lsize);
	if (buf == NULL) exit (2);

  	// copy the file into the buffer.
	fread (buf,1,lsize,fp);
	
	/*** the whole file is loaded in the buffer. ***/
	buf[lsize] ='\0';									//append the end byte
	//gettimeofday(&sendt, NULL);							//get the current time
	
	while(ci<= lsize)
	{
		if ((lsize+1-ci) <= DATALEN)
			slen = lsize+1-ci;
		else 
			slen = DATALEN;
			
		memcpy(sends, (buf+ci), slen);
		sends[slen] = identifier;
		
		//Keep retrying
		while ((n = sendto(sockfd, &sends, slen+1, 0, addr, addrlen)) == -1){
			printf("send error!");								//send the data
		}
		
		//Waiting for acknowledgement -- resend will happen here
		//force stop after retry several times as server may already end
		if ((n=recvfrom(sockfd, &ack, 2, 0, (struct sockaddr *)&addrR, &lenR)) == -1) {      //receive the packet
			printf("error receiving");
		
		}
		
		//ACK
		if (ack.num == 1 && ack.len == 0){
			printf("Received acknowledgement\n");
			
			ci += slen;
			printf("Sent %ld\n", ci);
			
			if (identifier == '1'){
				identifier = '0';
			}
			else{
				identifier = '1';
			}
		}
		
		//NACK
		else if(ack.num == 0 && ack.len == 0){
			
		} 
		
		else{
			
		}
		
	}
	
}
