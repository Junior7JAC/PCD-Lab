/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 6.1
    * Acest program reprezinta un client UDP care trimite comenzi catre un server UDP si afiseaza raspunsul primit de la server.
    * Comenzile sunt citite de la tastatura si trimise catre server.
    * Daca comanda este "quit", clientul trimite un mesaj catre server pentru a anunta deconectarea si se inchide.
    * Am tratat erorile si situatiile limita, precum:
    * - inchiderea corecta a socket-ului la deconectare
    * - tratarea semnalului SIGINT pentru deconectare
    * - tratarea cazului in care serverul se inchide
    * - tratarea cazului in care clientul trimite comanda "quit"
    * - tratarea cazului in care clientul trimite comanda "client_disconnect"
 */
#include <stdio.h> // printf, scanf
#include <stdlib.h> // standard i/o
#include <string.h> // strcmp
#include <sys/socket.h> // pentru a crea si trimite socket-uri de retea locala
#include <netinet/in.h> // pt structuri si constante pentru lucrul cu socket-uri de retea locala
#include <arpa/inet.h> // pt htons, inet_addr
#include <unistd.h> // pt functii pentru lucrul cu fisiere si directionare
#include <signal.h> // For signal handling

#define SERVER_PORT 12345 // portul pe care asculta serverul
#define SERVER_IP "127.0.0.1" // adresa IP a serverului
#define BUFFER_SIZE 2048 // dimensiunea maxima a buffer-ului

int sockfd;
struct sockaddr_in servaddr; //adresa serverului

void signal_handler(int sig) {
    // daca semnalul primit este SIGINT
    if (sig == SIGINT) {
        // trimite un mesaj catre server pentru a anunta deconectarea
        const char* msg = "client_disconnect";
        sendto(sockfd, msg, strlen(msg), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        // inchide socket-ul
        close(sockfd);
        printf("\nClient disconnected safely.\n");
        // iese din program
        exit(0);
    }
}

// functie pentru a primi raspunsul de la server
void get_server_response(int sockfd, struct sockaddr_in *servaddr) {
    char buffer[BUFFER_SIZE];
    int n;
    socklen_t len = sizeof(*servaddr);

    n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) servaddr, &len);
    if (n < 0) {
        perror("recvfrom error");
        return;
    }
    buffer[n] = '\0';
    printf("Server: %s\n", buffer);
}

int main() {
    // creaza un socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    // seteaza adresa serverului
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // inregistreaza functia de tratare a semnalului SIGINT
    signal(SIGINT, signal_handler);

    while (1) {
        char buffer[BUFFER_SIZE];
        printf(">>>");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // elimina newline-ul de la final
        // daca comanda este "quit", trimite un mesaj catre server si iese din program
        if (strcmp(buffer, "quit") == 0) {
            sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
            break;
        }
        // trimite comanda catre server
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        // asteapta raspunsul de la server
        get_server_response(sockfd, &servaddr);
    }
    // inchide socket-ul
    close(sockfd);
    return 0;
}
/** Exemple de compilare si rulare a programului
 * gcc client1.c -o client1
 * ./client1 --> pentru a rula clientul
 *
 * junior@junior-pc:~/Desktop/PCD/Tema6/Part1$ ./client1
>>>UPTIME
Server: Invalid command
>>>uptime
Server: Server is UP for 0d 2h 38min
>>>stats
Server: Load Avg: 1.54, 1.37, 1.30  CPU Usage: 4%  Memory Usage: Total: 12331700 kB, Used: 3264408 kB
>>>cmd: ls
Server: client1
client1.c
server1
server1.c

>>>cmd: tree
Server: .
├── client1
├── client1.c
├── server1
└── server1.c

0 directories, 4 files

>>>quit
junior@junior-pc:~/Desktop/PCD/Tema6/Part1$
*/