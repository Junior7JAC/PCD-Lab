/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 3.1
 * Programul de mai jos foloseste functia `execlp` pentru a afisa calendarul anului 2024.
 */


#include <stdio.h> //perror()
#include <unistd.h> //fork(), execlp()
#include <stdlib.h> //abort()
#include <sys/wait.h> // pt wait()

int main() {
    pid_t pid = fork(); // cream un nou proces

    if (pid == 0) { // procesul copil
        execlp("cal", "cal", "2024", NULL);
        perror("execlp");
        abort(); // eroare dacă execlp esueaza
    } else if (pid > 0) { // procesul parinte
        wait(NULL); // asteptam terminarea procesului copil
    } else { // eroare la fork
        perror("fork");
        return 1;
    }

    return 0;
}
/*> Exemple de compilare si rulare a programului

COMPILARE ->  gcc execlp.c -o execlp

RULARE ->./execlp
                            2024
      Ianuarie             Februarie               Martie
Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb
    1  2  3  4  5  6               1  2  3                  1  2
 7  8  9 10 11 12 13   4  5  6  7  8  9 10   3  4  5  6  7  8  9
14 15 16 17 18 19 20  11 12 13 14 15 16 17  10 11 12 13 14 15 16
21 22 23 24 25 26 27  18 19 20 21 22 23 24  17 18 19 20 21 22 23
28 29 30 31           25 26 27 28 29        24 25 26 27 28 29 30
                                            31

      Aprilie                 Mai                  Iunie
Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb
    1  2  3  4  5  6            1  2  3  4                     1
 7  8  9 10 11 12 13   5  6  7  8  9 10 11   2  3  4  5  6  7  8
14 15 16 17 18 19 20  12 13 14 15 16 17 18   9 10 11 12 13 14 15
21 22 23 24 25 26 27  19 20 21 22 23 24 25  16 17 18 19 20 21 22
28 29 30              26 27 28 29 30 31     23 24 25 26 27 28 29
                                            30

       Iulie                 August              Septembrie
Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb
    1  2  3  4  5  6               1  2  3   1  2  3  4  5  6  7
 7  8  9 10 11 12 13   4  5  6  7  8  9 10   8  9 10 11 12 13 14
14 15 16 17 18 19 20  11 12 13 14 15 16 17  15 16 17 18 19 20 21
21 22 23 24 25 26 27  18 19 20 21 22 23 24  22 23 24 25 26 27 28
28 29 30 31           25 26 27 28 29 30 31  29 30


     Octombrie             Noiembrie             Decembrie
Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb  Du Lu Ma Mi Jo Vi Sb
       1  2  3  4  5                  1  2   1  2  3  4  5  6  7
 6  7  8  9 10 11 12   3  4  5  6  7  8  9   8  9 10 11 12 13 14
13 14 15 16 17 18 19  10 11 12 13 14 15 16  15 16 17 18 19 20 21
20 21 22 23 24 25 26  17 18 19 20 21 22 23  22 23 24 25 26 27 28
27 28 29 30 31        24 25 26 27 28 29 30  29 30 31

*/