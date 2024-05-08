#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

//estados da state machine
typedef enum {
    START,
    FLAG_SENDER,
    A_SENDER,
    C_SENDER,
    BCC_OK,
    STOP_Mac
}State;
//State state = START;
//
unsigned char FLAG = 0x5c;
unsigned char A_SENDER= 0x01;
unsigned char A_RECEIVER = 0x03;
unsigned char C_SET = 0x07;
unsigned char C_UA = 0x06;

//UA MACHINE
void State_Machine_UA_first(unsigned char Var)
{
    switch (state)
    {
        case START:
            if(Var == 0x5c) state = FLAG_RCV;
            break;

        case FLAG_RCV:
            if(Var == 0x01) state = A_RCV;

            else if(Var == 0x5c) state = FLAG_RCV;
            
            else state = START;   
            break;
        
        case A_RCV:
            if(Var== 0x06) state = C_RCV;
            
            else if(Var == 0x5c) state = FLAG_RCV;
            
            else state = START;
            break;
        
        case C_RCV:
            
            if(Var == (0x01^0x06)) state = BCC_OK;
            
            else if(Var == 0x5c) state = FLAG_RCV;
            
            else state = START; 
            break;  
        case BCC_OK:  
            
            if(Var == 0x5c) state = STOP_Mac;
            
            else state = START;  
            break;
        
        default:
            printf("EXIT MAQUINA\n");
            break;
            
    }
}

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    unsigned char buf[255];
    char valor;
    int i, sum = 0, speed = 0;

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS10", argv[1])!=0) &&
          (strcmp("/dev/ttyS11", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */ // professor tinha colado 1

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    //SET framo a ser enviado 
    buf[0] = FLAG;
    buf[1] = A_SENDER; //x01
    buf[2] = C_SET;    //x07
    buf[3] = buf[1] ^ buf[2]; // BCC
    buf[4] = FLAG;
    
    //enviar o SET 
    printf("enviar o SET frame\n");
    res = write(fd,buf,5);
    printf("%d bytes written\n", res);
    printf("receber o UA\n");
    State state =START;
    while(STOP==FALSE){
        res=read(fd,buf,1);
        printf("0x%02x\n",buf[0]);
        switch (state)
        {
        case START:
            if(buf[0] == 0x5c) state = FLAG_SENDER;
            printf("estado 1 \n");
            break;

        case FLAG_SENDER:
            if(buf[0] == 0x01) state = A_SENDER;

            else if(buf[0] == 0x5c) state = FLAG_SENDER;
            
            else state = START;   
            break;
        
        case A_RCV:
            if(buf[0]== 0x06) state = C_SENDER;
            
            else if(buf[0] == 0x5c) state = FLAG_SENDER;
            
            else state = START;
            break;
        
        case C_RCV:
            
            if(buf[0] == (0x01^0x06)) state = BCC_OK;
            
            else if(buf[0] == 0x5c) state = FLAG_SENDER;
            
            else state = START; 
            break;  
        case BCC_OK:  
            
            if(buf[0] == 0x5c) state = STOP_Mac;
            
            else state = START;  
            break;
        
        default:
            printf("EXIT MAQUINA\n");
            break;
            
        }
    }
    printf("UA RECEBIDA\n");
    sleep(1);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);
    return 0;
}
