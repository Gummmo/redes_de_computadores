#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
//codigo que vai ser usado
//maquina de estados 

volatile int STOP=FALSE;

//estados da state machine
typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP_Mac
}State;
//State state = START;
//criar e atribuir os valores as diferentes variveis
unsigned char FLAG = 0x5c;
unsigned char A_SENDER= 0x01;
unsigned char A_RECEIVER = 0x03;
unsigned char C_SET = 0x07;
unsigned char C_UA = 0x06;

//criar a maquina de estados para o SET
void State_Machine_SET(unsigned char Var)
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
            if(Var== 0x07) state = C_RCV;
            
            else if(Var == 0x5c) state = FLAG_RCV;
            
            else state = START;
            break;
        
        case C_RCV:
            
            if(Var == (0x01^0x07)) state = BCC_OK;
            
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

//criar maquina de estados para o UA enviado mas depois so deve estar no transmitter

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
    char valor;
    struct termios oldtio,newtio;
    unsigned char buf[255];

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS10", argv[1])!=0) &&
          (strcmp("/dev/ttyS11", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

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

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

   // while (STOP==FALSE) {       /* loop for input */
    //    res = read(fd,buf,5);   /* returns after 5 chars have been input */
      //  if (res != 5) {
       //     printf("Erro na transmissão do frame\n");
         //   continue;
      /* }
        if (buf[0] != FLAG || buf[4] != FLAG) {
            printf("Frame invalido: começo e final diferente de FLAG\n");
            continue;
        }
        if (buf[1] != A_SENDER || buf[2] != C_SET || buf[3] != (buf[1] ^ buf[2])) {
            printf("Frame invalido: address, control or BCC incorreto\n");
            continue;
        }
        printf("Valido SET frame\n");

        //printar o buffer caso a transmissão seja bem feita
        for(int i=0;i<5;i++)
        {
            printf("buf[%d]: %02X \n",i,buf[i]);
        }
        //enviar a mensagem UA
        
        STOP = TRUE;
    }*/ 
    
    //res = read(fd,buf,1);   // ler um byte

    // maquina Estados Set
    printf("maquina Estados Set\n");
    State state = START;
    while(STOP==FALSE){
        res=read(fd,buf,1);
        printf("0x%02x\n",buf[0]);
        switch (state)
        {
        case START:
            if(buf[0] == 0x5c) state = FLAG_RCV;
            printf("estado 1\n");
            break;

        case FLAG_RCV:
            if(buf[0] == 0x01) state = A_RCV;

            else if(buf[0] == 0x5c) state = FLAG_RCV;
            
            else state = START;   
            break;
        
        case A_RCV:
            if(buf[0]== 0x07) state = C_RCV;
            
            else if(buf[0] == 0x5c) state = FLAG_RCV;
            
            else state = START;
            break;
        
        case C_RCV:
            
            if(buf[0] == (0x01^0x07)) state = BCC_OK;
            
            else if(buf[0] == 0x5c) state = FLAG_RCV;
            
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
    //enviar o UA
    printf("enviar o UA\n");
    buf[0] = FLAG;
    buf[1] = A_RECEIVER;
    buf[2] = C_UA;
    buf[3] = buf[1] ^ buf[2]; // BCC
    buf[4] = FLAG;    
    res = write(fd,buf,5); // envia UA

    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}