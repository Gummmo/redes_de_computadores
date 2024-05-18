#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "linklayer.h"




//////////////////////////////////////
//maquina de estados
volatile int STOP=FALSE;
int state = 0;
#define START 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4
#define MACHINE_STOP 5

int tentativas = 1;

#define FLAG 0x5c
#define A_SENDER 0x01
#define A_answers_Receiver 0x01
#define A_Receiver 0x03
#define A_answers_Sender 0x03

#define C_SET 0x07            
#define C_UA 0x06             
                                
#define C_I0 0x80
#define C_I1 0xC0
#define C_RR0 0x01
#define C_RR1 0x11
#define C_REJ0 0x05
#define C_REJ1 0x15
#define C_DISC 0x10



void atende()                   
{
    tentativas++;
}



// criar set
void maquina_set(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x07) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x07)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}

void maquina_ua_open(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x06) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x06)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}



void maquina_DISC_0(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x10) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x10)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}


void maquina_DISC_1(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x03) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x10) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x03^0x10)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}

void maquina_ua_close(unsigned char char)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x03) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x06) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x03^0x06)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}



//////////////////////////////////////////////////////////////////////////

void maquina_I0(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x80) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x80)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}


void maquina_I1(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0xC0) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0xC0)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state =MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}







void maquina_RR0(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x01) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x01)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}



void maquina_RR1(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(byte==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x11) state = C_RCV;
            else if(byte==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x11)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}


void maquina_REJ0(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x05) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x05)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}



void maquina_REJ1(unsigned char char_lido)
{
    switch (state)
    {
        case START:
            if(char_lido==0x5c) state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(char_lido==0x01) state = A_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;   
            break;
        case A_RCV:
            if(char_lido==0x15) state = C_RCV;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START;
            break;
        case C_RCV:
            if(char_lido==(0x01^0x15)) state = BCC_OK;
            else if(char_lido==0x5c) state = FLAG_RCV;
            else state = START; 
            break;  
        case BCC_OK:  
            if(char_lido==0x5c) state = MACHINE_STOP;
            else state = START;  
            break;
        default:
            printf("exit maquina\n");
            break;
            
    }
}

////
int envia_set(int fd){

    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                       ////0x5c
    buf_aux[1]=A_SENDER;     ////0x01
    buf_aux[2]=C_SET;                ////0x07
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                       ////0x5c
    res = write(fd,buf_aux,5);

    printf("%d bytes written\n", res);

    return NULL;
}

int envia_ua_open(int fd){
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_answers_Receiver;      ////0x01
    buf_aux[2]=C_UA;                         ////0x06
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c
    res = write(fd,buf_aux,5);

    printf("%d bytes written\n", res);

    return NULL;
}

int envia_I_0(int fd, char *buf_aux, int tamanho){
    int res;
    int n_carateres=1,i=5;
    //unsigned char buf2[10];
    char BBC2 = 0x00;
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_SENDER;             ////0x01
    buf_aux[2]=C_I0;                         ////0x80
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c    
    while(i<(tamanho+5)){
        BBC2 = BBC2^buf_aux[i];
        if(buf_aux[i]==0x5c){
            res = write(fd,0x5d,1);
            res = write(fd,0x7c,1);
        }
        else if(buf_aux[i]==0x5d){
            res = write(fd,0x5d,1);
            res = write(fd,0x7d,1);
        }
        else{
            res = write(fd,buf_aux[i],1);
        }
        i++;
    }
    res = write(fd,BBC2,1);
    res = write(fd,0x5c,1);
    printf("%d bytes written\n", res);
    return NULL;
}


int envia_I_1(int fd, char *buf_aux, int tamanho){
    int res;
    int n_carateres=1,i=5;
    //unsigned char buf2[10];
    char BBC2 = 0x00;
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_SENDER;             ////0x01
    buf_aux[2]=C_I1;                         ////0xC0
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c
    while(i<(tamanho+5)){       //porque contando com o header ficam mais 5 unidades de dados
        BBC2 = BBC2^buf_aux[i];
        if(buf_aux[i]==0x5c){
            res = write(fd,0x5d,1);
            res = write(fd,0x7c,1);
        }
        else if(buf_aux[i]==0x5d){
            res = write(fd,0x5d,1);
            res = write(fd,0x7d,1);
        }
        else{
            res = write(fd,buf_aux[i],1);
        }
        i++;
    }
    res = write(fd,BBC2,1);
    res = write(fd,0x5c,1);
    printf("%d bytes written\n", res);
    return NULL;
}

int envia_RR_1(int fd){
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_answers_Receiver;     
    buf_aux[2]=C_RR1;
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}

int envia_RR_0(int fd){
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_answers_Receiver;      
    buf_aux[2]=C_RR0;
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}


int envia_REJ_1(int fd){
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_answers_Receiver;      
    buf_aux[2]=CONTROL_REJ1;
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}

int envia_REJ_0(int fd){
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_answers_Receiver;     
    buf_aux[2]=CONTROL_REJ0;
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}

int envia_ua_close(int fd){
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                               ////0x5c
    buf_aux[1]=A_answers_Sender;        ////0x03
    buf_aux[2]=C_UA;                         ////0x06
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                               ////0x5c
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}

