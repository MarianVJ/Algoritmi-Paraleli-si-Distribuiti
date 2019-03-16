#ifndef HOMEWORK_H
#define HOMEWORK_H

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


void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */