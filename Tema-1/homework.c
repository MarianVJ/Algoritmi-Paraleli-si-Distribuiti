#include "homework.h"
#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>

int num_threads;
int resize_factor;

//Variabile globale care pointeaza catre matricile de intrare si iesire
image* out;
image* in;


//rap reprezinta numarul de linii in care este impartita matricea
//pe care un thread din cele num_threads trebuie sa le aplice operatia 
//de resize , in cazul in care numarul de linii nu se imparte exact 
//si la numarul de coruri si la resize_factor ultimul thread
//(cel cu id-ul cel mai amre) , in 
//functie de datele de intrare va avea de efecutat un nr mai mic/mare
//de operatii raportat la celalte 
int rap;

//Matricea ce contine valorile Kernelului Gausian
unsigned char mat[3][3];

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



//Functia primeste ca argument id-ul thread-ului
void* threadFunction(void *var)
{
	int thread_id = *(int*)var;
	int i, j, k, t;
	//Variabile auxiliare
	unsigned int r = 0, g = 0 , b = 0;
	//Numarul de elemente dintr-o matrice de dimensiune resize_factor
	unsigned int nr = (unsigned int)(resize_factor *  resize_factor);
	//ii si jj reprezinta coordonatele noului punct din matricea pe care
	//matricea de pixeli pe care noi o construim
	int ii , jj = 0;
	
	//st si fn reprezinta indicii liniilor , de unde thread-ul
	//curent va incepe sa efectueze operatia de resize 
	int st = thread_id * rap * resize_factor;
	int fn = st + (rap * resize_factor);
	
	//Ultimul thread(raportandu-ne la id) va efectua calculele pana 
	//la finalul matricei de intrare
	if(thread_id == num_threads - 1){
		fn = in->height;
	}
	
	//Algoritmul meu practic va retine mereu in i si j coordonatele
	//din coltul dreapta sus pt fiecare matrice de dimensiune resize_factor
	//de aceea adunam la valoarea de start 
	st += resize_factor;
	
	
	//Actualizam valoarea primei linii de unde incepe threadul
	//curent sa faca operatia de resize
	ii = (st - resize_factor) / resize_factor;
   
	
	//Daca imaginea este grayscacle type-ul va fi 5
	if(in->type == 5){
		//In functie de valoarea lui resize_factor avem 2 cazuri 
		//3 sau multiplu de 2
		if(resize_factor % 2 == 0){
			for(i = st; i <= fn ; i += resize_factor){
				jj = 0;
				for(j = resize_factor ; j <= in->width ; j += resize_factor){
						r = 0;
						for(t = i - resize_factor ; t < i ; t++){
							for(k = j - resize_factor ; k < j ; k++){
								r += (unsigned int)in->mat_gray[t][k];
							}
						}
						out->mat_gray[ii][jj] = (unsigned char)(r  / nr);
						jj++;
				}
				ii++;
			}
		}
		else{
			for(i = st; i <= fn ; i += resize_factor){
				jj = 0;
				for(j = resize_factor ; j <= in->width ; j += resize_factor){
						r = 0;
						for(t = i - resize_factor ; t < i ; t++){
							for(k = j - resize_factor ; k < j ; k++){
								r += (unsigned int)(in->mat_gray[t][k] * 
									  mat[t%3][k%3]);
							}
						}
						out->mat_gray[ii][jj] = (unsigned char)(r  / 16);
						jj++;	
				}
				ii++;
			}		
		}
	}
	else{

		if(resize_factor % 2 == 0){
			for(i = st; i <= fn ; i += resize_factor){
				jj = 0;
				for(j = resize_factor ; j <= in->width ; j += resize_factor){
						r = g = b = 0;
						for(t = i - resize_factor ; t < i ; t++){
							for(k = j - resize_factor ; k < j ; k++){
								r += (unsigned int) in->mat_col[t][k].r;
								g += (unsigned int) in->mat_col[t][k].g;
								b += (unsigned int) in->mat_col[t][k].b;
							}
						}
						out->mat_col[ii][jj].r = (unsigned char) (r  / nr);
						out->mat_col[ii][jj].g = (unsigned char) (g  / nr);
						out->mat_col[ii][jj].b = (unsigned char) (b  / nr);
						jj++;	
				}
				ii++;
			}
		}
		else{
			for(i = st; i <= fn ; i += resize_factor){
				jj = 0;
				for(j = resize_factor ; j <= in->width ; j += resize_factor){
						r = g = b = 0;
						for(t = i - resize_factor ; t < i ; t++){
							for(k = j - resize_factor ; k < j ; k++){
								r += (unsigned int) (in->mat_col[t][k].r * 
									  mat[t % 3][k % 3]);
								g += (unsigned int) (in->mat_col[t][k].g * 
									  mat[t % 3][k % 3]);
								b += (unsigned int) (in->mat_col[t][k].b * 
									  mat[t % 3][k % 3]);
								
							}
						}
						out->mat_col[ii][jj].r = (unsigned char) (r  / 16);
						out->mat_col[ii][jj].g = (unsigned char) (g  / 16);
						out->mat_col[ii][jj].b = (unsigned char) (b  / 16);
						jj++;	
				}
				ii++;
			}
		}
	}
	return NULL;
}



void resize(image *inn, image * outt) { 

	int i;
	//Vom realiza operatiile pe matrice folosind variabile globale
	in = inn;
	out = outt;
	
	//Conform cerintei daca rezolutia imaginii initiale nu se imparte perfect
	//la resize_factor se va pierde informatia din partea dreapta sau din
	//partea de jos a imaginii
	in->width -= (in->width % resize_factor);
	in->height -= (in->height % resize_factor);
	
	//Noua matrice va fi de acelasi tip 
	out->type = in->type;
	out->maxval = in->maxval;

	
	
	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	
	
	//Pentru ca valorile din matrice sa fie intr-o zona compacta de memorie 
	//de pe heap realizam alocarea inainte sa incepem paralelizarea 
	if(in->type == 5){
		out->mat_gray = malloc(sizeof(unsigned char*) * out->height);
		for(i = 0 ; i < out->height ; i++){
			out->mat_gray[i] = malloc(sizeof(unsigned char) * out->width);
		}
	}
	else{
		out->mat_col = malloc(sizeof(rgb*) * out->height);
		for(i = 0; i < out->height ; i++){
			out->mat_col[i] = malloc(sizeof(rgb) * out->width);
		}
	}
	
	//Initializam Kernelul Gausian 
	mat[0][0] = 1;
	mat[0][1] = 2;
	mat[0][2] = 1;
	
	mat[1][0] = 2;
	mat[1][1] = 4;
	mat[1][2] = 2;
	
	mat[2][0] = 1;
	mat[2][1] = 2;
	mat[2][2] = 1;
	
	
	//Initializam numarul de threaduri 
	int P = num_threads;
	rap = in->height / P / resize_factor;
	
	pthread_t tid[P];
	int thread_id[P];
	
	for(i = 0;i < P; i++)
		thread_id[i] = i;
	
	//Apelam functia pentru fiecare thread in parte
	for(i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));;
	}
		
	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}
}