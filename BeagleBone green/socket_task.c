/*
@file - socket_task.c
@brief - Includes all the functions for socket server
@author - Nikhil Divekar & Vipraja Patil
*/

#include "socket_task.h"
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "uart.h"

#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256

int fd;

typedef struct
{
	float data;
	int TaskID;
	int alert;
}message;

void socket_server()
{
	int sock;
	struct sockaddr_in server, client;
	int mysock;
	char buff[1024];
	int rval;

	//create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("Failed to create a socket");
		exit(1);
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(6011);
	
	//bind
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
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
        printf("Message: %d \n", incoming);

	int i;

	if(incoming == 1)
	for(i = 0; i< 3; i++)
	{
		message my_data;
		read(fd, &my_data, sizeof(message));
		if(my_data.TaskID != 1)
			continue;
		printf("External request serviced");
		//if(my_data.TaskID == 1)
		{
		printf("Task Id: %d \n", my_data.TaskID);
		//fprintf(fptr, "Task Id: %d \n", my_data.TaskID);
			
		printf("Data: %f \n", my_data.data);			
		//fprintf(fptr, "Data: %f \n", my_data.data);

		printf("Alert: %d \n", my_data.alert);
		//fprintf(fptr, "Alert: %d \n", my_data.alert);

		printf("\n");
		}

		float value = my_data.data;
		int data_out = send(mysock, (void*)&value, sizeof(value), 0);
		if (data_out < 0)
		{
			perror("Error writing");
			exit(1);
		}
		printf("Data sent back");

		break;
	}

	if(incoming == 2)
	for(i = 0; i< 2; i++)
	{
		message my_data;
		read(fd, &my_data, sizeof(message));
		if(my_data.TaskID != 2)
			continue;
		printf("External request serviced");
		
		printf("Task Id: %d \n", my_data.TaskID);
		//fprintf(fptr, "Task Id: %d \n", my_data.TaskID);
			
		printf("Data: %f \n", my_data.data);			
		//fprintf(fptr, "Data: %f \n", my_data.data);

		printf("Alert: %d \n", my_data.alert);
		//fprintf(fptr, "Alert: %d \n", my_data.alert);

		printf("\n");

		float value = my_data.data;
		int data_out = send(mysock, (void*)&value, sizeof(value), 0);
		if (data_out < 0)
		{
			perror("Error writing");
			exit(1);
		}
		printf("Data sent back");

		break;
	}

	if(incoming == 3)
	for(i = 0; i< 2; i++)
	{
		message my_data;
		read(fd, &my_data, sizeof(message));
		if(my_data.TaskID != 3)
			continue;
		printf("External request serviced");
		
		printf("Task Id: %d \n", my_data.TaskID);
		//fprintf(fptr, "Task Id: %d \n", my_data.TaskID);
			
		printf("Data: %f \n", my_data.data);			
		//fprintf(fptr, "Data: %f \n", my_data.data);

		printf("Alert: %d \n", my_data.alert);
		//fprintf(fptr, "Alert: %d \n", my_data.alert);

		printf("\n");

		float value = my_data.data;
		int data_out = send(mysock, (void*)&value, sizeof(value), 0);
		if (data_out < 0)
		{
			perror("Error writing");
			exit(1);
		}
		printf("Data sent back");

		break;
	}
	
	}
	exit(1);
}
