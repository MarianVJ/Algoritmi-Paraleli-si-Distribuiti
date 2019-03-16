					README 
					Tema 2 APD - Jorascu Marian Vladut 335CC
					
					
 In cadrul acestei teme am implementat toate metodele din clasa
 Miner si CommunicationChanel . Singura metoda care a fost adaugata
 este in clasa Miner si se numeste "encryptThisString" care are
 rolul sa faca hash de un nr N peste mesajul din fiecare camera
 care este trimis ca parametru. 
 (Functia este preluata din fisierul Main din folderul solver).
   
In acest "CANAL GENERIC" sunt 2 canale mai exact , unul in care scriu
Minerii si citesc Vrajitorii , iar altul in care scriu Vrajitorii si
citesc Minerii. Am simulat aceste doua canale cu ajutorul unei 
structuri de date , pe care am folosit o si in cadrul unui laborator
"ArrayBlockingQueue" , care realieaza operatii precum insertia si
stergerea unul element in mod atomic. Deoarece la fiecare operatie
pe care o rezolva Minerii , ei trimit un singur Mesaj care contine
id-ul tatalui , mesajul "rezolvat" si id-ul nodului curent , operatiile pentru
acest canal sunt efectiv adaugarea si scoaterea din coada. Problema
intervine cand se doreste implementarea canalului pe care scriu Vrajitorii.
Deoarece acestia pentru fiecare nod trimit doua mesaj , unul care contine
id-ul tatalui inainte , iar apoi unul cu id-ul nodului curent si mesajul
pe care unul dintre Mineri trebuie sa aplice
functia de hash de N ori. De asemenea,
Vrajitorii de fiecare data trimit si un mesaj de "END" dupa
ce descopera toate camerele adiacente camerei curente.
Pentru ca nu folosesc in Miner acest tip de mesaje , ele
nici nu vor mai fi trimise catre acestia. 

Vrajitorii au un comportament foarte ciudat , ei intra intr-o stare de 
somn pentru o perioada de timp , apoi incep sa trimita pe canal 
mesajele. Pentru a pastra ordinea la nivel de mesaje-interdependente
(Mesajul 1 cu id tata - MEsaj 2 cu id fiu )
astfel incat sa nu se intercaleze intre ele in coada 
Am luat un vector m , de tippul Message , in care sunt memorate
primele mesaje venite de la Vrajitori cele care contin id-ul tatalui
, iar apoi cand se va primi si Mesajul cu id-ul fiului , se va prelua
controlul asupra obiectului channel ( care este partajat atat
intre mineri cat si intre vrajitori) , si cele doua sunt scrise .
Astfel, Minerii sunt sigur ca daca citesc 2 mesaje consecutive
ele au si semnificatia din enunt. 
Am folosit metoda getId() din clasa Thread . Iar m[getId()] contine
primul din seria de 2 mesaje, iar in momentul cand va fi trimis si al doilea 
se preia controlul canalului si sunt scrise amandoua.
