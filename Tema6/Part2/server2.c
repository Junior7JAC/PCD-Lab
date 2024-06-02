/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 6.1
    * Acest program reprezinta un server TCP care primeste comenzi de la un client TCP si trimite raspunsuri inapoi.
    * Comenzile acceptate sunt:
    * - "uptime" pentru a obtine timpul de functionare al serverului
    * - "stats" pentru a obtine statistici despre server
    * - "cmd:comanda" pentru a executa o comanda shell si a trimite output-ul catre client
    * Am tratat erorile si situatiile limita, precum:
    * - tratarea semnalului SIGINT pentru deconectare
    * - tratarea cazului in care serverul primeste comanda "quit"
    * - tratarea cazului in care serverul primeste o comanda invalida
    * - tratarea cazului in care serverul se inchide
 */
#include <stdio.h> // printf, scanf
#include <stdlib.h> //standard i/o
#include <string.h> //strcmp
#include <sys/socket.h> //pentru a crea si trimite socket-uri de retea locala
#include <sys/un.h> // pt structuri si constante pentru lucrul cu socket-uri de retea locala
#include <unistd.h> // pt functii pentru lucrul cu fisiere si directionare
#include <signal.h> // pt semnale
#include <sys/wait.h> //pt wait

#define SOCKET_PATH "/tmp/server2.socket"
#define BUFFER_SIZE 2048

volatile sig_atomic_t keep_running = 1; // flag pentru a mentine serverul in functiune

void sig_handler(int _) {
    (void)_;
    keep_running = 0; // se seteaza flag-ul pentru a opri serverul
}

// trimite raspuns catre client
void send_response(int sockfd, const char *message) {
    send(sockfd, message, strlen(message), 0);
}

// functia adapted custom system preluata din tema3 si adaptata
int adapted_custom_system(int sockfd, const char *command) {
    int pipefds[2];
    pipe(pipefds);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        // redirectioneaza stdout si stderr catre pipe
        close(pipefds[0]);
        dup2(pipefds[1], STDOUT_FILENO);
        dup2(pipefds[1], STDERR_FILENO);
        close(pipefds[1]);

        // executa comanda in procesul copil
        execlp("/bin/sh", "sh", "-c", command, NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        // inchide capatul de scriere al pipe-ului in parinte
        close(pipefds[1]);

        // citește iesirea comenzii din procesul copil
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read = read(pipefds[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // termina sirul cu null
            send_response(sockfd,buffer); // trimite iesirea la client
        } else {
            send_response(sockfd,"Failed to capture command output");
        }
        close(pipefds[0]);

        // asteapta ca procesul copil sa se termine
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}

// functia pentru a gestiona cererea de uptime de la client
void handle_uptime_request(int sockfd) {
    // citeste uptime-ul din /proc/uptime
    double uptime_seconds;
    FILE *uptime_file = fopen("/proc/uptime", "r");
    if (uptime_file == NULL) {
        send_response(sockfd, "Failed to get uptime");
        return;
    }
    fscanf(uptime_file, "%lf", &uptime_seconds);
    fclose(uptime_file);

    int days = (int)uptime_seconds / (24 * 3600);
    uptime_seconds -= days * (24 * 3600);
    int hours = (int)uptime_seconds / 3600;
    uptime_seconds -= hours * 3600;
    int minutes = (int)uptime_seconds / 60;
    // formateaza informatiile despre uptime
    char uptime_info[BUFFER_SIZE];
    snprintf(uptime_info, BUFFER_SIZE, "Serverul este UP de %dd %dh %dmin", days, hours, minutes);
    send_response(sockfd, uptime_info);
    // trimite informatiile despre uptime la client
}

// functia pentru a gestiona cererea de statistici de la client
void handle_stats_request(int sockfd) {
    char response[2048];
    float load1, load5, load15;
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp != NULL) {
        fscanf(fp, "%f %f %f", &load1, &load5, &load15);
        fclose(fp);
    } else {
        strcpy(response, "Eroare la citirea mediei de incarcare");
        send_response(sockfd,response);
        return;
    }

    // utilizare CPU
    long double a[4], b[4], loadavg;
    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
    fclose(fp);
    sleep(1);  // Așteptați o secundă
    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
    fclose(fp);
    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    int cpu_usage = (int)(loadavg * 100); // Utilizarea CPU în %

    // utilizarea memoriei
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
    int used_memory = total_memory - free_memory - buffers - cached - sreclaimable + shmem;  // aproximare a memoriei utilizate

    snprintf(response, sizeof(response),
             "Medie încărcare: %.2f, %.2f, %.2f  Utilizare CPU: %d%%  Utilizare memorie: Total: %d kB, Utilizat: %d kB",
             load1, load5, load15, cpu_usage, total_memory, used_memory);
    send_response(sockfd,response);
}

// functia pentru a gestiona cererea de executie a comenzii de la client
void handle_command_execution(int sockfd, const char *command) {
    const char *cmd_prefix = "cmd: ";
    // daca comanda este prefixata cu "cmd: "
    if (strncmp(command, cmd_prefix, strlen(cmd_prefix)) == 0) {
        // extrageti comanda reala
        const char *actual_command = command + strlen(cmd_prefix);
        adapted_custom_system(sockfd, actual_command);
    } else {
        // mesaj de eroare daca comanda nu este prefixata cu "cmd: "
        send_response(sockfd, "Eroare: Comanda trebuie prefixata cu 'cmd: '\n");
    }
}

int main() {
    // socket-ul serverului
    int server_sockfd, client_sockfd;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    // handler-ul de semnal pentru SIGINT
    signal(SIGINT, sig_handler);

    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // leaga socket-ul serverului
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH);
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // listen pentru conexiuni de intrare
    listen(server_sockfd, 5);
    while (keep_running) {
        socklen_t client_len = sizeof(struct sockaddr_un);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_sockfd < 0) {
            perror("accept failed");
            continue;
        }
        printf("Client conectat.\n");

        // gestionare cererile clientului intr-un loop
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t num_bytes_read = read(client_sockfd, buffer, BUFFER_SIZE - 1);

            // daca clientul s-a deconectat sau a aparut o eroare
            if (num_bytes_read <= 0) {
                if (num_bytes_read == 0) {
                    printf("Clientul s-a deconectat.\n");
                    exit(EXIT_SUCCESS); // iesire automata din loop daca clientul s-a deconectat / poate fi eliminat
                } else {
                    perror("read failed");
                }
                break;  // iesire din loop in caz de eroare sau deconectare
            }

            buffer[num_bytes_read] = '\0';  // null-termiante
            // optiunea clientului
            if (strncmp(buffer, "uptime", 6) == 0) {
                handle_uptime_request(client_sockfd);
            } else if (strncmp(buffer, "stats", 5) == 0) {
                handle_stats_request(client_sockfd);
            } else {
                handle_command_execution(client_sockfd, buffer);
            }
        }
        // close socket
        close(client_sockfd);
    }
    close(server_sockfd);
    unlink(SOCKET_PATH);
    printf("Serverul a ieșit.\n");
    return 0;
}

/** Exemple de compilare si rulare a programului
 * gcc server2.c -o server2
 * ./server2 --> pentru a rula serverul
 *
 *junior@junior-pc:~/Desktop/PCD/Tema6/Part2$ ./server2
Client conectat.
Clientul s-a deconectat.
junior@junior-pc:~/Desktop/PCD/Tema6/Part2$

*/