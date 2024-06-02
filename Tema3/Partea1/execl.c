/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.1
 * Programul de mai jos foloseste functia `execl` pentru a afisa numele user-ului curent.
 */

#include <stdio.h> //perror()
#include <unistd.h> //fork(), execl()
#include <stdlib.h> //abort()
#include <sys/wait.h> // pt wait()

int main() {
    pid_t pid = fork();// cream un nou proces

    if (pid == 0) { // procesul copil
        execl("/usr/bin/whoami", "whoami", NULL);
        perror("execl");
        abort(); // eroare dacă execl esueaza
    } else if (pid > 0) { // procesul parinte
        wait(NULL); // asteptam terminarea procesului copil
    } else { // eroare la fork
        perror("fork");
        return 1;
    }

    return 0;
}
/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc execl.c -o execl

RULARE ->./execl
junior

 */