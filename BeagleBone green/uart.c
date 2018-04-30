//Reference: https://en.wikibooks.org/wiki/Serial_Programming/termios

//Filename : uart.c
//Author : Nikhil and Vipraja
//This file acts as the main file on beaglebone side which creates the communcation, logger and API function

#include "uart.h"
#include "string.h"
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include "userled.h"
#include "socket_task.h"

struct termios *my_term;

typedef enum
{
    Alive = 0,
    Dead = 1,
}command_enum;

pthread_t read_thread;
int read_thread_check;

pthread_t write_thread;
int write_thread_check;

pthread_t api_thread;
int api_thread_check;

pthread_t heartbeat_thread;
int heartbeat_check;

FILE * fptr;
pthread_mutex_t pmutex;

char * uart_driver = "/dev/ttyO4";
//char * spi_driver = "/dev/SSIDEV0";
char log_name[50];
extern int fd;

typedef struct
{
	float data;
	int TaskID;
	int alert;
}message;

void uart_setup();

//void spi_setup();

//void spi_read();

void termios_setup(struct termios * my_term, int descriptor);

void read_byte(int fd, char * received);

void * read_thread_func()
{
	char buffer[1024];
	fptr = fopen(log_name, "w");
	char recv = 'a';
	message my_data;
	int retval;
	int command;
	int sock;
	while(1)
	{
		fptr = fopen(log_name, "a");
		pthread_mutex_lock(&pmutex);

		retval = read(fd, &my_data, sizeof(message));

		int sock;			//This part is heartbeat check
		struct sockaddr_in server;
		char buff[1024];
		struct hostent *hp;
	
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
	server.sin_port = htons(6006);
	
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connection failes");
		exit(1);
	}

	int command = Alive;
	
	
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		perror("Send failed");
		exit(1);
	}
	close(sock);

		if(retval > 0 && my_data.TaskID != 0)		//checks the recived data
		{
			if(my_data.alert == 2)
			{
				printf("Connection Lost to sensor. Terminating Task");
				fprintf(fptr, "Connection Lost to sensor. Terminating Task");
				exit(-1);
			}
			printf("Task Id: %d \n", my_data.TaskID);
			fprintf(fptr, "Task Id: %d \n", my_data.TaskID);
			
			printf("Data: %f \n", my_data.data);			
			fprintf(fptr, "Data: %f \n", my_data.data);

			printf("Alert: %d \n", my_data.alert);
			fprintf(fptr, "Alert: %d \n", my_data.alert);
			
			char msg_data_string[100];			//alerts below
			if(my_data.alert == 1 && my_data.TaskID == 1)
			{
			strcpy(msg_data_string, "ALERT RECEIEVED FROM GAS SENSOR \n");
			printf("%s", msg_data_string);
			fprintf(fptr, "%s", msg_data_string);
			userLED(3,1);
			}

			if(my_data.alert == 1 && my_data.TaskID == 2)
			{
			strcpy(msg_data_string, "ALERT RECEIEVED FROM FLAME SENSOR \n");
			printf("%s", msg_data_string);
			fprintf(fptr, "%s", msg_data_string);
			userLED(2,1);
			}

			if(my_data.alert == 1 && my_data.TaskID == 3)
			{
			strcpy(msg_data_string, "ALERT RECEIEVED FROM TEMPERATURE SENSOR \n");
			printf("%s", msg_data_string);
			fprintf(fptr, "%s", msg_data_string);
			userLED(3,1);
			}

			fprintf(fptr, "%s", "\n");
			printf("\n");
		}

		pthread_mutex_unlock(&pmutex);
		fclose(fptr);
	}
	command = Dead;				//Task Dead
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		userLED(3,1);
		perror("Send failed");
		printf("Some thread dead, Terminating process\n");
	}
}

void * write_thread_func()			//write to UART
{		
	int i;
	char data[3] = "Nik";
        i = write(fd, &data, sizeof(data));
	printf("%d", i);
}

void * api_thread_func()			//API function for external task
{
	int command;
	int sock;
		struct sockaddr_in server;
		char buff[1024];
		struct hostent *hp;
	
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
	server.sin_port = htons(6006);
	
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connection failes");
		exit(1);
	}

	command = Alive;
	
	
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		perror("Send failed");
		exit(1);
	}
	close(sock);
	socket_server();

	command = Dead;						//Task Dead
	if(send(sock, (void*)&command, sizeof(command), 0) < 0)
	{
		userLED(3,1);
		perror("Send failed");
		printf("Some thread dead, Terminating process\n");
	}
}

