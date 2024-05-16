#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE B38400
//#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;
int estado=0;
//estados da state machine
#define START 1
#define FLAG_RCV 2
#define A_RCV 3
#define C_RCV 4
#define BCC_OK 5
#define STOP_Mac 6

//State state = START;
//
unsigned char FLAG = 0x5c; //talvez tenha que colocar #define
unsigned char A_SENDER= 0x01;
unsigned char A_RECEIVER = 0x03;
unsigned char C_SET = 0x07;
unsigned char C_UA = 0x06;

// criar set
int enviar_set(int fd){
    int res;

    unsigned char buf_aux[255];

    buf_aux[0]=FLAG;                       
    buf_aux[1]=A_SENDER; //0x01
    buf_aux[2]=C_SET;                
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                       
    buf_aux[5] = '\n'; // talvez retirar depois
    res = write(fd,buf_aux,5); // envia o SET
    printf("%d bytes written\n", res);
    return NULL;
}
//cria ua
int enviar_UA(int fd){
    int res;

    unsigned char buf_aux[255];

    buf_aux[0]=FLAG;                               
    buf_aux[1]=A_SENDER;      
    buf_aux[2]=C_UA;                         
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               
    buf_aux[5] = '\n';
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}
// criar maquina de estados UA
void UA_machine(unsigned char char_lido){
    
        switch (estado)
        {
        case START:
            if(char_lido == 0x5c) estado = FLAG_RCV;
            printf("estado 1 \n");
            break;

        case FLAG_RCV:
            if(char_lido == 0x01) estado = A_RCV;

            else if(char_lido == 0x5c) estado = FLAG_RCV;
            
            else estado = START;   
            break;
        
        case A_RCV:
            if(char_lido== 0x06) estado = C_RCV;
            
            else if(char_lido == 0x5c) estado = FLAG_RCV;
            
            else estado = START;
            break;
        
        case C_RCV:
            
            if(char_lido == (0x01^0x06)) estado = BCC_OK;
            
            else if(char_lido == 0x5c) estado = FLAG_RCV;
            
            else estado = START; 
            break;  
        case BCC_OK:  
            
            if(char_lido == 0x5c) estado = STOP_Mac;
            
            else estado = START;  
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
    char char_lido;
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
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */ // professor tinha colado 1

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    //SET framo a ser enviado  
    printf("enviar o SET frame\n");
    enviar_set(fd); //enviar o SET 
   /*
   buf[0] = FLAG;
    buf[1] = A_SENDER; //x01
    buf[2] = C_SET;    //x07
    buf[3] = buf[1] ^ buf[2]; // BCC
    buf[4] = FLAG;
   */ 
    printf("receber o UA\n");
    while(STOP==FALSE){
        res=read(fd,&char_lido,1);
        UA_machine(char_lido);
        printf("Estado: %d\n",estado);
        printf("0x%02x\n",char_lido);
        if(estado==STOP_Mac)
        {           state=START;
                    STOP=TRUE;
                    
                    alarm(0);           
        } 
    }
    STOP=FALSE;
    printf("UA RECEBIDA\n");
    sleep(1);
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);
    return 0;
}
