#include "homework1.h"
#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<math.h>


int num_threads;
int resolution;

//Vom modifica imaginea folosindu-ne de aceasta variabila globala
//In functia ce va fi executata de cele num_threads threaduri
image* im;
//Rap este raportul in care vor fi impartite liniile
//de numarul de threaduri ( similar cu problema precedenta, doar ca 
//aici , impartirea depinde doar de numarul lor)
int rap;

void initialize(image *im) {

	int i;
	//Imaginea are dimensiunile egale
	im->width = resolution;
	im->maxval = 255;
	//Alocam matricea in care vor fi stocati 
	//pixelii imaginii pe care o vom creea
	im->mat_gray = malloc(sizeof(unsigned char*) * im->width);
	for(i = 0 ; i < im->width ; i++)
		im->mat_gray[i] = malloc(sizeof(unsigned char) * im->width);
}


void* threadFunction(void *var)
{
	int thread_id = *(int*) var;
	int i,j;
	double x,y;
	double distanta;
	double scal_var = ((double) 100 / (double) resolution);
	double radical = sqrt(5.0f);
	
	//st si fn reprezinta indicii de start si stop ai liniilor
	//pentru care thread-ul curent va verifica care din pixelii
	//din aceasta zona apartin acelei linii determinata de 
	//dreapta -1 x  + 2 y + 0 = 0
	int st = rap * thread_id;
	int fn = st + rap;
	
	if(thread_id == num_threads - 1){
		fn = im->width;
	}
	
	for(i = st ; i < fn ; i++){
		for(j = 0 ; j < im->width ; j++){
				
			//Pentru fiecare pixel in parte , ii scalam pozitia raportat la 
			//o matrice de 100 de cm
			//Si de asemenea , avem grija sa scalam si jumate din acesta
			//conform cu specifiicatiile de pe forum
			y = (i + 0.5) * scal_var;
			x = (j + 0.5) * scal_var;
			
			//Distanta de la punctul curent pana la dreapta
			distanta = (abs((-1) * x + 2 * y) / radical);
			
			//Daca pentru indici coloana , numerotarea este la fel , pentru 
			//linia 0 va fi practic linia cea mai de sus din imagine , 
			//si actualizam in matricea de pixeli , valorile corespunzator
			
			//Daca distanta este mai mica de 3cm atunci pixelul este negru
			if(distanta <= 3.0f){
				im->mat_gray[resolution - 1 - i][j] = 0;
			}
			else{
				im->mat_gray[resolution - 1 - i][j] = 255;
			}
		}
	}
	return NULL;
}


void render(image *imm) {
	
	im = imm;
	int i;
	int P = num_threads;
	rap = im->width / P;
	
	pthread_t tid[P];
	int thread_id[P];
	
	for(i = 0;i < P; i++)
		thread_id[i] = i;
	
	//Apelam functia pentru fiecare thread in parte
	for(i = 0; i < P; i++){
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));;
	}
	
	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}
}

void writeData(const char * fileName, image *img) {

	//Deschidem fisierul pentru a scrie imaginea
	int i;
	FILE* f = f = fopen(fileName, "w"); 
	
	//Scriem imaginea in conformitate cu formatul precizat in enunt
	fprintf(f, "P5\n");
	fprintf(f, "%d %d\n", img->width, img->width);
	fprintf(f, "%d\n", img->maxval);
		
	for(i = 0 ; i < img->width ; i ++)
		fwrite(img->mat_gray[i], sizeof(unsigned char), img->width, f);
		
	//Inchidem fisieruld e scriere
	fclose(f);
}

