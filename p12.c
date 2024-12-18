
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ANTAL_BYER 100

typedef struct {
    int index;                //byens nummer i csv-filen
    char navn[50];            //byens navn
    char prisniveau[20];      //budget, mellemklasse eller luksus
    int km_til_Venedig;       //afstand fra byen til Venedig
    char oplev_1[20];         //landskab, historisk eller vand
    char oplev_2[20];         //aktiv, afslapning eller mad og vin
    int km_DK_by;             //afstand fra Københavns Hovedbanegård til byen
    int km_DK_Vluft;          //afstand fra Kastrup Lufthavn til Venedig Lufthavn
    int km_Vluft_by;          //afstand fra Venedig Lufthavn til by
    char *beskrivelse;        //beskrivelse af byen.
} By;

void filtrer_byer(By by_array[], int *antal_byer);
void bedste_by_match(By by_array[], int *antal_byer);
By find_random_by(By by_array[], int *antal_byer);
By *filtrer_pris(By by_array[], int *antal_byer, int prisklasse);
By *filtrer_radius(By by_array[], int *antal_byer, int valg_radius);
By *filtrer_oplevelse(By by_array[], int *antal_byer, int valg);
By *filtrer_oplevelse2(By by_array[], int *antal_byer, int valg_opl);
double beregn_co2_udledning(double afstand, int transportmiddel);
void print_CO2_beregning(By by);
void laes_alle_byer(FILE *file, By by_array[], int *length);
int Sammenlign_Kilometer(const void *a, const void *b);
int laes_fra_data_fil(const char *filnavn, By by_array[], int *antal_byer);
int laes_fra_beskrivelser_fil(const char *filnavn, By by_array[]);


int main(void){
    int antal_byer = 0;
    By by_array[ANTAL_BYER] = {0};  
    srand(time(NULL));


    printf("Hej og velkommen til vores hiddengem guide til Veneto!\n");
    printf("Vi vil stille dig en række spørgsmål der hjælper dig til at finde det perfekte alternativ til det overturistiske Venedig.\n");
    
    laes_fra_data_fil("data.csv", by_array, &antal_byer);

    laes_fra_beskrivelser_fil("beskrivelser.csv", by_array);

    filtrer_byer(by_array, &antal_byer);

    return 0;
}

//her kalder vi alle vores funktioner og printer vores recommendation
//*antal_byer er en pointer fra de funktioner vi kalder, fordi antallet af byer ændrer sig undervejs
void filtrer_byer(By by_array[], int *antal_byer){
    int oplevelsesvalg;
    int oplevelsesvalg_2;
    int radius;
    int prisklasse;
    int random_valg;
    int alternativ;
    By random_by;

    //vælg afstand til Venedig
    printf("\nHvor langt må byen være fra Venedig?\n1: 5-30 km\n2: 31-60 km\n3: 61+ km\n");
    scanf(" %d", &radius);

    //check for ugyldigt input
    while (radius != 1 && radius != 2 && radius != 3) {
        printf("\nUgyldig værdi. Venligst vælg fra listen:\n1: 5-30 km\n2: 31-60 km\n3: 61+ km\n");
        
        //hvis man taster andet end 1, 2 eller 3
        while(getchar() != '\n');  // Fjerner resten af inputtet (fx bogstaver eller ekstra tegn)
        
        scanf(" %d", &radius); 
    }

    //vælg budget
    printf("\nVælg prisklasse:\n1: budget(5000-10.000 DKK)\n2: mellemklasse(10.000-18.000 DKK)");
    printf("\n3: luksus(18.000+)\n");
    scanf(" %d", &prisklasse);

    //check for ugyldigt input
    while (prisklasse != 1 && prisklasse != 2 && prisklasse != 3) {
        printf("\nUgyldig værdi. Venligst vælg fra listen:\n1: budget(5000-10.000 DKK)\n2: mellemklasse(10.000-18.000 DKK\n3: luksus(18.000+)\n");
        
        //hvis man taster andet end 1, 2 eller 3
        while(getchar() != '\n');  // Fjerner resten af inputtet (fx bogstaver eller ekstra tegn)
        
        scanf(" %d", &prisklasse);
    }

    //vælger oplevelse og opdaterer by_array med byer som opfylder kriteriet
    printf("\nHvilke omgivelser ønsker du mest?\n1: Smukke landskaber\n2: Historiske seværdigheder\n3: Aktiviteter ved vandet\n");
    scanf(" %d", &oplevelsesvalg);
    
    //check for ugyldigt input
    while (oplevelsesvalg != 1 && oplevelsesvalg != 2 && oplevelsesvalg != 3) {
        printf("\nVælg venligst fra listen:\n1: Smukke landskaber\n2: Historiske seværdigheder\n3: Aktiviteter ved vandet\n");
        
        //hvis man taster andet end 1, 2 eller
        while(getchar() != '\n');  // Fjerner resten af inputtet (fx bogstaver eller ekstra tegn)
        
        scanf(" %d", &oplevelsesvalg);
    }
    
    //oplevelses_valg_2 bruges hvis der er mere end én by tilbage, når by_array har været igennem de øvrige filtreringsfunktioner
    //bruges i print_anbefaling
    printf("\nHvad går du mest op i at lave på din rejse:\n1: Aktive oplevelser\n2: Afslapning\n3: Kulinariske oplevelser\n");
    scanf(" %d", &oplevelsesvalg_2);

    //check for ugyldigt input
    while (oplevelsesvalg_2 != 1 && oplevelsesvalg_2 != 2 && oplevelsesvalg_2 != 3) {
        printf("\nVælg venligst fra listen:\n1: Aktive oplevelser\n2: Afslapning\n3: Kulinariske oplevelser\n");
        
        //hvis man taster andet end 1, 2 eller
        while(getchar() != '\n');  // Fjerner resten af inputtet (fx bogstaver eller ekstra tegn)

        scanf(" %d", &oplevelsesvalg_2);
    }

    //by_array går igennem de tre filtre
    filtrer_radius(by_array, antal_byer, radius);
    filtrer_pris(by_array, antal_byer, prisklasse);
    filtrer_oplevelse(by_array, antal_byer, oplevelsesvalg);
    qsort(by_array, *antal_byer, sizeof(By), Sammenlign_Kilometer);
    
    if (*antal_byer < 1) {
        bedste_by_match(by_array, antal_byer);
    } else {
        filtrer_oplevelse2(by_array, antal_byer, oplevelsesvalg_2);
    }
    
    if (*antal_byer < 1) {
        bedste_by_match(by_array, antal_byer);
    } 
    printf("\nVi foreslår at du rejser til %s.\n", by_array[0].navn);
    printf("\n%s er %s\n",by_array[0].navn, by_array[0].beskrivelse);
    
    //printer kun for den første by
    print_CO2_beregning(by_array[0]);
    printf("\nTak for nu\n");
}

