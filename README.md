# Encryption-Decryption-Project
---
 This project implements an image encryption/decryption mechanism.
 
 ![Capture](https://user-images.githubusercontent.com/49486605/75119560-a7de5f00-568c-11ea-9d28-3bcb832ba485.PNG)

 
## The first part: Encryption
 The project receives one txt file that contains the path to an image, a name for the future encrypted image and the secret key used in the encryption algorithm.
 
 Example: 
 
     peppers.bmp (the name of the image)
     
     imagine_criptata.bmp (the name for the encrypted image)
     
     secret_key.txt (the file with the secret key)
 
![criptare](https://user-images.githubusercontent.com/49486605/75119766-91390780-568e-11ea-92a8-fb17a18703bd.png)


## The second part: Decryption

 The project receives one txt file that contains the path to the encrypted image, a name for the future decrypted image and the secret key used in the decryption algorithm.
 
  Example: 
 
     imagine_criptata.bmp (the name of the encrypted image)
     
     imagine_decriptata.bmp (the name for the decrypted image)
     
     secret_key.txt (the file with the secret key)
     
![decriptare](https://user-images.githubusercontent.com/49486605/75119857-613e3400-568f-11ea-9b70-3b332c20c179.png) 

## The third part: The chi square test

The chi square test is a statistical method assessing the goodness of fit between a set of observed values and those expected theoretically.

 
## Description of the functions

1. void XORSHIFT32(unsigned int **R, unsigned int dim, unsigned int r)

   Pornind de la valoarea r, această funcţie furnizează un vector de numere pseudo-aleatoare de dimensiune dim. Numerele sunt obtinute      cu ajutorul operatiilor pe biti.
   
2. void liniarizare(const char *cale_imagine, Pixel **L, unsigned char **H, unsigned char *val_padding )

    Aceasta funcţie primeşte calea unei imagini bmp si furnizează un vector în care sunt reţinuţi pixeli imaginii, un vector în care         este reţinut header-ul imaginii şi o variabilă care reţine valoarea padding-ului.
    Imaginea este considerată un fişier binar care se deschide pentru citire. Se copiază primii 54 de octeţi din fisier în vectorul H       reprezentând zona header-ului. Cu ajutorul informaţiilor din header se afla laţimea şi înălţimea imaginii şi se calculează valoarea     padding-ului. Se copiază fiecare linie a imaginii în vectorul L.
    
3. void salvare_memorie_externa(const char *cale_imagine, Pixel *L, unsigned char *H, unsigned char val_padding)

   Această funcţie implementeză operaţia inversă funcţiei de liniarizare. Scrie într-un fişier binar valorile header-ului şi apoi          fiecare linie a imaginii (folosind vectorul în care aceasta este salvată în formă liniarizată) adaugand la finalul liniei octetii de    padding.
   
4. Pixel xorpp (Pixel P1, Pixel P2)

   Acesta functie implementează operaţia de XOR dintre doi pixeli şi returnează pixelul rezultat.
   
5. Pixel xorpnr(Pixel P1, unsigned int x)

   Acesta functie implementează operaţia de XOR dintre un pixel şi un număr natural şi returnează pixelul rezultat.
   
6. void criptare (const char * cale_imagine_initiala, const char * cale_imagine_criptata, const char * cale_fisier_text)

   Funcţia foloseşte prima valoare din fişierul text care conţine cheia secretă şi generează folosind funcţia XORSHIFT32 dublul            numărului de pixeli din imagine numere pseudo-aleatoare, apoi cu ajutorul primei jumătăţi a numerelor şi a imaginii în formă            liniarizată se generează o permutare aleatoare folosind algoritmul lui Durstenfeld. Imaginea criptată se obţine prin aplicarea          relaţiei de substituţie, din cerinţă, imaginii permutate, folosind a doua valoare din fişierul text şi restul de numere pseudo-          aleatoare. Imagirea obţinută se salvează în memoria externă folosind funcţia descrisă mai sus.
   
7. void decriptare (const char * cale_imagine_initiala, const char * cale_imagine_criptata, const char * cale_fisier_text )

   Funcţia calculează permutarea aleatoare în acelaşi mod ca funcţia anterioară, apoi calculează permutarea inversă. Se aplică formula      de substituţie folosind a doua valoare din fisierul text şi imaginea criptată în formă liniarizată. Se obţine imaginea intermediară,   ai cărei pixeli se interschimbă conform permutării inverse. Imaginea rezultată se salvează în memorie folosind funcţia descrisă         anterior.
   
8. void testul_chi (const char * cale_imagine)

   Funcţia calculeză frecvenţa estimată, frecvenţa fiecărui pixel al imaginii pe fiecare canal de culoare şi afişează valorile testului    chi calculate cu formula.
