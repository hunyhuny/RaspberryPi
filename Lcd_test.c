
#include <wiringPi.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <lcd.h>

#define LED1 7    //Green  gpio4
#define LED2 16 // Yellow gpio15
#define LED3 1   // Red       gpio18


const char *DEVICE= "/dev/ttyACM0";	
const unsigned BAUDRATE = B9600;	

                                                                                                                                            
int open_serial() {
	int fd;
    	struct termios oldtio, newtio;
	
	fd = open(DEVICE, O_RDWR | O_NOCTTY );
	if (fd <0) {
          printf("Serial %s  Device Open Error\n", DEVICE);
          exit(1);
	}
	
    	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;   		// inter-character timer unused
	newtio.c_cc[VMIN] = 1;    		// blocking read until 8 chars received
	
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);
	
	return fd;
}

int main() {
	int n;
	int c=0;
	int lcd;
	char ch[128];
	int fd_port;				
        int p;					
	int spo2;			
	char read_buf[20];			

	fd_port = open_serial();		// 장치 초기화 및 열기
	
	if(wiringPiSetup()==-1)
	return 1;
	
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);	

	lcd= lcdInit(2,16,4,6,5,4,0,2,3,0,0,0,0);
	sleep(1);
	
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"WElCOME!!");
		delay(1000);
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"Your Finger Test");
		delay(1500);
		lcdClear(lcd);
		
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading   ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading.  ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading.. ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading... ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading.  ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading..  ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading... ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading.  ");
		delay(900);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Loading..  ");
		delay(900);
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"S");
		delay(700);
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"ST");
		delay(700);
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"STA");
		delay(700);
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"STAR");
		delay(700);
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"START");
		delay(1200);

	while (1)
	{
	 	n=read(fd_port,ch, sizeof(ch));
		p=atoi(ch);
		printf("문자->%s 변환->%d", ch,p);
		
		lcdClear(lcd);				
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Handle Test!");
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"Pitch>%d",p);
		if(p<70) {
			digitalWrite(LED3,1);
			digitalWrite(LED2,0);
			digitalWrite(LED1,0);
		}
		else if(p<88){
			digitalWrite(LED3,0);
			digitalWrite(LED2,1);
			digitalWrite(LED1,0);		
		}
		else {
			c++;
			digitalWrite(LED3,0);
			digitalWrite(LED2,0);
			digitalWrite(LED1,1);
			if(c>5){
			lcdClear(lcd);
			lcdPosition(lcd,0,0);
			lcdPrintf(lcd,"Congratulation");
			lcdPosition(lcd,0,1);
			lcdPrintf(lcd,"Pitch>%d  Cure!!",p);
			break;
			}			
		}
//		delay(1);
	}


        close(fd_port);				// 장치 닫기

	return 0;
}
		

