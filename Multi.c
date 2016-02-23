
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
#include <errno.h>
#include <pthread.h>

#define LED1 7    //Green  gpio4
#define LED2 16 // Yellow gpio15
#define LED3 1   // Red       gpio18
#define	SW	29	/* GPIO21 */
#define	SPKR	27	/* GPIO16 */


const char *DEVICE= "/dev/ttyACM1";	
const unsigned BAUDRATE = B9600;	

pthread_mutex_t ac_lock;
pthread_mutex_t sw_lock;

static int is_run=1;
int cnt =0;

void *acfunction(void *arg){
	if(pthread_mutex_trylock(&ac_lock) != EBUSY){
		handtest();
		pthread_mutex_unlock(&ac_lock);
	}
	return NULL;
}
void *swfunction(void *arg){
	if(pthread_mutex_trylock(&sw_lock) != EBUSY){
		button();
		pthread_mutex_unlock(&sw_lock);
	}
	return NULL;
}
                                                                                                                                    
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
void lcdstart(int lcd){
/*
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"WElCOME!!");
		delay(1000);
*/
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Your Finger Test");
//		delay(1500);
/*
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
*/		
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"START");
		delay(1500);
		lcdClear(lcd);
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"NORMAL Mode=>1");
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"ADDCOUNT Mode=>2");
		

	}
int button(){

	pinMode(SW, INPUT); 
	pinMode(SPKR, OUTPUT); 		
    	softToneCreate(SPKR) ; 			/* 톤 출력을 위한 GPIO 설정 */
   	
	while(is_run){
		if(digitalRead(SW) == LOW) { 
  			 softToneWrite(SPKR, 440); 
			printf("눌림\n");
			cnt++;
			delay(280); 
		}
		else{
			softToneWrite(SPKR, 0); 
			printf("안눌림\n");
			delay(280); 
		}	
	}    
	return 0;
}
int handtest(){
	
	int n;
	int c=0;
	char ch[128];
	char read_buf[20];
	int p;
	int fd_port = open_serial();
 	int lcd= lcdInit(2,16,4,6,5,4,0,2,3,0,0,0,0);
		while(1){
	 	n=read(fd_port,ch, sizeof(ch));
		p=atoi(ch);
		printf("문자->%s 변환->%d", ch,p);
		
	lcdClear(lcd);				
		lcdPosition(lcd,0,0);
		lcdPrintf(lcd,"Handle Test!");
		lcdPosition(lcd,0,1);
		lcdPrintf(lcd,"Pitch>%d",p);
		lcdPosition(lcd,9,1);
		lcdPrintf(lcd,"PUSH %d",cnt);
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
		delay(100);
	}	
		return 0;

}
int kbhit()
{
    struct termios oldt, newt; 			/* 터미널에 대한 구조체 */
    int ch, oldf;

    tcgetattr(0, &oldt); 			/* 현재 터미널에 설정된 정보를 가져온다. */
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); 		/* 정규 모드 입력과 에코를 해제한다. */
    tcsetattr(0, TCSANOW, &newt); 		/* 새로 값으로 터미널을 설정한다. */
    oldf = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK); 	/* 입력을 논블로킹 모드로 설정한다. */

    ch = getchar();

    tcsetattr(0, TCSANOW, &oldt); 		/* 기존의 값으로 터미널의 속성을 바로 적용한다. */
    fcntl(0, F_SETFL, oldf);
    if(ch != EOF) {
        ungetc(ch, stdin); 			/* 앞에서 읽은 위치로 이전으로 포인터를 돌려준다. */
        return 1;
    }

    return 0;
}

int main() {
	
	
	int fd_port,i;					
	pthread_t ptTest, ptSwitch;
	pthread_mutex_init(&ac_lock,NULL);	
	pthread_mutex_init(&sw_lock,NULL);	

			// 장치 초기화 및 열기
	
	if(wiringPiSetup()==-1)
	return 1;
	
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);	

	int lcd= lcdInit(2,16,4,6,5,4,0,2,3,0,0,0,0);
	sleep(1);
	
	lcdstart(lcd);
 	for(i = 0; ;i++) {
		if(kbhit()) {
			switch(getchar( )){
				case '1':	
				pthread_create(&ptTest,NULL,acfunction,NULL);
				break;
				case '2':
				pthread_create(&ptTest,NULL,swfunction,NULL);
				break;
				case 'q':
				goto END;
				break;
				case 'r':
				is_run=0;
				close(fd_port);
				pthread_join(ptTest,NULL);
				pthread_join(ptSwitch,NULL);
	
				pthread_mutex_destroy(&ac_lock);	
				pthread_mutex_destroy(&sw_lock);	
				main();
				break;
			};
		}
 	printf("%20d\t\t\r", i); /* 현재 카운트한 숫자를 출력한다. */
        delay(100); /* 100ms 동안 잠시 쉰다. */
	}
END:
        close(fd_port);				// 장치 닫기
	
	is_run=0;
	
	pthread_join(ptTest,NULL);
	pthread_join(ptSwitch,NULL);

	pthread_mutex_destroy(&ac_lock);	
	pthread_mutex_destroy(&sw_lock);	
	return 0;
}
		

