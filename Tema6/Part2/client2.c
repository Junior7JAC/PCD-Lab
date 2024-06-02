/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 6.1
    * Acest program reprezinta un client TCP care trimite comenzi catre un server TCP si afiseaza raspunsurile primite.
    * Comenzile acceptate sunt:
    * - orice comanda care poate fi executata de server
    * - "quit" pentru a deconecta clientul
    * Comenzile sunt trimise catre server, iar raspunsurile sunt afisate in consola.
    * Clientul se deconecteaza la comanda "quit" sau la semnalul SIGINT.
    * Am tratat erorile si situatiile limita, precum:
    * - tratarea semnalului SIGINT pentru deconectare
    * - tratarea cazului in care clientul primeste raspunsul "quit"
    * - tratarea cazului in care clientul primeste un raspuns invalid
    * - tratarea cazului in care clientul se inchide
 */
#include <stdio.h> // printf, scanf
#include <stdlib.h> //standard i/o
#include <string.h> //strcmp
#include <sys/socket.h> //pentru a crea si trimite socket-uri de retea locala
#include <sys/un.h> // pt structuri si constante pentru lucrul cu socket-uri de retea locala
#include <unistd.h>  // pt functii pentru lucrul cu fisiere si directionare
#include <signal.h> // pt semnale

#define SOCKET_PATH "/tmp/server2.socket"
#define BUFFER_SIZE 2048

int sockfd;
// functia de cleanup si exit
void cleanup_and_exit() {
    if (sockfd >= 0) {
        close(sockfd);
    }
    exit(EXIT_SUCCESS);
}
// functia de tratare a semnalului
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Client disconnecting...\n");
        cleanup_and_exit();
    }
}

int main() {
    // variabile pentru socket si structura sockaddr_un
    struct sockaddr_un servaddr;

    // creaza semnalul pentru SIGINT
    signal(SIGINT, signal_handler);

    // creaza socket pentru client
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, SOCKET_PATH, sizeof(servaddr.sun_path) - 1);
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    char input[BUFFER_SIZE]; // citeste comenzi de la tastatura
    while (1) {
        memset(input, 0, BUFFER_SIZE); // reseteaza bufferul
        printf(">>> ");
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) break; // se opreste daca ajunge la end of file
        input[strcspn(input, "\n")] = 0;
        if (strcmp(input, "quit") == 0) {
            break;
        }
        // trimite comanda la server
        write(sockfd, input, strlen(input));
        char buffer[BUFFER_SIZE] = {0};
        // citeste raspunsul de la server
        read(sockfd, buffer, BUFFER_SIZE);
        printf("Server response: %s\n", buffer);
    }
    // cleanup si exit
    cleanup_and_exit();
    return 0;
}
/** Exemple de compilare si rulare a programului
 * gcc client2.c -o client2
 * ./client2 --> pentru a rula clientul
 *
 *junior@junior-pc:~/Desktop/PCD/Tema6/Part2$ ./client2
>>> STATS
Server response: Eroare: Comanda trebuie prefixata cu 'cmd: '

>>> stats
Server response: Medie încărcare: 1.93, 1.76, 1.48  Utilizare CPU: 3%  Utilizare memorie: Total: 12331700 kB, Utilizat: 3276276 kB
>>> uptime
Server response: Serverul este UP de 0d 2h 44min
>>> cmd: ls
Server response: client2
client2.c
server2
server2.c

>>> cmd: tree
Server response: .
├── client2
├── client2.c
├── server2
└── server2.c

0 directories, 4 files

>>> quit
junior@junior-pc:~/Desktop/PCD/Tema6/Part2$

*/