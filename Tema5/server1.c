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
 */

#include <stdio.h> // pentru printf și scanf
#include <string.h> // pentru strlen
#include <sys/socket.h> // pentru socket
#include <netinet/in.h> // pentru structura INET
#include <unistd.h> // pentru close
#include <time.h> // pentru time

#define PORT 12347 //port ul serverului
#define BUFFER_SIZE 2048 //size ul buffer ului

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // creare socket server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return 1;
    }

    // definirea adresei socket-ului server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind la adresa
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("Bind failed");
        return 1;
    }

    // ascultare pe socket
    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        return 1;
    }

    // acceptare conexiuni
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("Accept");
        return 1;
    }

    // procesare comenzi
    while (1) {
        char echo_msg[BUFFER_SIZE] = {0};
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE - 1);

        if (bytes_read <= 0) {
            perror("read failed or connection closed");
            break;
        }

        buffer[bytes_read] = '\0'; //  null-terminate

        if (strcmp(buffer, "time") == 0) { //daca clientul alege: time
            time_t t = time(NULL);
            char *time_string = ctime(&t);
            time_string[strlen(time_string) - 1] = '\0'; // eliminam newline-ul de la sfarsit
            send(new_socket, time_string, strlen(time_string), 0);
            memset(echo_msg, 0, BUFFER_SIZE); //golire buffer
            printf("::: Sending current time to client...\n");
        } else if (strcmp(buffer, "user") == 0) { //daca clientul alege: user
            char *user = getlogin();
            send(new_socket, user, strlen(user), 0);
            memset(echo_msg, 0, BUFFER_SIZE);
            printf("::: Sending current user to client...\n");
        } else { //orice alta comanda este tratata ca si "echo"
            char echo_msg[BUFFER_SIZE] = {0}; // curata echo_msg buffer
            snprintf(echo_msg, sizeof(echo_msg), "echo: %.*s", BUFFER_SIZE - 10, buffer);
            send(new_socket, echo_msg, strlen(echo_msg), 0);
            memset(echo_msg, 0, BUFFER_SIZE); //golire buffer
            printf("::: Sending echo to client...\n");
        }

        if (strcmp(buffer, "exit") == 0) { //daca clientul alege: exit
            break;
        }
    }

    // inchidere socket-uri
    close(new_socket);
    close(server_fd);

    return 0;
}

/**
 * Exemple de compilare si rulare a programului
 * gcc server1.c -o server1 ---> COMPILARE
 * ./server1 ----> RULARE
 *
 *junior@junior-pc:~/Desktop/PCD/Tema5$ ./server1
::: Sending current time to client...
::: Sending current user to client...
::: Sending echo to client...
::: Sending echo to client...
::: Sending echo to client...
junior@junior-pc:~/Desktop/PCD/Tema5$

NOTA: ACESTA ESTE OUTPUT-UL DIN CLIENT1.C
*/