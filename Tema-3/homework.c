#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Aceasta se va utiliza aceasta structura atunci cand imaginea este color
typedef struct {
	
	unsigned char r;
	unsigned char g;
	unsigned char b;
	
} rgb;

typedef struct {
	//5 pentru P5 si 6 pentru P6
	unsigned char type;
	
	//Dimensiunile matricii
	int width ;
	int height;
	
	//Valoarea maxima a unui pixel
	int maxval;
	
	//Aceasta matrice va fi folosita pentru imaginile color
    rgb **mat_col;
	
	//Iar aceasta atunci cand trebuie sa facem operatia de resize pe o imagine
	//grayscale
	unsigned char **mat_gray;
	
}image;



void readInput(const char * fileName, image *img) {

	//Deschid fisierul pentru citire
	FILE *f;
	f = fopen(fileName, "r");
	
	char *type = NULL;
	char *buffer;
	size_t size; 
	
	//Citesc prima linie care specifica tipul imaginii (color sau grayscale)
	getline(&type, &size, f);
	
	//Completez structura ce retine imaginea corespunzator
	if(type[1] == '6'){
		img->type = 6;
	}
	else{
		img->type = 5;
	}
		
	//Citesc a doua linie in care sunt scrise dimensiunile matricii
    //in format ASCII
	size = 0;
	getline(&buffer, &size, f);
	
	
	//Parsam datele de pe prima linie pentru a afla 
	//dimensiunile imaginii
	int i = 0,j = 0;
	char aux[10];
	while(buffer[i] != ' '){
		aux[i] = buffer[i];
		i++;
	}
	aux[i] = 0;
	img->width = atoi(aux);
	
	i++;
	while(buffer[i] != '\n'){
		aux[j] = buffer[i];
		i++;
		j++;
	}
	aux[j] = 0;
	img->height = atoi(aux);
	
	//Pe a treia linie este scrisa valoarea maxima ce o poate avea un pixel
	buffer = NULL;
	size = 0;
	getline(&buffer, &size, f);
	
	//parsam maxval
	i = 0;
	while(buffer[i] != '\n'){
		aux[i] = buffer[i];
		i++;
	}
	aux[i] = 0;
	img->maxval  = atoi(aux);
	
	//In functie de tipul imaginii alocam matricea corespunzatoare
	if(img->type == 5){
		img->mat_gray = malloc(sizeof(unsigned char*) * img->height);
		for(i = 0 ; i < img->height ; i++){
			img->mat_gray[i] = malloc(sizeof(unsigned char) * img->width);
			fread(img->mat_gray[i], sizeof(unsigned char), img->width, f);
		}
	}
	else{
		img->mat_col = malloc(sizeof(rgb *) * img->height);
		for(i = 0 ; i < img->height ; i++){
			img->mat_col[i] = malloc(sizeof(rgb) * img->width);
			fread(img->mat_col[i], sizeof(rgb), img->width, f); 
		}
	}

	//Inchidem fisierul din care am citit imaginea initiala
	fclose(f);
}

void writeData(const char * fileName, image *img) {
	
	//Deschidem fisierul pentru scriere
	int i;
	FILE *f;
	f = fopen(fileName, "w");
	
	//In functie de tipul imaginii se realizeaza scrierea in conformitate cu
	//formatul (RGB cu 3 valori diferite , sau o singura valoare in cazul
	//grayscale)
	if(img->type == 5){
		fprintf(f, "P5\n");
		fprintf(f, "%d %d\n", img->width, img->height);
		fprintf(f, "%d\n", img->maxval);
		
		for(i = 0 ; i < img->height ; i ++)
			fwrite(img->mat_gray[i], sizeof(unsigned char), img->width, f);
	}
	else{
		
		fprintf(f ,"P6\n");
		fprintf(f ,"%d %d\n", img->width, img->height);
		fprintf(f ,"%d\n", img->maxval);
		
		for(i = 0 ; i < img->height ; i++){
			fwrite(img->mat_col[i], sizeof(rgb), img->width, f);
		}
	}
	//Dupa ce am terminat de realizat scrierea inchidem fisierul
	fclose(f);
}