void bedste_by_match(By by_array[], int *antal_byer){
    int alternativ;
    int random_valg;
    By random_by;

    printf("\nVi fandt desværre intet perfekt match.\n");
    // Giver det bedste bud fra før den blev filtreret af oplev1 og derefter oplev2
    printf("%s er et muligt match.\n", by_array[0].navn);
    printf("%s er %s\n",by_array[0].navn, by_array[0].beskrivelse);
    printf("\nVirker dette interessant for dig?\n1: Ja\n2: Nej\n");
    scanf("%d", &alternativ);

    while (alternativ != 1 && alternativ != 2){
        printf("\nVælg fra listen:\n1: Ja\n2: Nej\n");
        //hvis man taster andet end 1 eller 2
        while(getchar() != '\n');  //fjerner resten af inputtet (fx bogstaver eller ekstra tegn)
        scanf(" %d", &alternativ);
    }

    if (alternativ == 1){
        print_CO2_beregning(by_array[0]);
        printf("\nTak for nu\n");
        exit(EXIT_FAILURE);
    } else if (alternativ == 2){
        printf("\nVil du prøve vores random gem generator?\n1: Ja\n2: Nej\n");
        scanf(" %d", &random_valg);

        while (random_valg != 1 && random_valg != 2){
        printf("\nVælg fra listen:\n1: Ja\n2: Nej\n");
        //hvis man taster andet end 1 eller 2
        while(getchar() != '\n');  //fjerner resten af inputtet (fx bogstaver eller ekstra tegn)
        scanf(" %d", &random_valg);
    }
        
        if (random_valg == 1) {
        random_by = find_random_by(by_array, antal_byer);
        printf("\nVi foreslår at du rejser til %s.\n", random_by.navn);
        printf("\n%s er %s\n", random_by.navn, random_by.beskrivelse);
        print_CO2_beregning(random_by);
        printf("\nTak for nu\n");
        exit(EXIT_FAILURE);
        } else {
            printf("\nTak for nu\n");
            exit(EXIT_FAILURE);
    }
    }
    
    
} 

//funktion til at udskrive random by
By find_random_by(By by_array[], int *antal_byer){

    //by_array fyldes op igen
    laes_fra_data_fil("data.csv", by_array, antal_byer);
    laes_fra_beskrivelser_fil("beskrivelser.csv", by_array);
    
    //find en tilfældig by
    int i = rand() % *antal_byer;

    //
    return by_array[i];
}

