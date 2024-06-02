/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 2
 * Programul de mai jos primeste diferite ca argument numarul
 * de procese pe care trebuie sa le creeze plecand de la procesul 0,
 * si numarul de subprocese pe care le creaza avand ca parinte procesele create anterior
 * Am tratat urmatoarele situatii limita care pot aparea in contextul
 * programului de mai jos:
 * -- numar argumente incorect
 * -- argumente incorecte
 * -- valori incorecte pentru argumentele date (nu sunt numerice, valoarea nu se conformeaza constrangerilor etc.)

 */

#include <stdio.h> // libraria standard pentru functii I/O
#include <stdlib.h> // libraria standard pentru alocare de memorie, control procese etc.
#include <unistd.h> // pentru functiile fork, getpid, and getppid
#include <string.h> // strcmp
#include <sys/wait.h> // functia wait() pentru sincronizarea proceselor


// functie pentru crearea subproceselor
void create_subprocesses(int process_number, int num_subprocesses) {
    if(num_subprocesses <= 0) { // verificam daca numarul de subprocese este mai mic sau egal cu 0
        exit(EXIT_SUCCESS);
    }
        pid_t sub_pid = fork();
        if (sub_pid == -1) {
            perror("Failed to fork subprocess");
            exit(EXIT_FAILURE);
        } else if (sub_pid == 0) { // subprocess
            printf("Process[%d.%d] PID %d PPID %d\n", process_number, num_subprocesses, getpid(), getppid());
            create_subprocesses(process_number, num_subprocesses - 1);
            exit(EXIT_SUCCESS);
        } else{
            wait(NULL); // asteapta ca subprocesul sa se termine
        }
}

// functie pentru crearea proceselor
void create_processes(int num_processes, int num_subprocesses) {
    printf("Process[0] PID %d PPID %d\n", getpid(), getppid());

    for (int i = 1; i <= num_processes; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Failed to fork child process");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // procesul copil
            printf("Process[%d] PID %d PPID %d\n", i, getpid(), getppid());
            create_subprocesses(i, num_subprocesses);
            exit(EXIT_SUCCESS);
        }
        wait(NULL); // asteapta ca procesul copil sa se termine
    }
}

int main(int argc, char *argv[]) {
    // verificam daca numarul de argumente este corect
    if (argc != 5) {
        fprintf(stderr, "Usage: %s --processes <num> --subprocesses <num>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_processes = 0, num_subprocesses = 0;

    // procesam argumentele liniei de comanda, si pt short commands
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--processes") == 0 || strcmp(argv[i], "-p") == 0) {
            num_processes = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--subprocesses") == 0 || strcmp(argv[i], "-s") == 0) {
            num_subprocesses = atoi(argv[i + 1]);
        }
    }

    // verificam daca numarul de procese si subprocese este valid
    if (num_processes <= 0 || num_subprocesses <= 0) {
        fprintf(stderr, "Invalid number of processes or subprocesses. Both must be positive.\n");
        return EXIT_FAILURE;
    }

    // crearea procesului A
    printf("Process[A] PID %d PPID %d\n", getpid(), getppid());

    // crearea procesului B
    pid_t pid_b = fork();
    if (pid_b == -1) {
        perror("Failed to fork process B");
        return EXIT_FAILURE;
    } else if (pid_b == 0) { // procesul B
        printf("Process[B] PID %d PPID %d\n", getpid(), getppid());

        // crearea procesului 0
        pid_t pid_0 = fork();
        if (pid_0 == -1) {
            perror("Failed to fork process 0");
            return EXIT_FAILURE;
        } else if (pid_0 == 0) { // procesul 0
            create_processes(num_processes, num_subprocesses);
            exit(EXIT_SUCCESS);
        }

        wait(NULL); // asteapta ca procesul 0 sa se termine
        exit(EXIT_SUCCESS);
    }
    wait(NULL); // asteapta ca procesul B sa se termine
    return EXIT_SUCCESS;
}

/** > Exemple de compilare si rulare a programului

gcc app2.c -o app2 --> compilare


 ./app2 -p <nr intreg pozitiv> -s <nr intreg pozitiv> --> rulare

junior@junior-pc:~/Desktop/PCD/Tema2$ gcc app2.c -o app2
junior@junior-pc:~/Desktop/PCD/Tema2$ ./app2 -p 2 -s 3
Process[A] PID 6042 PPID 3487
Process[B] PID 6043 PPID 6042
Process[0] PID 6044 PPID 6043
Process[1] PID 6045 PPID 6044
Process[1.3] PID 6046 PPID 6045
Process[1.2] PID 6047 PPID 6046
Process[1.1] PID 6048 PPID 6047
Process[2] PID 6049 PPID 6044
Process[2.3] PID 6050 PPID 6049
Process[2.2] PID 6051 PPID 6050
Process[2.1] PID 6052 PPID 6051

junior@junior-pc:~/Desktop/PCD/Tema2$ ./app2 --processes 2 --subprocesses 3
Process[A] PID 6745 PPID 3487
Process[B] PID 6746 PPID 6745
Process[0] PID 6747 PPID 6746
Process[1] PID 6748 PPID 6747
Process[1.3] PID 6749 PPID 6748
Process[1.2] PID 6750 PPID 6749
Process[1.1] PID 6751 PPID 6750
Process[2] PID 6752 PPID 6747
Process[2.3] PID 6753 PPID 6752
Process[2.2] PID 6754 PPID 6753
Process[2.1] PID 6755 PPID 6754



ERORI:

 ./app2
Folosire: ./app2 -p <procese> -s <subprocese>

./app2 -p 4 -s 0
Eroare: Numar invalid de subprocese. Numarul trebuie sa fie intreg pozitiv.

 ./app2 -p gluma -s 3
Eroare: Numar invalid de procese. Numarul trebuie sa fie intreg pozitiv.

 */