void copiere_matrice(float flt[3][3], float** matrice){
	int i, j;
	for(i = 0  ;  i < 3 ; i++)
		for(j = 0 ; j < 3	 ; j++)
			matrice[i][j] = flt[i][j];
}


void gaseste_filtru(char* nume, float** matrice){
	
float smooth[3][3] = {
					{(float)1/9,(float)1/9,(float)1/9},
					{(float)1/9,(float)1/9,(float)1/9},
					{(float)1/9,(float)1/9,(float)1/9}
					};
					
float blur[3][3] = {
					{(float)1/16,(float)2/16,(float)1/16},
					{(float)2/16,(float)4/16,(float)2/16},
					{(float)1/16,(float)2/16,(float)1/16},
					};
					
float mean[3][3] = {
					{(float)-1,(float)-1,(float)-1},
					{(float)-1,(float)9,(float)-1},
					{(float)-1,(float)-1,(float)-1}
					};		

float emboss[3][3] = {
					{(float)0,(float)1,(float)0},
					{(float)0,(float)0,(float)0},
					{(float)0,(float)-1,(float)0},
					};			

float sharpen[3][3] = {
				     {(float)0/3,(float)-2/3,(float)0/3},
					 {(float)-2/3,(float)11/3,(float)-2/3},
					 {(float)0/3,(float)-2/3,(float)0/3}
					};						

	if(strcmp(nume,"smooth") == 0)
		copiere_matrice(smooth,matrice);
	else if(strcmp(nume,"blur") == 0)
			copiere_matrice(blur,matrice);
		 else if(strcmp(nume,"mean") == 0)
				copiere_matrice(mean,matrice);
			  else if(strcmp(nume,"emboss") == 0)
					    copiere_matrice(emboss,matrice);
					else if(strcmp(nume,"sharpen") == 0)
							copiere_matrice(sharpen,matrice);
}
int main(int argc, char* argv[]) {

	//Numarul de procese
	int size;
	//Id-ul fiecarui proces
	int rank;
	image im;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	//m - numarul liniilor , n - coloanele
	int n, m, rap, i, j;
	m = n = rap = 0;
	//indica tipul imaginii
	//6 pt imaginile de tip color , 5 pt grayscale
	unsigned char type = 0;

		
	if(rank == 0){
		//Citesc imaginea 
		readInput(argv[1],&im);
		n = im.width;
		m = im.height;
		type = im.type;
		rap = m / size;
		//Trimit dimensiunile imaginii si tipul acestora catre celalte fire de executie 
		for(i = 1 ; i < size ; i++){
			MPI_Ssend(&m, 1, MPI_INT, i, 99, MPI_COMM_WORLD);
			MPI_Ssend(&n, 1, MPI_INT, i, 99, MPI_COMM_WORLD);
			MPI_Ssend(&type, 1, MPI_UNSIGNED_CHAR, i, 99, MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Recv(&m, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
		MPI_Recv(&n, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);
		MPI_Recv(&type, 1, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
		rap = m / size;
	}
	
	//st si fn reprezinta indicii liniilor din matricea de pixeli
	//pe care thread-ul curent realizeaza aplicarea filtrului 
	int st, fn;
	st = rap * rank;
	fn = st + rap;
	if(rank == size - 1){
		fn = m;
	}
	
	//Aceste imaginii sunt folosite in cazul in care avem 
	//de aplicat filtre pe o imagine color , im1 va retine
	//pixelii r , im2 - g, im3- b , in matricile color
	image im1,im2,im3;
	
	if(type == 6){
			
		//mat1, mat2 si mat3 sunt stocate liniile corespunzatoare fiecarui thread 
		unsigned char **mat1;
		unsigned char **mat2;
		unsigned char **mat3;
		
		if(rank == 0){
			
			im1.mat_gray = malloc(sizeof(unsigned char *) * im.height);
			im2.mat_gray = malloc(sizeof(unsigned char *) * im.height);
			im3.mat_gray = malloc(sizeof(unsigned char *) * im.height);
			for(i = 0 ; i < im.height ; i++){
				im1.mat_gray[i] = malloc(sizeof(unsigned char) * im.width);
				im2.mat_gray[i] = malloc(sizeof(unsigned char) * im.width);
				im3.mat_gray[i] = malloc(sizeof(unsigned char) * im.width);
			}
			
			for(i = 0 ; i < m ; i++)
				for(j = 0 ; j < n ; j++){
					//Rosu verde si albastru (1 2 3)
					im1.mat_gray[i][j] = im.mat_col[i][j].r;
					im2.mat_gray[i][j] = im.mat_col[i][j].g;
					im3.mat_gray[i][j] = im.mat_col[i][j].b;
				}
		}
		else{
			if(rank != 0 ){
				//Pentru fiecare thread in parte , voi lua si 2 linii in plus ( pentru a
				//calcula prima si ultima linie ce corespund threadului curent)
				mat1 = malloc(sizeof(unsigned char*) * (fn-st+2));
				mat2 = malloc(sizeof(unsigned char*) * (fn-st+2));
				mat3 = malloc(sizeof(unsigned char*) * (fn-st+2));
				
				for(i = 0 ; i <= fn-st+1 ; i++){
					mat1[i] = malloc(sizeof(unsigned char) * (n+2));
					mat2[i] = malloc(sizeof(unsigned char) * (n+2));
					mat3[i] = malloc(sizeof(unsigned char) * (n+2));
				}
			}
		}
		
		if(rank  == 0){	
			for(i = 1 ; i < size ; i++){
					st = rap * i ;
				fn = st + rap ;
				if(i == size - 1){
					fn = m;
				}	
				for(j = st - 1 ; j < fn ; j++){
					MPI_Ssend(im1.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
					MPI_Ssend(im2.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
					MPI_Ssend(im3.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
				}
				
				//Trimit o linie in plus (pentru a calcula ultima linie ce apartine de threadull crt)
				if(i != size-1){
					MPI_Ssend((im1.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
					MPI_Ssend((im2.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
					MPI_Ssend((im3.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
				}
			}
		}
		else{
			
			for(i = 0; i < fn - st + 1; i++){
				MPI_Recv(mat1[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
				MPI_Recv(mat2[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
				MPI_Recv(mat3[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
			}
			//Am primit cate o linie in plus , pt a calcula linia de granita
			if(rank != size - 1){
				MPI_Recv(mat1[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
				MPI_Recv(mat2[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
				MPI_Recv(mat3[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
			}
		}
	
		//Calculez limitele si pt primul thread 
		if(rank == 0){
			st = rap * rank ;
			fn = st + rap ;
			if(rank == size-1){
				fn = m;
			}
		}
		
		//numarul de linii pe care le preluxreaza threadul curent 
		int mm = 0;
		
		//Fiecare are cate 2 linii de la vecinii 
		//pentru a calcula pixelii marginali ( exceptie primul thread si ultimul)
		mm = fn-st+2;
		
		int y,z;
		//Matrici auxiliare , ce sunt utilizate in momentul aplicarii filtrului
		unsigned char ** aux1;
		unsigned char ** aux2;
		unsigned char ** aux3;
		aux1 = malloc(sizeof(unsigned char*) * (mm));
		aux2 = malloc(sizeof(unsigned char*) * (mm));
		aux3 = malloc(sizeof(unsigned char*) * (mm));
		for(i = 0 ; i < mm ; i++){
			aux1[i] = malloc(sizeof(unsigned char) * (n));
			aux2[i] = malloc(sizeof(unsigned char) * (n));
			aux3[i] = malloc(sizeof(unsigned char) * (n));
		}
		//Sumele pentru fiecare pixel in parte 
		//sum1 - r, sum2 - g , sum3 - b
		float suma1,suma2,suma3;
		
		//Ultima linie trebuie sa ramana nemodificata
		//iar ultima linie este prelucrata de ultimul thread
		if(rank == size-1){
			mm-=1		;
		}
		if(size == 1)
			mm--;
		
		
		float** filtru;
		filtru  = malloc(sizeof(float*) * 3);
		for(i = 0 ; i < 3 ; i ++)
			filtru[i] = malloc(sizeof(float) * 3);
		int  ind;
		
		//Aplic pe rand fiecare filtru
		for(ind  = 3; ind < argc ; ind++){

			gaseste_filtru(argv[ind], filtru);
			if(rank == 0){
				
				//Retin copiile matricilor de pixeli
				for(i = 0 ; i < mm ; i++)
					for(j = 0 ; j < n ; j++){
						aux1[i][j] = im1.mat_gray[i][j];
						aux2[i][j] = im2.mat_gray[i][j];
						aux3[i][j] = im3.mat_gray[i][j];
					}
					
				for(i = 1 ; i < mm-1; i++){
					for(j = 1; j < n-1; j++){
								
						suma1 = suma2 = suma3 = 0;
						for(y = -1 ; y < 2 ; y++){
							for(z = -1 ; z <  2 ; z++){
								suma1 += (float)((float)aux1[i + y][j + z] * filtru[y + 1][z + 1]);
								suma2 += (float)((float)aux2[i + y][j + z] * filtru[y + 1][z + 1]);
								suma3 += (float)((float)aux3[i + y][j + z] * filtru[y + 1][z + 1]);
							}
						}
						//Modific pixelii , cu noua valoarea
						im1.mat_gray[i][j] = (unsigned char)suma1;
						im2.mat_gray[i][j] = (unsigned char)suma2;
						im3.mat_gray[i][j] = (unsigned char)suma3;
					}
				}
			}
			else{
				
				//Retin copiile matricilor de pixeli
				for(i = 0 ; i < mm ; i++){
					for(j = 0 ; j < n ; j++){
						aux1[i][j] = mat1[i][j];
						aux2[i][j] = mat2[i][j];
						aux3[i][j] = mat3[i][j];
					}
				}
				for(i = 1 ; i < mm-1; i++){
					for(j = 1; j < n-1; j++){
						
						suma1 = suma2 = suma3 = 0;
						for(y = -1 ; y < 2 ; y++){
							for(z = -1 ; z <  2 ; z++){
								suma1 += (float)((float)aux1[i + y][j + z] * filtru[y + 1][z + 1]);
								suma2 += (float)((float)aux2[i + y][j + z] * filtru[y + 1][z + 1]);
								suma3 += (float)((float)aux3[i + y][j + z] * filtru[y + 1][z + 1]);
							}
						}
						//Modific pixelii , cu noua valoarea
						mat1[i][j] = (unsigned char)suma1;
						mat2[i][j] = (unsigned char)suma2;
						mat3[i][j] = (unsigned char)suma3;
					}
				}
			}
			
			//Etapa in care iau de la toate threadurile liniile , dupa aplicarea filtrelor
			if(rank  == 0){
				for(i = 1 ; i < size ; i++){
					
					int st_aux;
					int fn_aux;
					st_aux = rap * i;
					fn_aux = st_aux + rap ;
					if(i == size - 1){
						fn_aux = m;
					}	
					//Deoarece trimit cu send , voi trimite pe porturi diferite , pentru fiecare culoare
					for(j = st_aux ; j < fn_aux ; j++){
						MPI_Recv(im1.mat_gray[j], n, MPI_UNSIGNED_CHAR, i, 99, MPI_COMM_WORLD, &status);
						MPI_Recv(im2.mat_gray[j], n, MPI_UNSIGNED_CHAR, i, 100, MPI_COMM_WORLD, &status);
						MPI_Recv(im3.mat_gray[j], n, MPI_UNSIGNED_CHAR, i, 101, MPI_COMM_WORLD, &status);
					}
				}
			}
			else{
				if(rank  == size-1)
				for(i = 1 ; i < mm ; i++){
					MPI_Send(mat1[i], n, MPI_UNSIGNED_CHAR, 0 , 99, MPI_COMM_WORLD);
					MPI_Send(mat2[i], n, MPI_UNSIGNED_CHAR, 0 , 100, MPI_COMM_WORLD);
					MPI_Send(mat3[i], n, MPI_UNSIGNED_CHAR, 0 , 101, MPI_COMM_WORLD);
				}
				else{
					for(i = 1 ; i < mm-1 ; i++){
						MPI_Send(mat1[i], n, MPI_UNSIGNED_CHAR, 0 , 99, MPI_COMM_WORLD);
						MPI_Send(mat2[i], n, MPI_UNSIGNED_CHAR, 0 , 100, MPI_COMM_WORLD);
						MPI_Send(mat3[i], n, MPI_UNSIGNED_CHAR, 0 , 101, MPI_COMM_WORLD);
					}
				}
			}
			
			MPI_Barrier(MPI_COMM_WORLD);
			
			//Retrimit apoi portiunile de matrice pe care trebuie aplicat  urmatorul filtru
			//fiecarui thread in parte
			if(ind < argc-1){
				if(rank  == 0){
					int st_aux,fn_aux;
					st_aux = fn_aux = 0;
					for(i = 1 ; i < size ; i++){
						st_aux = rap * i ;
						fn_aux = st_aux + rap ;
						if(i == size - 1){
							fn_aux = m;
						}	
						for(j = st_aux-1 ;  j < fn_aux ; j++){
							MPI_Ssend(im1.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
							MPI_Ssend(im2.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
							MPI_Ssend(im3.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
						}

						//Trimit o linie in plus 
						if(i != size-1){
							MPI_Ssend((im1.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
							MPI_Ssend((im2.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
							MPI_Ssend((im3.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
						}
					}
				}
				else{
					for(i = 0; i < fn-st+1; i++){
						MPI_Recv(mat1[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
						MPI_Recv(mat2[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
						MPI_Recv(mat3[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
					}
					//Am primit cate o linie in plus , pt a calcula linia de granita
					if(rank != size-1){
						MPI_Recv(mat1[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
						MPI_Recv(mat2[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
						MPI_Recv(mat3[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
					}
					
				}
			}
		}
	}
	else{
		
		//Variabilele au aceeasi semnificatie ca in cazul imaginilor de tip color
		//diferente este ca sunt de 3 ori mai putine 
		unsigned char** mat;
		if(rank != 0 ){
			mat = malloc(sizeof(unsigned char*) * (fn - st + 2));
			for(i = 0 ; i <= fn - st + 1 ; i++){
				mat[i] = malloc(sizeof(unsigned char) * (n + 2));
			}
		}
		if(rank == 0){
			for(i = 1 ; i < size ; i++){
				st = rap * i ;
				fn = st + rap ;
				if(i == size - 1){
					fn = m;
				}	
				for(j = st-1 ;  j < fn ; j++)
					MPI_Ssend(im.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
				
				//Trimit o linie in plus 
				if(i != size-1){
					MPI_Ssend((im.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
				}
			}
		}
		else{
			for(i = 0; i < fn - st + 1; i++){
				MPI_Recv(mat[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
			}
			//Am primit cate o linie in plus , pt a calcula linia de granita
			if(rank != size - 1){
				MPI_Recv(mat[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
			}
		}
		
		//Calculez limitele si pt primul thread 
		if(rank == 0){
			st = rap * rank ;
			fn = st + rap ;
			if(rank == size - 1){
				fn = m;
			}
		}
		//numarul de linii pe care fiecare thread le prelucreaza
		int mm = 0;
		//Fiecare are cate 2 linii de la vecinii 
		//pentru a calcula pixelii marginali ( exceptie primul thread si ultimul)
		mm = fn - st + 2;
		int y, z;
		unsigned char **aux;
		aux = malloc(sizeof(unsigned char*) * (mm));
		for(i = 0 ; i < mm ; i++){
			aux[i] = malloc(sizeof(unsigned char) * (n));
		}
		float suma;
		
		if(rank == size-1){
			mm -= 1	;	
		}
		if(size == 1){
			mm--;
		}
		
		
		float** filtru;
		filtru  = malloc(sizeof(float*) * 3);
		for(i = 0 ; i < 3 ; i ++)
			filtru[i] = malloc(sizeof(float) * 3);
		int  ind;
		for(ind  = 3; ind < argc ; ind++){
			
			gaseste_filtru(argv[ind],filtru);
			if(rank == 0){
				
				for(i = 0 ; i < mm ; i++)
					for(j = 0 ; j < n ; j++)
						aux[i][j] = im.mat_gray[i][j];
					
				for(i = 1 ; i < mm - 1; i++){
					for(j = 1; j < n - 1; j++){
								
						suma = 0;
						for(y = -1 ; y < 2 ; y++){
							for(z = -1 ; z <  2 ; z++){
								suma += (float)((float)aux[i + y][j + z] * filtru[y + 1][z + 1]);
							}
						}
						im.mat_gray[i][j] = (unsigned char)suma;
					}
				}
			}
			else{
				
				for(i = 0 ; i < mm ; i++){
					for(j = 0 ; j < n ; j++){
						aux[i][j] = mat[i][j];
					}
				}
				
				for(i = 1 ; i < mm-1; i++){
					for(j = 1; j < n-1; j++){
							
						suma = 0;
						for(y = -1 ; y < 2 ; y++){
							for(z = -1 ; z <  2 ; z++){
								suma += (float)((float)aux[i + y][j + z] * filtru[y + 1][z + 1]);
							}
						}
						mat[i][j] = (unsigned char)suma;
					}
				}
			}
			
			//Etapa in care iau de la toate threadurile informatiile necesare
			if(rank  == 0){
				for(i = 1 ; i < size ; i++){
					int st_aux;
					int fn_aux;
					st_aux = rap * i;
					fn_aux = st_aux + rap ;
					if(i == size - 1){
						fn_aux = m;
					}	
					for(j = st_aux ; j < fn_aux ; j++)
						MPI_Recv(im.mat_gray[j], n, MPI_UNSIGNED_CHAR, i, 99, MPI_COMM_WORLD, &status);
				}
			}
			else{
				if(rank  == size-1)
					for(i = 1 ; i < mm ; i++){
						MPI_Send(mat[i], n, MPI_UNSIGNED_CHAR, 0 , 99, MPI_COMM_WORLD);
				}
				else{
					for(i = 1 ; i < mm - 1 ; i++){
						MPI_Send(mat[i], n, MPI_UNSIGNED_CHAR, 0 , 99, MPI_COMM_WORLD);
					}
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);

			if(ind < argc - 1)
			{
				if(rank == 0){
					int st_aux,fn_aux;
					st_aux = fn_aux = 0;
					
					for(i = 1 ; i < size ; i++){

						st_aux = rap * i ;
						fn_aux = st_aux + rap ;
						if(i == size - 1){
							fn_aux = m;
						}	
						for(j = st_aux - 1 ;  j < fn_aux ; j++)
							MPI_Ssend(im.mat_gray[j], n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
						
						//Trimit o linie in plus 
						if(i != size-1){
							MPI_Ssend((im.mat_gray[j]), n, MPI_UNSIGNED_CHAR, i , 99, MPI_COMM_WORLD);
						}
					}
				}
				else{
					
					for(i = 0; i < fn - st + 1; i++){
						MPI_Recv(mat[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
					}
					//Am primit cate o linie in plus , pt a calcula linia de granita
					if(rank != size - 1){
						MPI_Recv(mat[i], n, MPI_UNSIGNED_CHAR, 0, 99, MPI_COMM_WORLD, &status);
					}
				}
		    }
		}
	}
		
	if(rank == 0){
		if(type == 5)
			writeData(argv[2],&im);
		else{
			for(i = 0 ; i < m ; i++)
				for(j = 0 ; j < n ; j++){
					//Rosu verde si albastru (1 2 3)
					 im.mat_col[i][j].r = im1.mat_gray[i][j] ;
					 im.mat_col[i][j].g = im2.mat_gray[i][j] ;
					 im.mat_col[i][j].b = im3.mat_gray[i][j] ;
				}
				writeData(argv[2],&im);
		}
	}
	
	MPI_Finalize();	
	return 0;
}