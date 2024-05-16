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
int estado=0;
#define START 1
#define FLAG_RCV 2
#define A_RCV 3
#define C_RCV 4
#define BCC_OK 5
#define STOP_Mac 6

//State state = START;
//criar e atribuir os valores as diferentes variveis
unsigned char FLAG = 0x5c;
unsigned char A_SENDER= 0x01;
unsigned char A_RECEIVER = 0x03;
unsigned char C_SET = 0x07;
unsigned char C_UA = 0x06;
// criar set
int envia_set(int fd){
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
int envia_UA(int fd){
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

//criar a maquina de estados para o SET
void SET_machine(unsigned char char_lido){
    
    switch (estado)
        {
        case START:
            if(char_lido == 0x5c) estado = FLAG_RCV;
            printf("estado 1\n");
            break;

        case FLAG_RCV:
            if(char_lido == 0x01) estado = A_RCV;

            else if(char_lido == 0x5c) estado = FLAG_RCV;
            
            else estado = START;   
            break;
        
        case A_RCV:
            if(char_lido== 0x07) estado = C_RCV;
            
            else if(char_lido == 0x5c) estado = FLAG_RCV;
            
            else estado = START;
            break;
        
        case C_RCV:
            
            if(char_lido == (0x01^0x07)) estado = BCC_OK;
            
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
//
int main(int argc, char** argv)
{
    int fd,c, res;
    char char_lido;
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
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

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
    printf("Maquina Estados Set\n");
    State state = START;
    while(STOP==FALSE){
        res=read(fd,&char_lido,1);
        SET_machine(char_lido);
        printf("Estado: %d\n",estado);
        printf("0x%02x\n",char_lido);
        if(estado==STOP_Mac)
        {           
            state=START;
            STOP=TRUE;     
        } 
    }
    STOP=FALSE;
    //enviar o UA
    printf("enviar o UA\n");
    envia_UA(fd);
    /*unsigned char buf2[255];
    buf2[0] = FLAG;
    buf2[1] = 0x01; //0x03
    buf2[2] = 0x06; //0x06
    buf2[3] = buf[1] ^ buf[2]; // BCC
    buf2[4] = FLAG;    
    res = write(fd,buf2,5); // envia UA*/

    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}