//Reference: https://en.wikibooks.org/wiki/Serial_Programming/termios

#include "uart.h"

struct termios *my_term;

char * uart_driver = "/dev/ttyO4";

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
    int retval;

    if(retval = read(fd, received, 1)>0)
    {
    	if (retval < 0)
            perror("Read failed");    
    }
    else
    {
        printf("Cannot read");
    }
}


int main()
{
    uart_setup();
    sleep(1);
    char recv;
    int rec_data;
    char received_string[100];

    int i;
    int j,k=0;
    int flag1, flag2 = 0;
    message msg_struct;

    //while(1)
    /*{
    	do
    	{
    		read_byte(fd, &recv);
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
    	}while(rec_data != 0);
    }*/
    //while(1)
    {
    char data[3] = "Nik";
    i = write(fd, &data, sizeof(data));
	printf("%d", i);
    close(fd);
    }
}
