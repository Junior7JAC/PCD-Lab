/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 2
 *  Programul de mai jos primeste argument numarul
 * de procese pe care trebuie sa le creeze plecand de la procesul 0
 * Am tratat urmatoarele situatii limita care pot aparea in contextul
 *  programului de mai jos:
 * -- numar argumente incorect
 * -- argumente incorecte
 * -- valori incorecte pentru argumentele date (nu sunt numerice, valoarea nu se conformeaza constrangerilor etc.)
 */

#include <stdio.h> // libraria standard pentru functii I/O
#include <stdlib.h> // libraria standard pentru alocare de memorie, control procese etc.
#include <unistd.h> // pentru functiile fork, getpid, and getppid
#include <string.h> // strcmp
#include <sys/wait.h> // functia wait() pentru sincronizarea proceselor

int main(int argc, char *argv[]) {
    int p = 0; // procesele
    pid_t pid; //var pt PID-urile proceselor

    // procesam argumentele liniei de comanda
    if (argc != 3) {
        fprintf(stderr, "Folosire: %s -p <procese>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // argumente pentru numarul de procese
    if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--processes") == 0) {
        p = atoi(argv[2]); // convertim argumentul pentru procese intr-un intreg
        if (p <= 0) { // verificam daca numarul este pozitiv
            fprintf(stderr, "Eroare: Numar invalid de procese. Numarul trebuie sa fie intreg pozitiv.\n");
            return EXIT_FAILURE;
        }
    } else { // daca argumentul nu este nici "-p", nici "--processes"
        fprintf(stderr, "Eroare: Optiune invalida. '%s'. Foloseste -p sau --processes.\n", argv[1]);
        return EXIT_FAILURE;
    }

    // procesul A (procesul parinte)
    printf("Proces[A] PID %d PPID %d\n", getpid(), getppid());
    pid = fork(); //primul fork(), catre B

    // verificam dacă fork() a reusit
    if (pid < 0) {
        // eroare la fork
        fprintf(stderr, "Fork failed\n");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // afisam procesul copil B
        printf("Proces[B] PID %d PPID %d\n", getpid(), getppid());
        pid = fork(); //cream procesul copil 0

        // verificam daca fork a reusit
        if (pid < 0) {
            // eroare la fork
            fprintf(stderr, "Fork failed\n");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            // afisam procesul copil 0
            printf("Proces[0] PID %d PPID %d\n", getpid(), getppid());
            for (int i = 1; i <= p; i++) {
                pid = fork(); // cream procesele copii de la 1 la p
                if (pid < 0) {
                    //eroare la fork
                    fprintf(stderr, "Fork failed\n");
                    return EXIT_FAILURE;
                } else if (pid == 0) {
                    // afisam procesele copil de la 1 la p
                    printf("Proces[%d] PID %d PPID %d\n", i, getpid(), getppid());
                    return EXIT_SUCCESS; // Children terminate after printing
                }
            }

            while (wait(NULL) > 0); // asteapta toti copii sa iasa
            return EXIT_SUCCESS; // procesul 0 iese dupa ce toti copii lui termina
        } else {
            wait(NULL); // parintele luui 0 (B) asteapta copii lui 0
            return EXIT_SUCCESS;
        }
    } else {
        wait(NULL); // parintele luui B (A) asteapta copii lui B
        return EXIT_SUCCESS;
    }
}

/** > Exemple de compilare si rulare a programului

gcc app1.c -o app1 --> compilare

 ./app1 -s <nr intreg pozitiv> --> rulare

./app1 -p 4
Proces[A] PID 4512 PPID 3426
Proces[B] PID 4513 PPID 4512
Proces[0] PID 4514 PPID 4513
Proces[1] PID 4515 PPID 4514
Proces[2] PID 4516 PPID 4514
Proces[3] PID 4517 PPID 4514
Proces[4] PID 4518 PPID 4514

./app1 --processes 2
Proces[A] PID 4603 PPID 3426
Proces[B] PID 4604 PPID 4603
Proces[0] PID 4605 PPID 4604
Proces[1] PID 4606 PPID 4605
Proces[2] PID 4607 PPID 4605


 ERORI:

./app1 -x 2
Eroare: Optiune invalida. '-x'. Foloseste -p sau --processes.

./app1 -p ac
Eroare: Numar invalid de procese. Numarul trebuie sa fie intreg pozitiv.

./app1 -p -3
Eroare: Numar invalid de procese. Numarul trebuie sa fie intreg pozitiv.

 ./app1
Folosire: ./app1 -p <procese>


 */
