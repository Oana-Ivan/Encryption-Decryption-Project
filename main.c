#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
///Modulul de criptare/decriptare
typedef struct {
    unsigned char B, G, R;
} Pixel;

// implementare generator de numere pseudo-aleatoare
void XORSHIFT32 (unsigned int **R, unsigned int dim, unsigned int r) {
    unsigned int i;
    (*R) = (unsigned int *)malloc(dim * sizeof(unsigned int *));
    for (i = 0; i < dim; i++) {
        r = r ^ r << 13;
        r = r ^ r >> 17;
        r = r ^ r << 5;
        (*R)[i] = r;
    }
}
// incarcare imagine BMP in memoria interna in forma liniarizata
void liniarizare (const char *cale_imagine, Pixel **L, unsigned char **H, unsigned char *val_padding )
{
    FILE *f = fopen(cale_imagine, "rb");
    if (f == NULL) {
        printf("Imaginea nu a fost gasita.\n");
        return;
    }
    (*H) = (unsigned char*)malloc(54 * sizeof(unsigned char));
    if ((*H) == NULL) {
        printf("Nu exista suficienta memorie pentru a memora header-ul imaginii.\n");
        return;
    }
    int i, j, k = 0;
    for (i = 0; i < 54; i++) fread(&((*H)[i]), sizeof(unsigned char), 1, f);

    int latime, inaltime;
    fseek(f, 18, SEEK_SET);
    fread(&latime, sizeof(int), 1, f);
    fread(&inaltime, sizeof(int), 1, f);

    unsigned char padding;
    if ((3*latime)%4 == 0) padding = 0;
    else {
        padding = 4 - (3 * latime)%4;
        fseek (f, -1, SEEK_END);
        fread (&(*val_padding), sizeof(unsigned char), 1, f);
    }
    (*L) = (Pixel *)malloc(latime * inaltime * sizeof(Pixel));
    if ((*L) == NULL) {
        printf("Nu exista suficienta memorie pentru a memora imaginea in forma liniarizata.\n");
        return;
    }
    fseek(f, -(3 * latime + padding), SEEK_END);

    for (i = 0; i < inaltime; i++) {
        for (j = 0; j < latime; j++) {
            fread(&(*L)[k].B,sizeof(unsigned char), 1, f);
            fread(&(*L)[k].G,sizeof(unsigned char), 1, f);
            fread(&(*L)[k].R,sizeof(unsigned char), 1, f);
            k++;
        }
        fseek(f, -(2*3*latime+padding), SEEK_CUR);
    }
    fclose(f);
}
// salvarea unei imagini BMP in memoria externa
void salvare_memorie_externa(const char *cale_imagine, Pixel *L, unsigned char *H, unsigned char val_padding)
{
    FILE *f = fopen(cale_imagine,"wb+");
    if (f == NULL) {
        printf("Imaginea nu s-a putut deschide.\n");
        return;
    }
    int i;
    for (i = 0; i < 54; i++) fwrite(&H[i], sizeof(unsigned char), 1, f);

    int latime, inaltime;
    fseek (f, 18, SEEK_SET);
    fread (&latime, sizeof(int), 1, f);
    fread (&inaltime, sizeof(int), 1, f);

    unsigned int padding;
    if ((3 * latime)%4 == 0) padding = 0;
    else padding = 4-(3 * latime)%4;

    fseek (f, 54, SEEK_SET);
    int k = 1;
    while (k <= inaltime) {
        for (i = latime * inaltime - k * latime; i < latime * inaltime + (1 - k) * latime; i++)
            {
                fwrite (&L[i].B, sizeof(unsigned char), 1, f);
                fwrite (&L[i].G, sizeof(unsigned char), 1, f);
                fwrite (&L[i].R, sizeof(unsigned char), 1, f);
            }
        unsigned int aux;
        aux = padding;
        while (aux != 0) {
            fwrite (&val_padding, sizeof(unsigned char), 1, f);
            aux--;
        }
        k++;
    }
    fclose(f);
}
Pixel xorpp (Pixel P1, Pixel P2) { //xoreaza valorile a doi pixeli
    Pixel P;
    P.B = P1.B ^ P2.B;
    P.G = P1.G ^ P2.G;
    P.R = P1.R ^ P2.R;
    return P;
}
typedef union  {
    unsigned  int y;
    struct {
        unsigned char octet0;
        unsigned char octet1;
        unsigned char octet2;
        unsigned char octet3;
    };
}octeti;

