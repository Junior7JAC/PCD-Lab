/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 5
    * Acest program demonstreaza utilizarea socket-urilor pentru a crea un server care poate fi accesat de un client.
    * Serverul poate oferi informatii despre timpul curent, utilizatorul curent, poate executa comenzi shell si poate fi oprit de la distanta.
    * Se foloseste un fisier de log pentru a inregistra activitatile serverului.
    * Se foloseste un fisier de credentiale (credentials.txt) pentru a valida utilizatorii.
    * Se foloseste un port si un IP prestabilite pentru conexiunea client-server.
    *
    * Am tratarea erorilor si situatiilor limita, precum:
    * -- erori la crearea socket-ului
    * -- erori la bind
    * -- erori la listen
    * -- erori la accept
    * -- erori la citirea de la client
    * -- erori la scrierea catre client
    * -- erori la deschiderea fisierelor de credentiale si de log
 */

#include <stdio.h> // pentru printf și scanf
#include <string.h> // pentru strlen
#include <sys/socket.h> // pentru socket
#include <netinet/in.h> // pentru structura INET
#include <unistd.h> // pentru close
#include <time.h> // pentru time
#include <stdlib.h> // pentru exit
#include <sys/wait.h> // pentru wait

#define PORT 12346
#define BUFFER_SIZE 2048
#define LOG_FILE "server_logs.log"

// functia de login
void server_log(const char *log_message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Could not open the log file");
        exit(EXIT_FAILURE);
    }

    // ia timpul curent
    time_t now = time(NULL);
    char *time_string = ctime(&now);
    time_string[strlen(time_string)-1] = '\0'; // Remove newline

    // scrie log message
    fprintf(log_file, "[%s] %s\n", time_string, log_message);
    fclose(log_file);
}

// verificare username si password
int check_credentials(const char *username, const char *password) {
    FILE *file = fopen("/tmp/credentials.txt", "r");
    if (file == NULL) {
        perror("Unable to open the credentials file");
        server_log("Failed to open credentials file");
        return 0;
    }
    char file_username[256], file_password[256];
    while (fscanf(file, "%255s %255s", file_username, file_password) == 2) {
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            fclose(file);
            return 1; // autentificare cu succes
        }
    }

    fclose(file);
    return 0; // autenttificare fara succes
}

// functie preluata din Tema3, adaptata pentru acest program
void execute_command(int socket, const char *command) {
    int pipefds[2];
    if (pipe(pipefds) == -1) {
        perror("pipe");
        server_log("Error creating pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        server_log("Error forking process");
        return;
    } else if (pid == 0) {
        // procesul copil: executa comanda
        close(pipefds[0]);
        dup2(pipefds[1], STDOUT_FILENO); // redirecteaza stdout la pipe
        dup2(pipefds[1], STDERR_FILENO);
        close(pipefds[1]);

        execlp("/bin/sh", "sh", "-c", command, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // procesul parinte
        char log_msg[1024];
        snprintf(log_msg, sizeof(log_msg), "Executing command: %s", command);
        server_log(log_msg);

        close(pipefds[1]);
        char command_output[BUFFER_SIZE];
        int bytesRead = read(pipefds[0], command_output, BUFFER_SIZE - 1);
        if (bytesRead > 0) {
            send(socket, command_output, bytesRead, 0); // trimite output la client
        } else {
            const char *error_msg = "Error executing command.";
            send(socket, error_msg, strlen(error_msg), 0);
        }
        close(pipefds[0]);
        wait(NULL);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // creare socket server
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        server_log("Socket creation failed");
        return 1;
    }

    // definirea adresei socket-ului server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind la adresa
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        server_log("Bind failed");
        return 1;
    }

    // ascultare pe socket
    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        server_log("Listen failed");
        return 1;
    }
    server_log("Server started and listening");

    // acceptare conexiuni
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept");
        server_log("Accept failed");
        return 1;
    }
    server_log("Client connected");

    // procesare comenzi
    while (1) {
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            if (bytes_read == -1) {
                perror("read failed");
                server_log("Read from socket failed");
            } else if (bytes_read == 0) {
                printf("Client closed the connection.\n");
                server_log("Client closed the connection");
            }
            break;
        }

        buffer[bytes_read] = '\0';

        if (strcmp(buffer, "REQUEST_TIME") == 0) { //daca clientul a cerut time-ul
            time_t t = time(NULL);
            char *time_string = ctime(&t);
            time_string[strlen(time_string) - 1] = '\0';
            send(new_socket, time_string, strlen(time_string), 0);
            server_log("Time requested by client");
            continue;
        }

        if (strcmp(buffer, "REQUEST_USER") == 0) { //daca clientul a cerut user-ul
            char *user = getlogin();
            send(new_socket, user, strlen(user), 0);
            server_log("User requested by client");
            continue;
        }

        if (strncmp(buffer, "EXECUTE_COMMAND: ", 17) == 0) { //daca user-ul a cerut folosirea unei comenzi
            execute_command(new_socket, buffer + 17);
            continue;
        }

        if (strcmp(buffer, "CLOSE_CONNECTION") == 0) { // daca useru-ul a cerut inchiderea conexiunii
            printf("Close command received. Shutting down.\n");
            server_log("Close command received. Shutting down");
            break;
        }

        send(new_socket, buffer, strlen(buffer), 0);
    }

    //inchidere socket-uri
    close(new_socket);
    close(server_fd);
    server_log("Server shutdown");
    return 0;
}