void * hb_thread_func()					//checks the heartbeat functionality of other tasks
{
	int sock;
	struct sockaddr_in check_server;
	int mysock;
	char buff[1024];
	int rval;
	int flag = 0;
	//create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("Failed to create a socket");
		exit(1);
	}
	
	check_server.sin_family = AF_INET;
	check_server.sin_addr.s_addr = INADDR_ANY;
	check_server.sin_port = htons(6006);
	
	//bind
	if(bind(sock, (struct sockaddr *)&check_server, sizeof(check_server)) < 0)
	{
		perror("Didn't bind");
		exit(1);
	}

	//Listen
	if(listen(sock, 5) < 0)
	{
		perror("Listening error");
		exit(1);
	}

	//Accept
	while(1)
	{
	mysock = accept(sock, (struct sockaddr *)0, 0);
	if(mysock == -1)
	{
		perror("Accept failed");
		exit(1);
	}
	int incoming;
	
	int data_in = read(mysock,&incoming,sizeof(incoming));
     
	if (data_in < 0)
	{ 
		perror("Error reading");
		exit(1);
	}

	if(incoming  == 2)
	{
		printf("Temp task Dead\n");
		break;
	}

	}
	exit(1);
}

void uart_setup()				//uart driver setup
{
    fd = open(uart_driver, O_RDWR, O_SYNC, O_NOCTTY);
    if(fd < 0)
    {
	perror("Error opening uart driver");
    }
    
    my_term  = (struct termios *)malloc(sizeof(struct termios));
    
    termios_setup(my_term, fd);
}

void termios_setup(struct termios * my_term, int descriptor)
{
    tcgetattr(descriptor, my_term);
    my_term->c_iflag &= ~(IGNBRK | ICRNL | INLCR | PARMRK | ISTRIP | IXON);
    my_term->c_oflag &= ~OPOST;
    my_term->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    my_term->c_cflag |= CS8 | CLOCAL;
   
    cfsetispeed(my_term, B115200);
    cfsetospeed(my_term, B115200);

    if(tcsetattr(descriptor, TCSAFLUSH, my_term) < 0)
    {
        perror("ERROR in set attr\n");
    }
}

void read_byte(int fd,char *received)
{
    if(read(fd, received, 1)<0)
    {
           perror("Read failed"); 
	userLED(3,1);   
    }
}

/*void spi_setup()				//spi driver setup
{
    fd = open(spi_driver, O_RDWR, O_SYNC, O_NOCTTY);
    if(fd < 0)
    {
	perror("Error opening spi driver");
    }
}

void spi_read()
{
	int n;
	n =read(fd,&rec,sizeof(rec));
	if(fd < 0)
      {
	   perror("Error reading spi driver");
      }

}*/



int main(int argc, char *argv[])
{
    uart_setup();
    sleep(1);
    char recv;
    int rec_data;
    char received_string[100];
    printf("Starting things\n");

    memset(log_name, '\0', sizeof(log_name));
    strncpy(log_name, argv[1], strlen(argv[1]));
    //fptr = fopen(log_name, "w");

    int i;
    int j,k=0;
    int flag1, flag2 = 0;
    message msg_struct;

    read_thread_check = pthread_create(&read_thread, NULL, read_thread_func, NULL);
	if(read_thread_check)
	{
		perror("Error creating read thread");
		userLED(3,1);
		exit(-1);
	}

    /*write_thread_check = pthread_create(&write_thread, NULL, write_thread_func, NULL);
	if(write_thread_check)
	{
		perror("Error creating read thread");
		exit(-1);
	}*/

    api_thread_check = pthread_create(&api_thread, NULL, api_thread_func, NULL);
	if(api_thread_check)
	{
		perror("Error creating read thread");
		userLED(3,1);
		exit(-1);
	}

    heartbeat_check = pthread_create(&heartbeat_thread, NULL, hb_thread_func, NULL);
	if(heartbeat_check)
	{
		perror("Error creating read thread");
		userLED(3,1);
		exit(-1);
	}

    printf("Threads created\n");
    //fptr = fopen("log_name", "a");

    //while(1)
    {
    	//do
    	//{
		//fptr = fopen(log_name, "a");
    		//read_byte(fd, &recv);
		//fprintf(fptr, "%c", recv);
		//read(fd, &msg_struct, sizeof(msg_struct));
		//printf("%d", msg_struct.data);
		//printf("%c", recv);
		
		/*if(recv == 'C')
		{
			flag1=1;
			k=j;
			j++;
			//printf("C Detected");
		}
		if(recv == 'O')
		{
			flag2 = 1;
			k++;
			if(j==k)
			{
				printf("O Detected");
			}
		}
		if(flag2 == 1)
			printf("ALERT DETECTED");
		flag1 = flag2 = 0;
		rec_data = (int)recv;
		fclose(fptr);*/
    	//}while(rec_data != 0);
    }
    //while(1)
    {
    //char data[3] = "Nik";
    //i = write(fd, &data, sizeof(data));
//	printf("%d", i);
	
    //close(fd);
    //fclose(fptr);
	pthread_join(read_thread, NULL);
	//pthread_join(write_thread, NULL);
	//pthread_join(api_thread, NULL);
	//pthread_join(heartbeat_thread, NULL);
    }
}
