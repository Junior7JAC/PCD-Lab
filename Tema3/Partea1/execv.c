/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.1
 * Programul de mai jos foloseste functia `execv` pentru a afisa data curenta în formatul an-luna-zi.
 */

#include <stdio.h> //perror()
#include <unistd.h> //fork(), execv()
#include <stdlib.h> //abort()
#include <sys/wait.h> // pt wait()

int main() {
    pid_t pid = fork();// cream un nou proces

    if (pid == 0) { // procesul copil
        char *args[] = {"/bin/date", "+%Y-%m-%d", NULL};
        execv("/bin/date", args);
        perror("execv");
        abort(); // eroare dacă execv esueaza
    } else if (pid > 0) { // procesul parinte
        wait(NULL); // asteptam terminarea procesului copil
    } else { // eroare la fork
        perror("fork");
        return 1;
    }
    return 0;
}
/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc execv.c -o execv

RULARE ->./execv
2024-03-20

 */