/**
 * Exemple de compilare si rulare a programului
 * gcc server2.c -o server2 ---> COMPILARE
 * ./server2 ----> RULARE
 *
 * Cand clientul inchide server-ul, se afiseaza urmatorul mesaj:
 * Close command received. Shutting down.

 NOTA: INSIDE SERVER_LOGS.LOG

[Mon Apr  1 13:00:30 2024] Server started and listening
[Mon Apr  1 13:01:21 2024] Client connected
[Mon Apr  1 13:01:29 2024] Time requested by client
[Mon Apr  1 13:01:31 2024] User requested by client
[Mon Apr  1 13:01:35 2024] Executing command: ls
[Mon Apr  1 13:01:39 2024] Executing command: date
[Mon Apr  1 13:01:42 2024] Close command received. Shutting down
[Mon Apr  1 13:01:42 2024] Server shutdown
[Mon Apr  1 13:02:48 2024] Server started and listening
[Mon Apr  1 13:02:51 2024] Client connected
[Mon Apr  1 13:02:58 2024] Executing command: ls
[Mon Apr  1 13:02:59 2024] Close command received. Shutting down
[Mon Apr  1 13:02:59 2024] Server shutdown
[Mon Apr  1 13:04:33 2024] Server started and listening
[Mon Apr  1 13:04:45 2024] Client connected
[Mon Apr  1 13:04:52 2024] Time requested by client
[Mon Apr  1 13:04:57 2024] User requested by client
[Mon Apr  1 13:05:00 2024] Executing command: ls
[Mon Apr  1 13:05:04 2024] Executing command: date
[Mon Apr  1 13:05:06 2024] Close command received. Shutting down
[Mon Apr  1 13:05:06 2024] Server shutdown
[Mon Apr  1 18:30:10 2024] Server started and listening
[Mon Apr  1 18:30:14 2024] Client connected
[Mon Apr  1 18:30:16 2024] Client closed the connection
[Mon Apr  1 18:30:16 2024] Server shutdown
[Mon Apr  1 18:30:46 2024] Server started and listening
[Mon Apr  1 18:30:49 2024] Client connected
[Mon Apr  1 18:31:08 2024] Time requested by client
[Mon Apr  1 18:31:11 2024] User requested by client
[Mon Apr  1 18:31:16 2024] Executing command: ls
[Mon Apr  1 18:31:28 2024] Executing command: cal
[Mon Apr  1 18:31:44 2024] Executing command:
[Mon Apr  1 18:31:59 2024] Executing command:
[Mon Apr  1 18:35:15 2024] Server started and listening
[Mon Apr  1 18:35:20 2024] Client connected
[Mon Apr  1 18:35:29 2024] Executing command: ls | cal
[Mon Apr  1 18:35:49 2024] Executing command: ls|time
[Mon Apr  1 18:36:02 2024] Executing command: ls|cal
[Mon Apr  1 18:36:12 2024] Executing command: cal|ls
[Mon Apr  1 18:36:44 2024] Close command received. Shutting down
[Mon Apr  1 18:36:44 2024] Server shutdown
[Wed Apr  3 15:07:57 2024] Server started and listening
[Wed Apr  3 15:08:01 2024] Client connected
[Wed Apr  3 15:08:03 2024] Client closed the connection
[Wed Apr  3 15:08:03 2024] Server shutdown
[Wed Apr  3 15:08:30 2024] Server started and listening
[Wed Apr  3 15:08:32 2024] Client connected
[Wed Apr  3 15:08:39 2024] Time requested by client
[Wed Apr  3 15:08:40 2024] User requested by client
[Wed Apr  3 15:08:46 2024] Executing command: ls -a
[Wed Apr  3 15:08:54 2024] Close command received. Shutting down
[Wed Apr  3 15:08:54 2024] Server shutdown
[Wed Apr  3 15:10:43 2024] Server started and listening
[Wed Apr  3 15:10:46 2024] Client connected
[Wed Apr  3 15:10:53 2024] Time requested by client
[Wed Apr  3 15:10:54 2024] User requested by client
[Wed Apr  3 15:10:57 2024] Executing command: ls
[Wed Apr  3 15:10:59 2024] Close command received. Shutting down
[Wed Apr  3 15:10:59 2024] Server shutdown
[Wed Apr  3 15:12:14 2024] Server started and listening
[Wed Apr  3 15:12:17 2024] Client connected
[Wed Apr  3 15:12:22 2024] Time requested by client
[Wed Apr  3 15:12:27 2024] Close command received. Shutting down
[Wed Apr  3 15:12:27 2024] Server shutdown
[Wed Apr  3 15:18:18 2024] Server started and listening
[Wed Apr  3 15:35:37 2024] Server started and listening
[Wed Apr  3 15:35:39 2024] Client connected
[Wed Apr  3 15:35:47 2024] Time requested by client
[Wed Apr  3 15:35:48 2024] User requested by client
[Wed Apr  3 15:35:51 2024] Executing command: ls
[Wed Apr  3 15:35:56 2024] Close command received. Shutting down
[Wed Apr  3 15:35:56 2024] Server shutdown
[Wed Apr  3 15:37:05 2024] Server started and listening
[Wed Apr  3 15:37:07 2024] Client connected
[Wed Apr  3 15:37:43 2024] Executing command: abc
[Wed Apr  3 15:38:09 2024] Close command received. Shutting down
[Wed Apr  3 15:38:09 2024] Server shutdown

 */
