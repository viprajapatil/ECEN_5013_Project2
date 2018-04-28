//Reference: https://en.wikibooks.org/wiki/Serial_Programming/termios

#include "uart.h"
#include "string.h"

struct termios *my_term;

char * uart_driver = "/dev/ttyO4";
char log_name[50];
int fd;

typedef struct
{
	float data;
	int data_len;
	int TaskID;
	int LogLevel;
	int alert;
}message;

void uart_setup();

void termios_setup(struct termios * my_term, int descriptor);

void read_byte(int fd, char * received);

void uart_setup()
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
    }
}


int main(int argc, char *argv[])
{
    uart_setup();
    sleep(1);
    char recv;
    int rec_data;
    char received_string[100];

    memset(log_name, '\0', sizeof(log_name));
    strncpy(log_name, argv[1], strlen(argv[1]));
    FILE *fptr = fopen(log_name, "w");

    int i;
    int j,k=0;
    int flag1, flag2 = 0;
    message msg_struct;
    //fptr = fopen("log_name", "a");

    //while(1)
    {
    	do
    	{
		fptr = fopen(log_name, "a");
    		read_byte(fd, &recv);
		fprintf(fptr, "%c", recv);
		//read(fd, &msg_struct, sizeof(msg_struct));
		//printf("%d", msg_struct.data);
		printf("%c", recv);
		
		if(recv == 'C')
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
		fclose(fptr);
    	}while(rec_data != 0);
    }
    //while(1)
    {
    //char data[3] = "Nik";
    //i = write(fd, &data, sizeof(data));
//	printf("%d", i);
	
    close(fd);
    //fclose(fptr);
    }
}
