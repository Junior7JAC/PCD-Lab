/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Test Nivel C, app1.c
 */


#include <stdio.h> // librarie pt functii de intrare/iesire
#include <stdlib.h> // librarie pt functii standard
#include <poll.h> // librarie pt polling
#include <sys/stat.h> // librarie pt informatii despre fisiere
#include <pthread.h> // librarie pt thread-uri
#include <unistd.h> // librarie pt functii de sistem
#include <sys/types.h> // librarie pt tipuri de date
#include <sys/socket.h> // librarie pt socket-uri
#include <sys/un.h> // librarie pt socket-uri UNIX
#include <string.h> // librarie pt operatii cu siruri de caractere
#include <fcntl.h> // librarie pt operatii pe fisiere
#include <time.h> // librarie pt functii de timp


#define SOCKET_PATH "/tmp/unixsd" // calea catre socket-ul UNIX
#define BUFFER_SIZE 256 // dimensiunea buffer-ului

int pipefd[2]; // pipe file descriptors
int unix_socket; // UNIX socket file descriptor
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// functie executata de firul 1
void* thread1(void* arg) {
    // creare socket UNIX
    struct sockaddr_un addr;
    if ((unix_socket = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // bind socket
    unlink(SOCKET_PATH);
    if (bind(unix_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // deschidere fisier si calculare dimensiune
    FILE *file = fopen("/etc/passwd", "r");
    if (file == NULL) {
        perror("Failed to open /etc/passwd");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fclose(file);

    // scriere dimensiune in socket
    if (sendto(unix_socket, &filesize, sizeof(filesize), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Failed to send filesize");
        exit(EXIT_FAILURE);
    }
    return NULL;
}

// functie executata de firul 2
void* thread2(void* arg) {
    // asteptare mesaj pe socket folosind polling
    struct pollfd pfd;
    pfd.fd = unix_socket;
    pfd.events = POLLIN;
    // asteptare mesaj pe socket
    if (poll(&pfd, 1, -1) == -1) {
        perror("Poll failed");
        exit(EXIT_FAILURE);
    }
    // citire dimensiune fisier
    if (pfd.revents & POLLIN) {
        long filesize;
        if (recv(unix_socket, &filesize, sizeof(filesize), 0) == -1) {
            perror("Failed to receive filesize");
            exit(EXIT_FAILURE);
        }
        printf("File size: %ld\n", filesize);
    }
    // inchidere socket
    close(unix_socket);
    return NULL;
}

// functie executata de firul 3
void* thread3(void* arg) {
    // creare pipe
    if (pipe(pipefd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // cronometrare operatii fir1
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_mutex_lock(&lock);
    // simulare operatii fir1
    sleep(1);  // simulare operatie de 1 secunda
    pthread_mutex_unlock(&lock); // deblocare mutex

    // cronometrare operatii fir1
    clock_gettime(CLOCK_MONOTONIC, &end);
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    // salvare durata in format hh:mm:ss
    int hours = (int)(elapsed / 3600);
    int minutes = (int)((elapsed - hours * 3600) / 60);
    int secs = (int)(elapsed - hours * 3600 - minutes * 60);
    char duration[BUFFER_SIZE];
    snprintf(duration, BUFFER_SIZE, "%02d:%02d:%02d", hours, minutes, secs);

    // scriere durata in pipe
    write(pipefd[1], duration, strlen(duration) + 1);
    close(pipefd[1]);

    return NULL;
}

// functie executata de firul 4
void* thread4(void* arg) {
    // asteptare mesaj pe pipe folosind polling
    struct pollfd pfd;
    pfd.fd = pipefd[0];
    pfd.events = POLLIN;
    // asteptare mesaj pe pipe
    if (poll(&pfd, 1, -1) == -1) {
        perror("Poll failed");
        exit(EXIT_FAILURE);
    }
    // citire durata din pipe
    if (pfd.revents & POLLIN) {
        char buffer[BUFFER_SIZE];
        read(pipefd[0], buffer, BUFFER_SIZE);
        printf("Elapsed time: %s\n", buffer);
    }
    // inchidere pipe
    close(pipefd[0]);
    return NULL;
}

// functia main
int main() {
    pthread_t threads[4];
    // creare thread-uri detached
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&threads[0], &attr, thread1, NULL) != 0 ||
        pthread_create(&threads[1], &attr, thread2, NULL) != 0 ||
        pthread_create(&threads[2], &attr, thread3, NULL) != 0 ||
        pthread_create(&threads[3], &attr, thread4, NULL) != 0) {
        fprintf(stderr, "Error creating threads\n");
        exit(EXIT_FAILURE);
    }

    // cleanup
    pthread_attr_destroy(&attr);

    // asteptare thread-uri
    sleep(5);

    return 0;
}

/**
 * Exemple de compilare si rulare a programului
 * junior@junior-pc:~/Desktop/PCD/TESTc$ gcc -o app1 app1.c ---> COMPILARE
 * ./app1 ----> RULARE
 *
 *junior@junior-pc:~/Desktop/PCD/TESTc$ gcc -o app1 app1.c
junior@junior-pc:~/Desktop/PCD/TESTc$ ./app1
File size: 2884
Elapsed time: 00:00:01
junior@junior-pc:~/Desktop/PCD/TESTc$

*/