Pixel xorpnr(Pixel P1, unsigned int x) { //xoreaza un pixel si un numar
    Pixel P;
    octeti t;
    t.y = x;
    P.B = P1.B ^ t.octet0;
    P.G = P1.G ^ t.octet1;
    P.R = P1.R ^ t.octet2;
    return P;
}
// criptarea unei imagini BMP
void criptare (const char * cale_imagine_initiala, const char * cale_imagine_criptata, const char * cale_fisier_text) {
    FILE *imgi, *ftxt;
    ftxt = fopen (cale_fisier_text, "r");
    if (ftxt == NULL) {
        printf("Nu s-a gasit fisierul text.\n");
        return;
    }
    unsigned int R0;
    fscanf (ftxt, "%d", &R0);

    imgi = fopen(cale_imagine_initiala,"rb");
    int latime, inaltime;
    fseek (imgi, 18, SEEK_SET);
    fread (&latime, sizeof(int), 1, imgi);
    fread (&inaltime, sizeof(int), 1, imgi);
    unsigned int dim = 2 * latime * inaltime - 1;

    unsigned int *R;
    XORSHIFT32(&R,dim,R0);

    int *Permutare, i;
    Permutare = (int *)malloc(latime*inaltime*sizeof(int));
    for (i = 0; i < latime * inaltime; i++) Permutare[i] = i;

    unsigned int a, aux, r;
    for ((i=latime*inaltime-1) && (r=0);(i>=0)&&(r<latime*inaltime); (i--) && (r++) ) {
        a = R[r]%(i + 1);
        aux = Permutare[i];
        Permutare[i] = Permutare[a];
        Permutare[a] = aux;
    }

    Pixel *L;
    unsigned char *H, val_padding;
    liniarizare(cale_imagine_initiala, &L, &H, &val_padding);

    Pixel *I; //I forma liniarizata a imaginii permutate
    I = (Pixel *)malloc(latime * inaltime * sizeof(Pixel));
    unsigned int k;
    for(i = 0;i < latime * inaltime; i++) {
        k = Permutare[i];
        I[k].B = L[i].B;
        I[k].G = L[i].G;
        I[k].R = L[i].R;
    }

    salvare_memorie_externa("imagine_permutata.bmp", I, H, val_padding);

    unsigned int SV;
    fscanf (ftxt, "%d", &SV);

    Pixel *C;
    C = (Pixel *)malloc(latime * inaltime * sizeof(Pixel));
    C[0] = xorpnr(I[0],SV);
    C[0] = xorpnr(C[0], R[latime * inaltime]);

    for (i = 1; i < inaltime * latime; i++) {
        C[i] = xorpp (C[i-1], I[i]);
        C[i] = xorpnr (C[i], R[latime * inaltime + i]);
    }

    salvare_memorie_externa(cale_imagine_criptata, C, H, val_padding);

    free(R);
    free(Permutare);
    free(I);
    free(C);
    free(L);
    free(H);
    fclose(imgi);
    fclose(ftxt);
}
// decriptarea unei imagini BMP criptata cu functia anterioara
void decriptare (const char * cale_imagine_initiala, const char * cale_imagine_criptata, const char * cale_fisier_text )
{
    FILE *imgc, *ftxt;
    ftxt = fopen (cale_fisier_text, "r");
    if (ftxt == NULL) {
        printf("Nu s-a gasit fisierul text.\n");
        return;
    }
    unsigned int R0;
    fscanf (ftxt, "%d", &R0);

    imgc = fopen (cale_imagine_criptata, "rb");
    int latime, inaltime;
    fseek (imgc, 18, SEEK_SET);
    fread (&latime, sizeof(int), 1, imgc);
    fread (&inaltime, sizeof(int), 1, imgc);
    unsigned int dim = 2 * latime * inaltime - 1;

    unsigned int *R;
    XORSHIFT32 (&R, dim, R0);

    //Permutare aleatoare Durstenfeld
    int *Permutare, i;
    Permutare = (int *)malloc(latime * inaltime * sizeof(int));
    for (i = 0; i < latime * inaltime; i++)
        Permutare[i] = i;

    unsigned int a, aux, r;
    for ((i = latime * inaltime - 1) && (r = 0); (i >= 0) && (r < latime * inaltime); (i--) && (r++) ) {
        a = R[r] % (i + 1);
        aux = Permutare[i];
        Permutare[i] = Permutare[a];
        Permutare[a] = aux;
    }

    int *Permutare_inversa;
    Permutare_inversa = (int *)malloc(latime * inaltime * sizeof(int));
    for (i = 0; i < latime * inaltime; i++) Permutare_inversa[Permutare[i]] = i;
    free(Permutare);

    Pixel *C;
    unsigned char *H, val_padding;
    liniarizare (cale_imagine_criptata, &C, &H, &val_padding);

    unsigned int SV;
    fscanf (ftxt, "%d", &SV);

    Pixel *I; //imagime intermediara
    I = (Pixel *)malloc(latime * inaltime * sizeof(Pixel));
    I[0] = xorpnr(C[0], SV);
    I[0] = xorpnr(I[0], R[latime*inaltime]);
    for (i = 1; i < inaltime * latime; i++) {
        I[i] = xorpp(C[i-1], C[i]);
        I[i] = xorpnr(I[i], R[latime * inaltime + i]);
    }

    Pixel *D; //D imaginea decriptata
    D = (Pixel *)malloc(latime*inaltime*sizeof(Pixel));
    unsigned int k;
    for (i = 0; i < latime * inaltime; i++) {
        k = Permutare_inversa[i];
        D[k].B = I[i].B;
        D[k].G = I[i].G;
        D[k].R = I[i].R;
    }

    salvare_memorie_externa(cale_imagine_initiala, D, H, val_padding);

    free(R);
    free(Permutare_inversa);
    free(I);
    free(C);
    free(D);
    free(H);
    fclose(imgc);
    fclose(ftxt);
}
// afisarea valorilor testului chi pentru o imagine BMP pe fiecare canal de culoare
void testul_chi (const char * cale_imagine) {
    FILE *f = fopen(cale_imagine, "rb");
    if (f == NULL) {
        printf("Imaginea nu a fost gasita.\n");
        return;
    }
    int latime, inaltime, padding;
    fseek (f, 18, SEEK_SET);
    fread (&latime,sizeof(int), 1, f);
    fread (&inaltime,sizeof(int), 1, f);
    if (latime % 4 == 0) padding = 0;
     else padding = 4 - (3 * latime) % 4;

    double *frecvente_r, *frecvente_g, *frecvente_b;
    frecvente_r = (double *)calloc(256, sizeof(double));
    frecvente_g = (double *)calloc(256, sizeof(double));
    frecvente_b = (double *)calloc(256, sizeof(double));

    int i, j;
    fseek (f, 54, SEEK_SET);
    Pixel P;
    for (i = 0; i < inaltime; i++) {
        for (j = 0; j < latime; j++) {
            fread(&P, sizeof(Pixel), 1, f);
            frecvente_r[P.R]++;
            frecvente_b[P.B]++;
            frecvente_g[P.G]++;
        }
        fseek (f, padding, SEEK_CUR);
    }
    fclose(f);

    double frecventa_estimata;
    frecventa_estimata = (double)latime * inaltime / 256;

    double suma_frecvente_r = (double)0, suma_frecvente_g = (double)0, suma_frecvente_b = (double)0;
    for (i = 0; i < 256; i++) {
        suma_frecvente_r = (double)suma_frecvente_r + (double)(frecvente_r[i] - frecventa_estimata) * (double)(frecvente_r[i] - frecventa_estimata);
        suma_frecvente_g = (double)suma_frecvente_g + (double)(frecvente_g[i] - frecventa_estimata) * (double)(frecvente_g[i] - frecventa_estimata);
        suma_frecvente_b = (double)suma_frecvente_b + (double)(frecvente_b[i] - frecventa_estimata) * (double)(frecvente_b[i] - frecventa_estimata);
    }

    double chi_r, chi_g, chi_b;
    chi_r = (double)suma_frecvente_r / frecventa_estimata;
    chi_g = (double)suma_frecvente_g / frecventa_estimata;
    chi_b = (double)suma_frecvente_b / frecventa_estimata;

    printf("R: %.2lf\nG: %.2lf\nB: %.2lf\n", chi_r, chi_g, chi_b);
    free(frecvente_r);
    free(frecvente_g);
    free(frecvente_b);

}
char * deschidereFisier (char *date) {
    char *nume;
    nume = (char *)malloc(256 * sizeof(char));
    fscanf (date, "%s", nume);
    unsigned int dimensiune = strlen(nume);
    nume = (char *)realloc(nume, (dimensiune + 1)*sizeof(char));
    return nume;
}
int main() {
    // Criptare
    char *nume_fisier_date_criptare;
    nume_fisier_date_criptare = (char *)malloc(256 * sizeof(char));

    printf ("Introduceti numele fisierului care contine calea imaginii initiala, calea imaginii criptate si a cheii secrete: ");
    scanf ("%s", nume_fisier_date_criptare);
    unsigned int dimensiune_nume_fisier_criptare = strlen(nume_fisier_date_criptare);
    nume_fisier_date_criptare = (char *)realloc (nume_fisier_date_criptare, (dimensiune_nume_fisier_criptare+1) * sizeof(char));

    FILE *date_criptare = fopen(nume_fisier_date_criptare,"r");
    if (date_criptare == NULL) {
        printf("Nu s-a gasit fisierul %s.\n", nume_fisier_date_criptare);
    }
    char *nume_imagine_initiala = deschidereFisier(date_criptare);

    char *nume_imagine_criptata = deschidereFisier(date_criptare);

    char *nume_fisier_cheie_secreta = deschidereFisier(date_criptare);
    fclose(date_criptare);
    criptare(nume_imagine_initiala, nume_imagine_criptata, nume_fisier_cheie_secreta);

    // Decriptare
    char *nume_fisier_date_decriptare;
    nume_fisier_date_decriptare=(char *)malloc(256*sizeof(char));
    printf("Introduceti numele fisierului care contine calea imaginii criptate, calea imaginii decriptate si a cheii secrete: ");
    scanf("%s",nume_fisier_date_decriptare);
    unsigned int dimensiune_nume_fisier_decriptare=strlen(nume_fisier_date_decriptare);
    nume_fisier_date_decriptare=(char *)realloc(nume_fisier_date_decriptare,(dimensiune_nume_fisier_decriptare+1)*sizeof(char));

    FILE *date_decriptare=fopen(nume_fisier_date_decriptare,"r");
    if(date_decriptare==NULL)
    {
        printf("Nu s-a gasit fisierul %s.\n",nume_fisier_date_decriptare);
    }
    char *nume_imagine_initiala_criptata = deschidereFisier(date_decriptare);

    char *nume_imagine_decriptata = deschidereFisier(date_decriptare);

    char *nume_fisier_cheie_secreta2 = deschidereFisier(date_decriptare);

    fclose(date_decriptare);
    decriptare(nume_imagine_decriptata,nume_imagine_initiala_criptata,nume_fisier_cheie_secreta2);

    // Testul CHI patrat
    printf("Valorile testului CHI patrat pentru imaginea initiala:\n");
    testul_chi (nume_imagine_initiala);

    printf ("Valorile testului CHI patrat pentru imaginea criptata:\n");
    testul_chi (nume_imagine_criptata);
    return 0;

}
