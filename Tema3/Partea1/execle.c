/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.1
 * Programul de mai jos foloseste functia `execle` pentru a afisa ls -l.
 */


#include <stdio.h> //perror()
#include <unistd.h> //fork(), execle()
#include <stdlib.h> //abort()
#include <sys/wait.h> // pt wait()

int main() {
    pid_t pid = fork(); // cream un nou proces

    if (pid == 0) { // procesul copil
        char *env[] = { "VAR=varianta_noua", NULL }; // folosim mediul curent
        execle("/bin/sh", "sh", "-c", "/bin/echo $VAR", NULL, env);
        perror("execle");
        abort(); // eroare dacă execle esueaza
    } else if (pid > 0) { // procesul parinte
        wait(NULL); // asteptam terminarea procesului copil
    } else { // eroare la fork
        perror("fork");
        return 1;
    }

    return 0;
}
/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc execle.c -o execle

RULARE -> junior@junior-pc:~/Desktop/PCD/Tema3/Partea1$ ./execle
varianta_noua


 */
