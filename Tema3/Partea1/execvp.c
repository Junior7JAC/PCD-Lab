/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.1
 * Programul de mai jos foloseste functia `execvp` pentru a afisa directorul curent.
 */


#include <stdio.h> //perror()
#include <unistd.h> //fork(), execvp()
#include <stdlib.h> //abort()
#include <sys/wait.h> // pt wait()

int main() {
    pid_t pid = fork(); // cream un nou proces

    if (pid == 0) { // procesul copil
        char *args[] = {"pwd", NULL};
        execvp("pwd", args);
        perror("execvp");
        abort(); // eroare dacă execvp esueaza
    } else if (pid > 0) { // procesul parinte
        wait(NULL); // asteptam terminarea procesului copil
    } else { // eroare la fork
        perror("fork");
        return 1;
    }

    return 0;
}
/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc execvp.c -o execvp

RULARE ->./execvp
/home/junior/Desktop/Tema3/Partea1

 */