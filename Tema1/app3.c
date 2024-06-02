/**
 * Anghel Costel Junior
 * IA3 2024, subgrupa 1
 * Tema 1
 * Acest program are ca scop calcularea indicelui de masa corporala a unei persoane(IMC).
 * Se calculeaza cu formula "weight/height*height", acestea fiind argumente obligatorii,
 * adaugat fiind sexul ca si parametru optional, valoarea IMC fiind neschimbata indiferent de sex.
 * Am tratat urmatoarele erori care pot aparea in contextul
 * programului de mai jos:
 * -- argumente incorecte la apel
 * -- valori incorecte pentru argumentele date
 * -- atentionare la introducerea sexului
 */

#include <stdio.h>// libraria standard pentru functii I/O
#include <stdlib.h>// libraria standard pentru alocare de memorie, control procese etc.
#include <string.h> // pentru strcmp
#include <argp.h> //functia argp_parse

// struct-ul pentru argumente
struct arguments {
    float height;
    float weight;
    char* sex;
};

// definirea argumentelor acceptate
static struct argp_option options[] = {
        {"height", 'h', "HEIGHT", 0, "Inaltimea in metri", 0},
        {"weight", 'w', "WEIGHT", 0, "Greutatea in kilograme", 0},
        {"sex", 's', "SEX", 0, "Sexul (barbat/femeie)", 0},
        {0}
};

// functia pentru parsarea argumentelor
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'h':
            arguments->height = atof(arg);
            break;
        case 'w':
            arguments->weight = atof(arg);
            break;
        case 's':
            arguments->sex = arg;
            break;
        case ARGP_KEY_ARG:
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

// structura argp care contine informatii despre comanda
static struct argp argp = {options, parse_opt, 0, 0};

int main(int argc, char **argv) {
    struct arguments arguments;

    // valorile implicite pentru argumente
    arguments.height = 0.0;
    arguments.weight = 0.0;
    arguments.sex = NULL;

    // parsarea argumentelor
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    //daca una sau ambele dintre valorile mandatory este/sunt 0
    if(arguments.height == 0 || arguments.weight == 0){
        printf("Argumente incorecte!\n");
        exit(1);}

    //verificare sex introdus
    if(arguments.sex != NULL && strcmp(arguments.sex,"barbat") != 0  && (strcmp(arguments.sex,"femeie") != 0 )){
    printf("Introduceti sexul: barbat/femeie\n");
    exit(1);}

    // calcularea IMC-ului
    float bmi = arguments.weight / (arguments.height * arguments.height);

    // afisarea rezultatului
    printf("Inaltime: %.2f m\n", arguments.height);
    printf("Greutate: %.2f kg\n", arguments.weight);
    printf("Sex: %s\n", arguments.sex ? arguments.sex : "Nedefinit");
    printf("IMC: %.2f\n", bmi);

    // interpretarea rezultatului
    if (bmi < 18.5) {
        printf("Interpretare: Subponderal\n");
    } else if (bmi < 25) {
        printf("Interpretare: Greutate normala\n");
    } else if (bmi < 30) {
        printf("Interpretare: Supraponderal\n");
    } else {
        printf("Interpretare: Obezitate\n");
    }

    return 0;
}

/* Exemple de compilare si rulare a programului

 gcc -o app3 app3.c        --> compilare


./app3 -h 1.75 -w 65
Inaltime: 1.75 m
Greutate: 65.00 kg
Sex: Nedefinit
IMC: 21.22
Interpretare: Greutate normala


./app3 -h 1.75 -w 65 -s barbat
Inaltime: 1.75 m
Greutate: 65.00 kg
Sex: barbat
IMC: 21.22
Interpretare: Greutate normala


./app3 -h 1.75 -w 65 -s bar
Introduceti sexul: barbat/femeie


./app3 -h 1.75 -w 65 -s
./app3: option requires an argument -- 's'
Try `app3 --help' or `app3 --usage' for more information.


./app3 -h 1.75
Argumente incorecte!


 */