int envia_DISC0(int fd){
    
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                                   ////0x5c
    buf_aux[1]=A_Sender;          ////0x03
    buf_aux[2]=C_DISC;                           ////0x08
    buf2[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                                   ////0x5c
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}


int envia_DISC1(int fd){
    
    int res;
    unsigned char buf_aux[255];
    buf_aux[0]=FLAG;                                   ////0x5c
    buf_aux[1]=A_Receiver;          ////0x03
    buf_aux[2]=C_DISC;                           ////0x08
    buf_aux[3]=buf_aux[1]^buf_aux[2];
    buf_aux[4]=FLAG;                                   ////0x5c
    res = write(fd,buf_aux,5);
    printf("%d bytes written\n", res);
    return NULL;
}





llopen(linkLayer connectionParameters){
    int fd,c, res;
    struct termios oldtio,newtio;
    ////char buf[MAX_PAYLOAD_SIZE];         ////Defino um buffer que comporte o payload todo. Se calhar é MAX_PAYLOAD_SIZE+2 por causa do /n mas não sei se é necessário
    char valor_lido;        //Para comportar os valores lidos da porta série
    state = 0;              
    STOP = FALSE;
    tentativas = 1;

    if ((connectionParameters.serialPort == NULL) ||
         ((strcmp("/dev/ttyS0", connectionParameters.serialPort)!=0) &&
          (strcmp("/dev/ttyS1", connectionParameters.serialPort)!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(NOT_DEFINED);      //NOT_DEFINED (-1) porque as portas não coincidem com as que deviam
    }
    
    
    fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY );
    if (fd < 0) { 
        perror(connectionParameters.serialPort); 
        exit(-1); 
        }

    if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;                                  /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;                                  /* blocking read until 5 chars received */


    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    if(connectionParameters.role==0)
    {
        (void) signal(SIGALRM, atende);  
            alarm(connectionParameters.timeOut);                                        
            SET(fd);

            while(tentativas<connectionParameters.numTries && STOP==FALSE){
                        
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }
                
                maquina_ua_open(valor_lido);
                printf("Estado: %d\n",state);
                if(state==MACHINE_STOP){
                    STOP=TRUE;
                    state=START;
                    alarm(0);           
                    envia_ua_close(fd);       
                }
                else return -1;
                printf(":%x\n", valor_lido);                                     
            }
    }
                     
    if(connectionParameters.role==1)
        {
            while(tentativas<connectionParameters.numTries && STOP==FALSE)
            {
                
            res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
            if (res < 0) {
                perror("Erro na leitura");
                break;
            }
            maquina_set(valor_lido);
            printf("Estado: %d\n",state);
            if(state==MACHINE_STOP){
                STOP=TRUE;
                state=START;
            }
            else return -1;
            printf(":%x\n", valor_lido);                                    
        }
            //SET
            maquina_ua_open(fd);
        }
    if(tentativas==connectionParameters.numTries){

        printf("Erro ao estabelecer a ligacao.\n")
        return -1;
    }
    return 1;           
}

// Closes previously opened connection; if showStatistics==TRUE, link layer should print statistics in the console on close
int llclose(linkLayer connectionParameters, int showStatistics){
    state = 0;         
    int res;
    char valor_lido;                  
    STOP = FALSE;
    tentativas = 1;
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { 
        perror(argv[1]); 
        exit(-1); 
        }
    if(connectionParameters.role == 0){    
        (void) signal(SIGALRM, atende);  
        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        
            alarm(connectionParameters.timeOut);              
            envia_DISC0(fd);

            while(STOP==FALSE){
                            
                res = read(fd,&valor_lido,1);         
                
                if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }
                
                maquina_DISC_1(valor_lido);
                printf("Estado: %d\n",state);
                if(state==MACHINE_STOP){
                    STOP=TRUE;
                    state=START;
                    
                    alarm(0);           
                    
                } 
                printf(":%x\n", valor_lido);                
               
            }
        }
    }

    STOP=FALSE;
    if(connectionParameters.role == 1){
        while(STOP==FALSE){
                        
            res = read(fd,&valor_lido,1);         
            maquina_DISC_0(valor_lido);
            printf("Estado: %d\n",state);
            if(state==MACHINE_STOP){
                STOP=TRUE;
                state=START;
            } 
            printf(":%x:%d\n", valor_lido, res);                
        }


        ////////////
        (void) signal(SIGALRM, atende);  


        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        
        ////////////        
            alarm(connectionParameters.timeOut); 
            maquina_DISC_1(fd);

            while(STOP==FALSE){
                            
                res = read(fd,&valor_lido,1);         
                if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }
                maquina_ua_close(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    STOP=TRUE;
                    state=START;
                    ///////////////
                    alarm(0);           //Para parar o alarme
                } 
                printf(":%x:%d\n", valor_lido, res);                
            }
        }
    }
    sleep(1);                         
    if(tentativas==connectionParameters.numTries){

        printf("Erro ao fechar a ligacao.\n")
        return -1;
    }
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    close(fd);
    return 1;    
}
 llwrite(char* buf, int bufSize)
 {

 }