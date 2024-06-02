/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 6.1
    * Acest program reprezinta un server UDP care primeste comenzi de la un client UDP si trimite raspunsuri inapoi.
    * Comenzile acceptate sunt:
    * - "uptime" pentru a obtine timpul de functionare al serverului
    * - "stats" pentru a obtine statistici despre server
    * - "cmd:comanda" pentru a executa o comanda shell si a trimite output-ul catre client
    * Am tratat erorile si situatiile limita, precum:
    * - tratarea semnalului SIGINT pentru deconectare
    * - tratarea cazului in care serverul primeste comanda "quit"
    * - tratarea cazului in care serverul primeste o comanda invalida
 */

#include <stdio.h> // printf, scanf
#include <stdlib.h> //standard i/o
#include <string.h> //strcmp
#include <sys/socket.h> //pentru a crea si trimite socket-uri de retea locala
#include <netinet/in.h> // pt structuri si constante pentru lucrul cu socket-uri de retea locala
#include <unistd.h> // pt functii pentru lucrul cu fisiere si directionare
#include <sys/wait.h> //pt wait
#include <signal.h> // pt semnale

#define PORT 12345 // portul pe care asculta serverul
#define BUFFER_SIZE 2048 // dimensiunea maxima a buffer-ului

volatile sig_atomic_t keep_running = 1; // flag pentru a mentine serverul in functiune

// functia de tratare a semnalului
void sig_handler(int _) {
    (void)_;
    keep_running = 0;
}

// functie pentru a trimite un mesaj catre client
void send_response(int sockfd, struct sockaddr_in *cliaddr, socklen_t clilen, const char *message) {
    sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)cliaddr, clilen);
}

// functie pentru a executa un comand shell si a trimite output-ul catre client
int adapted_custom_system(int sockfd, struct sockaddr_in *cliaddr, socklen_t clilen, const char *command) {
    int pipefds[2];
    pipe(pipefds);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        close(pipefds[0]);
        dup2(pipefds[1], STDOUT_FILENO);
        dup2(pipefds[1], STDERR_FILENO);
        close(pipefds[1]);

        // executa comanda shell
        execlp("/bin/sh", "sh", "-c", command, NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        // inchide capatul de scriere al pipe-ului
        close(pipefds[1]);

        // citeste output-ul comenzii
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read = read(pipefds[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            send_response(sockfd, cliaddr, clilen, buffer); //trimite output-ul catre client
        } else {
            send_response(sockfd, cliaddr, clilen, "Failed to capture command output");
        }
        close(pipefds[0]);

        // asteapta ca procesul copil sa se termine
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}

// functie pentru a trata cererea de uptime
void handle_uptime_request(int sockfd, struct sockaddr_in *cliaddr, socklen_t clilen) {
    double uptime_seconds;
    FILE *uptime_file = fopen("/proc/uptime", "r");
    if (uptime_file == NULL) {
        send_response(sockfd, cliaddr, clilen, "Failed to get uptime");
        return;
    }
    fscanf(uptime_file, "%lf", &uptime_seconds);
    fclose(uptime_file);

    int days = (int)uptime_seconds / (24 * 3600);
    uptime_seconds -= days * (24 * 3600);
    int hours = (int)uptime_seconds / 3600;
    uptime_seconds -= hours * 3600;
    int minutes = (int)uptime_seconds / 60;

    char uptime_info[BUFFER_SIZE];
    snprintf(uptime_info, BUFFER_SIZE, "Server is UP for %dd %dh %dmin", days, hours, minutes);
    send_response(sockfd, cliaddr, clilen, uptime_info);
}

// functie pentru a trata cererea de statistici
void handle_stats_request(int sockfd, struct sockaddr_in *cliaddr, socklen_t clilen) {
    char response[2048];

    // Load averages
    float load1, load5, load15;
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp != NULL) {
        fscanf(fp, "%f %f %f", &load1, &load5, &load15);
        fclose(fp);
    } else {
        strcpy(response, "Error reading load averages");
        send_response(sockfd, cliaddr, clilen, response);
        return;
    }

    // CPU usage
    long double a[4], b[4], loadavg;
    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
    fclose(fp);
    sleep(1);  //asteapta 1 secunda
    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
    fclose(fp);
    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    int cpu_usage = (int)(loadavg * 100); // CPU usage in procente

    // memoria utilizata
    int total_memory, free_memory, buffers, cached, sreclaimable, shmem;
    fp = fopen("/proc/meminfo", "r");
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) sscanf(line, "MemTotal: %d kB", &total_memory);
        else if (strncmp(line, "MemFree:", 8) == 0) sscanf(line, "MemFree: %d kB", &free_memory);
        else if (strncmp(line, "Buffers:", 8) == 0) sscanf(line, "Buffers: %d kB", &buffers);
        else if (strncmp(line, "Cached:", 7) == 0) sscanf(line, "Cached: %d kB", &cached);
        else if (strncmp(line, "SReclaimable:", 13) == 0) sscanf(line, "SReclaimable: %d kB", &sreclaimable);
        else if (strncmp(line, "Shmem:", 6) == 0) sscanf(line, "Shmem: %d kB", &shmem);
    }
    fclose(fp);
    int used_memory = total_memory - free_memory - buffers - cached - sreclaimable + shmem;  // Approximation of used memory

    snprintf(response, sizeof(response),
             "Load Avg: %.2f, %.2f, %.2f  CPU Usage: %d%%  Memory Usage: Total: %d kB, Used: %d kB",
             load1, load5, load15, cpu_usage, total_memory, used_memory);
    send_response(sockfd, cliaddr, clilen, response);
}

