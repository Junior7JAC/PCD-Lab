/**
 * Nume si prenume: Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 4
 * <Includeti aici un scurt rezumat al temei>
 * Am creat o structura de fisiere fara apel de exec sau system
 * Am tratat urmatoarele situatii limita care pot aparea inbcontextul programului de mai jos :
 * -- numar de argumente incorect
 * -- valori incorecte pentru argumentele date (nu sunt numerice, valoarea nu se conformeaza constrangerilor problemei)
 */

#include <sys/stat.h>
#include <fcntl.h>     // pentru O_RDONLY, O_WRONLY
#include <unistd.h>

int main() {

    // creare unui dir "folder" si mutarea in el
    mkdir("folder", 0777);
    chdir("folder");

    // creare dir "d1" cu permisiuni specifice si mutarea in el
    mkdir("d1", 0005);
    chdir("d1");

    // creare dir "d2" in interiorul lui "d1"
    mkdir("d2", 0006);
    chdir("d2");

    // crearea fisierului "f1" in "d2"
    int fd_f1 = open("f1", O_WRONLY | O_TRUNC | O_CREAT, 0007);
    close(fd_f1);

    // crearea directorului "d3" in "d2" si mutarea in el
    mkdir("d3", 0632);
    chdir("d3");

    // crearea fisierului "f4" in "d3"
    int fd_f4 = open("f4", O_WRONLY | O_TRUNC | O_CREAT, 0007);
    close(fd_f4);

    // revenirea la dir "d2"
    chdir("..");

    // revenirea la dir "d1"
    chdir("..");

    // creare hard link "f5" la "f1" in "d1"
    link("d2/f1", "f5");

    // revenirea la dir "folder"
    chdir("..");

    // crearea fisierului "f2" in "folder"
    int fd_f2 = open("f2", O_WRONLY | O_TRUNC | O_CREAT, 0004);
    close(fd_f2);

    // crearea fisierului "f3" in "folder"
    int fd_f3 = open("f3", O_WRONLY | O_TRUNC | O_CREAT, 0003);
    close(fd_f3);

    // crearea unui symlink "f4" la "f1"
    symlink("d1/d2/f1", "f4");

    return 0;
}

/*

junior@junior-pc:~/Desktop/Tema4$ gcc ex4.c -o ex4 -----> COMPILARE
junior@junior-pc:~/Desktop/Tema4$ ./ex4 ------> RULARE
junior@junior-pc:~/Desktop/Tema4$ tree -p

├── [drwxrwxr-x]  folder
├── [d------r-x]  d1  [error opening dir]
├── [d------r--]  d2  [error opening dir]
├── [drw--wx---]  d3
├── [-------r-x]  f1
└── [-------r-x]  f4

*Nu cred ca respecta intocmai diagrama de pe classroom, insa nici nu am inteles-o prea bine :D
 */