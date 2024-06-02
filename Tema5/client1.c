/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 5
    * Acest program demonstreaza utilizarea socket-urilor pentru a crea un server care poate fi accesat de un client.
    * Serverul poate oferi informatii despre timpul curent, utilizatorul curent, poate executa comenzi shell si poate fi oprit de la distanta.
    * Am tratarea erorilor si situatiilor limita, precum:
    * -- erori la crearea socket-ului
    * -- erori la bind
    * -- erori la listen
    * -- erori la accept
    * -- erori la citirea de la client
    * -- erori la scrierea catre client
    * Acesta este fisierul client1.c, care se conecteaza la serverul creat in server1.c.
 */

#include <stdio.h> // pentru printf și scanf
#include <string.h> // pentru strlen
#include <sys/socket.h> //pentru socket
#include <netinet/in.h> // pentru structura INET
#include <unistd.h> // pentru close
#include <arpa/inet.h> // pentru inet_pton

#define SERVER_PORT 12347 //server port
#define SERVER_IP "127.0.0.1" //server io
#define BUFFER_SIZE 2048 // bufferr size

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    // creare socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Could not create socket");
        return 1;
    }

    // setare structura server_addr pentru serverul cu care se va face conexiunea
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // conectare la server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Could not connect");
        return 1;
    }

    // comunicarea cu server-ul
    while (1) {
        memset(buffer, 0, BUFFER_SIZE); //golire buffer
        printf(">>> ");
        scanf("%s", message);

        // trimite mesaj la server
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Could not send message");
            break;
        }

        // primire raspuns de la server
        memset(buffer, 0, BUFFER_SIZE); //golire buffer
        if (recv(sock, buffer, BUFFER_SIZE, 0) < 0) {
            perror("Could not receive response");
            break;
        }

        printf("::: %s :::\n", buffer);

        // comanda pentru inchidere
        if (strcmp(message, "exit") == 0) {
            break;
        }
    }

    // inchidere socket
    close(sock);

    return 0;
}

/**
 * Exemple de compilare si rulare a programului
 * gcc client1.c -o client1 ---> COMPILARE
 * ./client1 ----> RULARE
 *
 *junior@junior-pc:~/Desktop/PCD/Tema5$ ./client1
>>> time
::: Wed Apr  3 15:49:46 2024 :::
>>> user
::: junior :::
>>> salut
::: echo: salut :::
>>> LOL
::: echo: LOL :::
>>> exit
::: echo: exit :::
junior@junior-pc:~/Desktop/PCD/Tema5$

*/