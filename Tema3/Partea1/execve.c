/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.1
 * Programul de mai jos foloseste functia `execve` pentru a afisa un mesaj cu echo.
 */

#include <stdio.h> //perror()
#include <unistd.h> //fork(), execve()
#include <stdlib.h> //abort()
#include <sys/wait.h> // pt wait()

int main() {
    pid_t pid = fork(); // cream un nou proces

    if (pid == 0) { // procesul copil
        char* arg_list[] = { "sh", "-c", "/bin/echo $VAR", NULL };
        char *env[] = {"VAR=varianta_noua", NULL}; // folosim mediul curent
        execve("/bin/sh", arg_list, env); // echo
        perror("execve");
        abort(); // eroare dacă execve esueaza
    } else if (pid > 0) { // procesul parinte
        wait(NULL); // asteptam terminarea procesului copil
    } else { // eroare la fork
        perror("fork");
        return 1;
    }
    return 0;
}

/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc execve.c -o execve

RULARE ->junior@junior-pc:~/Desktop/PCD/Tema3/Partea1$ ./execve
varianta_noua

 */