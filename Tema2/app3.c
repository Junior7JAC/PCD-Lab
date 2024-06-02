/**
 * Anghel Costel Junior
 * IA3 2023, subgrupa 1
 * Tema 2
 * Programul de mai jos cauta in folderul /proc
 * informatiile despre procese de care are nevoie, apoi le afiseaza.
 */


#include <stdio.h> // libraria standard pentru functii I/O
#include <stdlib.h> // libraria standard pentru alocare de memorie, control procese etc.
#include <dirent.h>  // pentru manipularea directoarelor
#include <string.h> //pt strcmp

//functia care afiseaza informatiile despre proces bazate pe PID
void printProcessInfo(const char *pid) {
    char path[256];
    FILE *fp;
    // construim calea catre fisierul de status al procesului
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    fp = fopen(path, "r");
    if (!fp) return; // Daca nu poate deschide fisierul, ignora

    char line[256];
    // identificatori cautati
    char *identifiers[] = {"Pid:", "PPid:", "Uid:", "Gid:"};
    int found = 0; // contor pt informatiile gasite

    //citire fiecare linie din fisier
    while (fgets(line, sizeof(line), fp)) {
        //verificam fiecare identificator
        for (int i = 0; i < 4; ++i) {
            //daca linia contine identificatorul cautat o afisam
            if (strncmp(line, identifiers[i], strlen(identifiers[i])) == 0) {
                printf("%s %s", identifiers[i], line + strlen(identifiers[i]));
                found++;
                break;
            }
        }
        if (found == 4) break; // daca am gasit toate informatiile necesare
    }

    fclose(fp);
}

int main() {
    DIR *dir;
    struct dirent *ent; //pentru a stoca informatiile despre fiecare intrare in director

    //incercam sa deschidem fisierul /proc
    dir = opendir("/proc");
    if (!dir) {
        perror("Eroare la deschiderea /proc");
        return EXIT_FAILURE;
    }

    // parcurgem fiecare intrare în fisierul /proc
    while ((ent = readdir(dir)) != NULL) {
        // verificam daca numele directorului este un numar (PID)
        if (ent->d_type == DT_DIR && strspn(ent->d_name, "0123456789") == strlen(ent->d_name)) {
            printProcessInfo(ent->d_name); // afisam informatiile despre proces folosind PID-ul
        }
    }

    closedir(dir); //inchidem fisierul
    return EXIT_SUCCESS;
}

/** > Exemple de compilare si rulare a programului

gcc app3.c -o app3 --> compilare


 ./app3 --> rulare


Pid:    1
PPid:   0
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    2
PPid:   0
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    3
PPid:   2
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    4
PPid:   2
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    5
PPid:   2
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    6
PPid:   2
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    7
PPid:   2
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    8
PPid:   2
Uid:    0       0       0       0
Gid:    0       0       0       0
Pid:    11
PPid:   2

 */


