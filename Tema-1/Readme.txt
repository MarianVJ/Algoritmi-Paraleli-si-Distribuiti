					Tema 1 APD
						Jorascu Marian Vladut 335CC
						


Part 1. Super Sampling Anti Aliasing

In cadrul acestei parti a trebuit sa micsoram o imagine folosing procedeul
SSAA in care fiecare piel din imaginea finala reprezinta mai multi pixeli
din imaginea originala.
In realizarea acestei probleme am folosit structura definita deja in fisierul
homework.h numita image , in care am incapsulat toate informatiile nececsare
pentru a putea retine o imagine grayscale sau color. Astfel, structura contine
un camp type de tip unsigned char , care este 6 daca imaginea este de tip
color ( P6 pe prima linie) sau care are valoarea 5 daca este de tip
grayscale (P5) . Campurile width si height retin numarul de coloane ,respectiv
numarul de linii ale unei imagini, si maxval valoarea maxima a unui pixel.
Cum o imagine este reprezentata ca o matrice de pixeli , in functie de tipul
acesteia voi aloca pe heap o amtrice de unsigned-charuri pentru o matrice
grayscale denumita "mat_gray" , iar in cazul in care avem de redimensionat
o imagine color , pixelii acestuia sunt reprezentate prin 3 valori pe 
1 octet . Astfel mi-am mai creeat o sructura auxiliara denumita rgb , 
care contine 3 unsigned char-uri.Campul in care va fi alocata matricea 
pt imagini color in tipul image se numeste "mat_col".

In cele ce urmeaza voi descrie fiecare functie din fisierul hoemwork.c

Functia readInput primeste ca argument numele fisierului , si o variabila
in care trebuie retinuta imaginea . Pentru inceput citesc linie cu linie
(3 astfel de citiri). Dupa care parsez datele : pentru prima linie
completez in variabila de tip image , campul type in functie de dipul imaginii.
De pe linia a doua extrag dimensiunile , iar de pe linia 3 valoarea maxima.
Apoi conform cu  tipul imaginii , se aloca campul corespunzator (
mat_col sau mat_gray) . Cand citesc efectiv bitii , citesc cate o linie
la fiecare pas ( nu pixel cu pixel ). 

Functia writeData  scrie in fisierul de output , noua imagine obitnuta dupa
redimensionarea celei initiale.

Functia resize se initializeaza noua variabila de tip image , la 
dimensiunile corespunzatoare . Am decis sa aloc si matricea de pixeli
necesara. Initializam Kernelul Gausian, dupa care pornim cele num_threaduri 
sa execute in paralel functia threadFunction care realizeaza efectiv operatia
de resize ceruta in cerinta .


Functia treadFunction. Aceasta functie este executata de fiecare thread in parte
si primeste ca argument id-ul threadului. Pentru a accesa atat imaginea initiala
cat si cea pe care urmeaza sa o creem , am folosit doi pointeri globali .
De asemenea am alocat tot global si matricea ce retine kernelul gaussian si
o variabila rap care retine raportul dintre dimensiunea imaginii intiale
(Liniile) si podusul dintre numarul de threaduri si resize_factor. Astfel,
primele num_threaduri-1 vor executa acelasi numar de operatii , iar 
ultimul thread in cazul in care impartirea nu se realizeaza exact va efectua 
un nr mai mare.Variabilele locale din fiecare functie st si fn retin 
indicele primei linii de unde se face operaatia de resize, respectiva ultima.
Parcurg fiecare portiune din resize_factor in resize_factor elemente atat
pe linii , cat si pe coloane .In functie de tipul imaginii
Avem 2 cazuri : imaginea este grayscale sau color, si independent
de tip operatia se realizeaza diferit in functie de resize_factor, unde 
si aici avem doua cazuri cand este 3 sau cand este multiplu de 2.
 Am preferat sa rescriu cele 4 foruri 
in fiecare caz , pentru a evita la fiecare pas sa verific in 
care din ele sunt( introduceam in numar de operatii in plus).

Part 2. Micro Render

Pentru aceasta cerinta imaginile sunt doar grayscale, iar structura image
din fisierul homework1.h are mai putine campuri , deoarece imaginea pe care
trebuie sa o creem e patratica ( width  = height ) ,type pentru tip, maxval
si o matrice de unsigned char-uri ce urmeaza a fi alocata dednumita mat_gray.


Functia initialize : initializeaza campurile din imagine , si aloca matricea
de pixeli.


Functia writeData scrie imaginea creeata.

Functia render. In cadrul acestei functii se intializeaza variabila globala 
rap , care retine raportul dintre dimensiunea imaginii , si numarul de threaduri.
Primele num_threads - 1 threaduri executa acelasi numar de operatii , iar ultimul
thread in functie de datele de intrare va preluca mai multe/mai putine linii.
Pornim cele num_threaduri care vor executa in paralel functia threadFunction 
care realizeaza operatia de render , pentru portiunea corespunzatoare threadului.


Functia threadFunction primeste ca argument id-ul threadului si pentru 
prelucrarea imaginii acceseaza un pointer global catre varibiala de tip image.
Pentru fiecare pixel in parte , calculez distanta de la el la linia determinata
de dreapta -1 x  + 2 y + 0 = 0,
Folosind formula dist = |a xa + b ya + c| / sqrt(a^2  + b ^ 2). Imaginea
la care ne raportam este de 100 x 100 .Am scalat pozitiile
fiecarui pixel in parte la dimensiunea
functie de rezolutie
, dar am luat de fiecare data in calcul si jumatate din acesta,
jumatate ce de asemenea a fost scalata. Calculam distanta cu formula prezentata
mai sus , iar daca distanta este mai mica sau 
egala de 3cm , valoarea pixelului este de 0 ( negru ,apartine liniei ) 
 sau alb 255.
 
 
Scalabilitatea a fost testata atat de mine pe calculatorul propriu
(cat si folosind checkerul pus la dispozitie de echipa de la APD)
folosind diferite valori pentru "num_threads", iar pe masura ce crestem
numarul de procesoare timpul de executie pentru ambele programe se diminueaza.
Dar daca numarul de threaduri depaseste numarul de procesoare fizice , timpul
de exxecutie tot este mai mic decat atunci cand am folosit mai putine threaduri
decat coruri , insa timpul minim este atunci cand threads = numarul de coruri
fizice.