//funktion til filtrering af prisklasse
By *filtrer_pris(By by_array[], int *antal_byer, int prisklasse){
    int count = 0;

    for (int i = 0; i < *antal_byer; i++) {
        if ((prisklasse == 1 && strcmp(by_array[i].prisniveau, "Budget") == 0)       ||
           (prisklasse == 2 && strcmp(by_array[i].prisniveau, "Mellemklasse") == 0)  ||
           (prisklasse == 3 && strcmp(by_array[i].prisniveau, "Luksus") == 0)){
           by_array[count++] = by_array[i]; // Flyt byen frem i arrayet, hvis den opfylder kriteriet
           }

    }
    *antal_byer = count;    //opdaterer antallet af filtrerede byer
    return by_array;        //returner det nye array

}

//funktion til at opdatere by_array med byer der opfylder km_til_venedig og fjerne de andre
By *filtrer_radius(By by_array[], int *antal_byer, int valg_radius){
    int count = 0;

    for (int i = 0; i < *antal_byer; i++) {
        if ((valg_radius == 1 && by_array[i].km_til_Venedig <= 30 && by_array[i].km_til_Venedig >= 5) ||
            (valg_radius == 2 && by_array[i].km_til_Venedig <= 60 && by_array[i].km_til_Venedig > 31) ||
            (valg_radius == 3 && by_array[i].km_til_Venedig > 61)){
            by_array[count++] = by_array[i]; // Flyt byen frem i arrayet, hvis den opfylder kriteriet
        }
    }

    *antal_byer = count;    //opdaterer antallet af filtrerede byer
    return by_array;        //returner det nye array
}

//funktion til at opdatere by_array med byer der opfylder oplev_1 og fjerne de andre
By *filtrer_oplevelse(By by_array[], int *antal_byer, int valg_opl){
    int count = 0;

    for (int i = 0; i < *antal_byer; i++) {
        if ((valg_opl == 1 && strcmp(by_array[i].oplev_1, "Landskab") == 0) ||
            (valg_opl == 2 && strcmp(by_array[i].oplev_1, "Historisk") == 0) ||
            (valg_opl == 3 && strcmp(by_array[i].oplev_1, "Vand") == 0)) {
            by_array[count++] = by_array[i]; // Flyt byen frem i arrayet, hvis den opfylder kriteriet
        }
    }

    *antal_byer = count;    //opdaterer antallet af filtrerede byer
    return by_array;        //returner det nye array
}

//funktion til at opdatere by_array med byer der opfylder oplev_1 + oplev_2 og fjerne de andre
By *filtrer_oplevelse2(By by_array[], int *antal_byer, int valg_opl){
    int count = 0;

    for (int i = 0; i < *antal_byer; i++) {
        if ((valg_opl == 1 && strcmp(by_array[i].oplev_2, "Aktiv") == 0) ||
            (valg_opl == 2 && strcmp(by_array[i].oplev_2, "Afslapning") == 0) ||
            (valg_opl == 3 && strcmp(by_array[i].oplev_2, "Mad og vin") == 0)) {
            by_array[count++] = by_array[i]; // Flyt byen frem i arrayet, hvis den opfylder kriteriet
        }
    }

    *antal_byer = count;    //opdaterer antallet af filtrerede byer
    return by_array;        //returner det nye array
}

