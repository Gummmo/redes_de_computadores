#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;
//criar e atribuir os valores as diferentes variveis
unsigned char FLAG = 0x5c;
unsigned char A_SENDER= 0x03;
unsigned char A_RECEIVER = 0x01;
unsigned char C_SET = 0x08;
unsigned char C_UA = 0x06;


int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    unsigned char buf[5];

    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS0", argv[1])!=0) &&
          (strcmp("/dev/ttyS1", argv[1])!=0) )) {
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

    while (STOP==FALSE) {       /* loop for input */
        res = read(fd,buf,5);   /* returns after 5 chars have been input */
        if (res != 5) {
            printf("Erro na transmissão do frame\n");
            continue;
        }
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
        //atualizacao do buffer a ser enviado SET->UA
        buf[0] = FLAG;
        buf[1] = A_RECEIVER;
        buf[2] = C_UA;
        buf[3] = buf[1] ^ buf[2]; // BCC
        buf[4] = FLAG;
        res = write(fd,buf,5);
        STOP = TRUE;
    }
    
    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
