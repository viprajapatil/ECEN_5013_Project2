#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

typedef enum
{
    Gas = 1,
    Flame = 2,
	Humidity = 3,
	Temp = 4
}command_enum;
#define UnitTest
//#define Normal
#ifdef Normal
void main(int argc, char *argv[])
{
	//while(1)
	{
	int sock;
	struct sockaddr_in server;
	char buff[1024];
	struct hostent *hp;
	int command;
	
	printf("List of commands available:\n");
	printf("1. Get CO gas value\n");
	printf("2. Get Flame sensor value\n");
	printf("3. Get Humidity value\n");
	printf("4. Get Temperature\n");
	printf("Enter approprate command number\n");

	scanf("%d", &command);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket failed");
		exit(1);
	}
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if(hp == 0)
	{
		perror("gethost failed");
		exit(1);
	}
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(6001);
	
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connection failes");
		exit(1);
	}

	//int command = Light;
	
	
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		perror("Send failed");
		exit(1);
	}
	printf("Sent : %d \n", command);
	//sleep(1);

	//int mysock;
	//mysock = accept(sock, (struct sockaddr *)0, 0);

	float incoming;
	int data_in = read(sock, &incoming, sizeof(incoming));
        if (data_in < 0)
	{	 
        	perror("Read failed");
		exit(1);
	}
	printf("Data got back: %f", incoming);

	//close(sock);
	//exit(1);
	sleep(1);
	}
}
#endif

#ifdef UnitTest
int gas_value()
{
	{
	int sock;
	struct sockaddr_in server;
	char buff[1024];
	struct hostent *hp;
	int command = 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket failed");
		exit(1);
	}
	server.sin_family = AF_INET;
	hp = gethostbyname("localhost");
	if(hp == 0)
	{
		perror("gethost failed");
		exit(1);
	}
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(6001);
	
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connection fails");
		exit(1);
	}
	
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		perror("Send failed");
		exit(1);
	}
	printf("Sent : %d \n", command);
	
	float incoming;
	int data_in = read(sock, &incoming, sizeof(incoming));
        if (data_in < 0)
	{	 
        	perror("Read failed");
		exit(1);
	}
	printf("Data got back: %f", incoming);
	if(incoming > 3.2 && incoming < 9)
		return 0;
	else
		return 1;
	//close(sock);
	//exit(1);
	sleep(1);
	}
	
}

int flame_value()
{
	{
	int sock;
	struct sockaddr_in server;
	char buff[1024];
	struct hostent *hp;
	int command = 2;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket failed");
		exit(1);
	}
	server.sin_family = AF_INET;
	hp = gethostbyname("localhost");
	if(hp == 0)
	{
		perror("gethost failed");
		exit(1);
	}
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(6001);
	
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connection fails");
		exit(1);
	}
	
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		perror("Send failed");
		exit(1);
	}
	printf("Sent : %d \n", command);
	
	float incoming;
	int data_in = read(sock, &incoming, sizeof(incoming));
        if (data_in < 0)
	{	 
        	perror("Read failed");
		exit(1);
	}
	printf("Data got back: %f", incoming);
	if(incoming > 3.2 && incoming < 9)
		return 0;
	else
		return 1;
	//close(sock);
	//exit(1);
	sleep(1);
	}
}

int temp_value()
{
	{
	int sock;
	struct sockaddr_in server;
	char buff[1024];
	struct hostent *hp;
	int command = 3;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket failed");
		exit(1);
	}
	server.sin_family = AF_INET;
	hp = gethostbyname("localhost");
	if(hp == 0)
	{
		perror("gethost failed");
		exit(1);
	}
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(6001);
	
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connection failes");
		exit(1);
	}
	
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		perror("Send failed");
		exit(1);
	}
	printf("Sent : %d \n", command);
	
	float incoming;
	int data_in = read(sock, &incoming, sizeof(incoming));
        if (data_in < 0)
	{	 
        	perror("Read failed");
		exit(1);
	}
	printf("Data got back: %f", incoming);
	if(incoming > 3.2 && incoming < 9)
		return 0;
	else
		return 1;
	//close(sock);
	//exit(1);
	sleep(1);
	}
}

#endif