//funktion til udskrivning af CO2-beregning
void print_CO2_beregning(By by){
    int transportmiddel_1;
    int transportmiddel_2;
    int antal_traer;
    double CO2;

    printf("\nHvordan ønsker du at rejse?\n1: fly\n2: bus\n3: elbil\n4: tog\n");
    scanf(" %d", &transportmiddel_1);

    while (transportmiddel_1 != 1 && transportmiddel_1 != 2 && transportmiddel_1 != 3 && transportmiddel_1 != 4){
        printf("\nUgyldig værdi. Vælg fra listen:\n1: fly\n2: bus\n3: elbil\n4: tog\n");
        //hvis man taster andet end 1, 2 eller
        while(getchar() != '\n');  // Fjerner resten af inputtet (fx bogstaver eller ekstra tegn)
        scanf(" %d", &transportmiddel_1);
    }
    
    //Hvis 1 (fly) vælges, medregnes transport fra Venedig lufthavn til by
    if (transportmiddel_1 == 1){
        CO2 = beregn_co2_udledning(by.km_DK_Vluft, transportmiddel_1);
        printf("\nHvordan ønsker du at komme fra Venedig lufthavn til %s?\n2: bus\n3: elbil\n4: tog\n", by.navn);
        scanf(" %d", &transportmiddel_2);

        while (transportmiddel_2 != 2 && transportmiddel_2 != 3 && transportmiddel_2 != 4){
            printf("\nUgyldig værdi. Vælg fra listen:\n\n2: bus\n3: elbil\n4: tog\n");
            //hvis man taster andet end 1, 2 eller
            while(getchar() != '\n');  // Fjerner resten af inputtet (fx bogstaver eller ekstra tegn)
            scanf(" %d", &transportmiddel_2);
        }

        CO2 += beregn_co2_udledning(by.km_Vluft_by, transportmiddel_2);
    } else {
        CO2 = beregn_co2_udledning(by.km_DK_by, transportmiddel_1);
    }


    //Et træ absorberer ca 20 kg C02 pr. år. Det er omskrevet til gram, derfor 20.000.
    antal_traer = CO2 / 20000;

    if (antal_traer > 1 || antal_traer == 0){
        printf("\nDin rejse udleder omkring %0.2lf gram CO2 pr. passager.\nDer skal %d træer til for at absorbere det på et år.\n", CO2, antal_traer);
    } else {
        printf("\nDin rejse udleder omkring %0.2lf gram CO2 pr. passager.\nDer skal %d træ til for at absorbere det på et år.\n", CO2, antal_traer);
    } 
}

// Funktion til beregning af CO2-udledning
double beregn_co2_udledning(double afstand, int transportmiddel) {
    // CO2-udledning pr. km i gram pr passager.
    const double CO2_FLY = 255.0;       
    const double CO2_BUS = 105.0;        
    const double CO2_ELBIL = 53.0;      
    const double CO2_TOG = 6.0;        

    switch (transportmiddel) {
        case 1:
            return afstand * CO2_FLY; 
            break;
        case 2:
            return afstand * CO2_BUS;
            break;
        case 3:
            return afstand * CO2_ELBIL;
            break;
        case 4: 
            return afstand * CO2_TOG;
            break;
        default:
            return -1;
    }
}

//funktion til at lægge alle byer over i by_array
void laes_alle_byer(FILE *file, By by_array[], int *length){
    By by;
    int i = 0;

    //læser headeren først
    char header[512];
    fgets(header, sizeof(header), file);

    //indlæser filen i by_array
    while (fscanf(file, "%d,%49[^,],%19[^,],%d,%19[^,],%19[^,],%d,%d,%d\n", 
                   &by.index, by.navn, by.prisniveau, &by.km_til_Venedig, 
                   by.oplev_1, by.oplev_2, &by.km_DK_by, &by.km_DK_Vluft,
                   &by.km_Vluft_by) == 9) { 
        by_array[i] = by;
        i++;

        //stopper indlæsning, hvis vi når slutningen af filen eller overskrider arraystørrelsen
        if (i >= ANTAL_BYER) {
            break;
        }
    }
    *length = i;
}

//funktion til at sortere antal km fra Venedig, hvor lavest rangeres højest
int Sammenlign_Kilometer(const void *a, const void *b) {
    By *by1 = (By *)a;
    By *by2 = (By *)b;

    return by1->km_til_Venedig - by2->km_til_Venedig;
}

//funktion til at indlæse  by-csvfil
int laes_fra_data_fil(const char *filnavn, By by_array[], int *antal_byer){
    FILE *file = fopen("data.csv", "r");
    if (file == NULL){
        exit(EXIT_FAILURE);
    } else {

        //indlæs alle byerne i et array af structs:
        laes_alle_byer(file, by_array, antal_byer);

        fclose(file);
    }
    return 1;
}

//funktion til at indlæse by-beskrivelser i hver by
int laes_fra_beskrivelser_fil(const char *filnavn, By by_array[]) {
    FILE *file1 = fopen("beskrivelser.csv", "r");
    if (file1 == NULL) {
        exit(EXIT_FAILURE);
    }

    char header[512];
    int index;
    char beskrivelse[500];

    // Læs headeren
    fgets(header, sizeof(header), file1);

    // Læs hver linje og match index
    while (fgets(header, sizeof(header), file1)) {
        if (sscanf(header, "%d, %[^\n]", &index, beskrivelse) == 2) {
            for (int i = 0; i < ANTAL_BYER; i++) {
                if (by_array[i].index == index) {
                    by_array[i].beskrivelse = strdup(beskrivelse); // alloker og kopier beskrivelsen
                    break; 
                }
            }
        }
    }

    fclose(file1);
    return 1;
}