// functie pentru a trata cererea de executie a unei comenzi
void handle_command_execution(int sockfd, struct sockaddr_in *cliaddr, socklen_t clilen, const char *command) {
    // Executa comanda si trimite output-ul catre client
    adapted_custom_system(sockfd, cliaddr, clilen, command);
}

int main() {
    // inregistreaza handler-ul de semnal
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // leaga socket-ul la adresa IP si portul specificate
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // loop principal
    while (keep_running) {
        char buffer[BUFFER_SIZE];
        socklen_t len = sizeof(cliaddr);

        // asteapta cereri de la client
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            if (!keep_running) {
                break;  // daca serverul se inchide, iesi din loop
            }
            continue;
        }
        buffer[n] = '\0';  // null-terminate

        // verifica daca clientul a trimis comanda de deconectare
        if (strcmp(buffer, "client_disconnect") == 0) {
            printf("Client disconnected.\n");
            break;
        }
        else if (strcmp(buffer, "quit") == 0) {
            printf("Quit command received, shutting down.\n");
            break;
        }


        // verifica comanda primita si trimite raspunsul corespunzator
        if (strcmp(buffer, "uptime") == 0) {
            handle_uptime_request(sockfd, &cliaddr, len);
            printf("::: Sending current up time to client...\n");
        } else if (strcmp(buffer, "stats") == 0) {
            handle_stats_request(sockfd, &cliaddr, len);
            printf("::: Sending current stats to client...\n");
        } else if (strncmp(buffer, "cmd:", 4) == 0) {
            // comanda de executie
            if (strlen(buffer) > 4) {
                handle_command_execution(sockfd, &cliaddr, len, buffer + 4);
                printf("::: Sending some command output to client...\n");
            }
        } else {
            char error_msg[BUFFER_SIZE] = "Invalid command";
            sendto(sockfd, error_msg, strlen(error_msg), 0, (struct sockaddr *)&cliaddr, len);
        }
    }
    // inchide socket-ul
    printf("Shutting down server...\n");
    close(sockfd);
    return 0;
}

/** Exemple de compilare si rulare a programului
 * gcc server1.c -o server1
 * ./server1 --> pentru a rula serverul
 *
 * junior@junior-pc:~/Desktop/PCD/Tema6/Part1$ ./server1
::: Sending current up time to client...
::: Sending current stats to client...
::: Sending some command output to client...
::: Sending some command output to client...
Quit command received, shutting down.
Shutting down server...
junior@junior-pc:~/Desktop/PCD/Tema6/Part1$

 */
