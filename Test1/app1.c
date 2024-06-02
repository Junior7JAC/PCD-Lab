/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Test Nivel B, app1.c, R3
    * Acest program este un server care asculta pe un socket de tip AF_UNIX.
    * Serverul poate fi conectat de un client care va trimite un mesaj.
    * Am tratarea erorilor si situatiilor limita, precum:
    * -- eroare la crearea socket-ului
    * -- eroare la bind
    * -- eroare la listen
    * -- eroare la accept
 */

#include <stdio.h> // librarie pentru I/O
#include <stdlib.h> // librarie pt exit
#include <unistd.h> // librarie pt fork, pipe
#include <sys/socket.h> // librarie pt socket
#include <sys/un.h> // librarie pt struct sockaddr_un
#include <fcntl.h> // librarie pt open
#include <string.h> // librarie pt memset, strncpy

#define SOCKET_PATH "/tmp/ux.sock" // calea catre socket

int main() {
    int sockfd, client_sock, len;
    struct sockaddr_un addr;
    char buffer[1024];
    pid_t pid;
    // creare socket
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un)); // initializare structura sockaddr_un
    addr.sun_family = AF_UNIX; // setare tip socket
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1); // copiere cale socket

    // stergere socket daca exista
    unlink(SOCKET_PATH);
    // bind socket
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // ascultare socket
    if (listen(sockfd, 5) == -1) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // creare proces copil
    pid = fork();
    if (pid == -1) {
        perror("fork");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // proces copil
    if (pid == 0) {
        int hosts_fd = open("/etc/hosts", O_RDONLY);
        if (hosts_fd == -1) {
            perror("open /etc/hosts");
            exit(EXIT_FAILURE);
        }
        if (dup2(hosts_fd, STDIN_FILENO) == -1) { // redirectare stdin
            perror("dup2 stdin");
            close(hosts_fd);
            exit(EXIT_FAILURE);
        }
        close(hosts_fd);

        // conectare la socket
        if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) { // creare socket
            perror("child socket");
            exit(EXIT_FAILURE);
        }
        if (connect(client_sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) { // conectare la socket
            perror("connect");
            close(client_sock);
            exit(EXIT_FAILURE);
        }

        // redirectare stdout
        if (dup2(client_sock, STDOUT_FILENO) == -1) { // redirectare stdout
            perror("dup2 stdout");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
        close(client_sock); // inchidere socket

        execlp("tail", "tail", "-n", "3", NULL); // executie tail
        perror("execlp"); // eroare la execlp
        exit(EXIT_FAILURE);

    } else { // proces parinte
        client_sock = accept(sockfd, NULL, NULL);
        if (client_sock == -1) { // acceptare conexiune
            perror("accept");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // citire din socket si scriere pe stdout
        while ((len = read(client_sock, buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, len);
        }

        close(client_sock); // inchidere socket
        close(sockfd); // inchidere socket
    }

    return 0;
}

/**
 * Exemple de compilare si rulare a programului
 * gcc app1.c -o app1 ---> COMPILARE
 * ./app1 ----> RULARE
 *
 *junior@junior-pc:~/Desktop/PCD/Test$ gcc app1.c -o app1
junior@junior-pc:~/Desktop/PCD/Test$ ./app1
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters

*/
