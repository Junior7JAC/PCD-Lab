/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Test Nivel B, app2.c, R3
    * Acest program  este un server care asculta pe un socket de tip AF_UNIX.
 */

#include <stdio.h> // librarie pentru I/O
#include <pthread.h> // librarie pt thread-uri
#include <signal.h> // librarie pt semnale
#include <unistd.h> // librarie pt fork, pipe
#include <sys/stat.h> // librarie pt stat
#include <semaphore.h> // librarie pt semafoare

#define CALE_FISIER "/etc/passwd" // calea catre fisier

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // mutex-ul
pthread_cond_t condSig1 = PTHREAD_COND_INITIALIZER; // variabila de conditie pentru semnalul 1
pthread_cond_t condSig2 = PTHREAD_COND_INITIALIZER; // variabila de conditie pentru semnalul 2
sem_t semafor; // semaforul

volatile sig_atomic_t sig1_primit = 0; // semnalul 1 primit
volatile sig_atomic_t sig2_primit = 0; // semnalul 2 primit
volatile size_t bytesNo = 0; // numarul de bytes

// handler pentru semnalul 1
void handler_sig1(int sig) {
    sig1_primit = 1;
    pthread_cond_signal(&condSig1);
}

// handler pentru semnalul 2
void handler_sig2(int sig) {
    sig2_primit = 1;
    pthread_cond_signal(&condSig2);
}

// functia firului 1
void* functiaFir1(void* arg) {
    struct stat info;

    // verifica daca fisierul exista
    if (stat(CALE_FISIER, &info) == 0) {
        pthread_mutex_lock(&mtx);
        bytesNo = info.st_size;
        pthread_mutex_unlock(&mtx);

        // trimite SIGUSR1 catre firul 2
        pthread_kill(*(pthread_t*)arg, SIGUSR1);
    }
    return NULL;
}

// functia firului 2
void* functiaFir2(void* arg) {
    pthread_mutex_lock(&mtx);
    while (!sig1_primit)
        pthread_cond_wait(&condSig1, &mtx);

    bytesNo += 1500;
    pthread_mutex_unlock(&mtx);

    // trimite SIGUSR2 catre firul principal
    pthread_kill(pthread_self(), SIGUSR2);

    return NULL;
}

int main() {
    struct sigaction act1, act2;
    pthread_t fir1, fir2;

    // initializeaza semaforul
    sem_init(&semafor, 0, 0);

    // seteaza handler-ele pentru semnale
    act1.sa_handler = handler_sig1;
    act1.sa_flags = 0;
    sigemptyset(&act1.sa_mask);
    sigaction(SIGUSR1, &act1, NULL);

    act2.sa_handler = handler_sig2;
    act2.sa_flags = 0;
    sigemptyset(&act2.sa_mask);
    sigaction(SIGUSR2, &act2, NULL);

    // creeaza firele
    pthread_create(&fir2, NULL, functiaFir2, NULL);
    pthread_create(&fir1, NULL, functiaFir1, &fir2);

    // detaseaza firele
    pthread_detach(fir1);
    pthread_detach(fir2);

    // asteapta SIGUSR2
    pthread_mutex_lock(&mtx);
    while (!sig2_primit)
        pthread_cond_wait(&condSig2, &mtx);
    pthread_mutex_unlock(&mtx);

    // afiseaza valoarea finala a lui bytesNo
    printf("Valoarea finala a lui bytesNo: %zu\n", bytesNo);

    // distruge mutex-ul si variabilele de conditie
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&condSig1);
    pthread_cond_destroy(&condSig2);
    sem_destroy(&semafor);

    return 0;
}

/**
 * Exemple de compilare si rulare a programului
 * gcc app2.c -o app2 ---> COMPILARE
 * ./app2 ----> RULARE
 *
    junior@junior-pc:~/Desktop/PCD/Test$ gcc app2.c -o app2
    junior@junior-pc:~/Desktop/PCD/Test$ ./app2
    Valoarea finala a lui bytesNo: 4384
    junior@junior-pc:~/Desktop/PCD/Test$ wc -c /etc/passwd
    2884 /etc/passwd

*/
