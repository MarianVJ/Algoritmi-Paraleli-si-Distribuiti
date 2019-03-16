#ifndef HOMEWORK_H1
#define HOMEWORK_H1


typedef struct {
	
	//In cazul acestui task type va fi mereu 5 ( deoarece imaginile sunt grayscale)
	unsigned char type;
	
	//Dimensiunea imaginii 
	int width ;
	int maxval;
	//Matricea de pixeli de dimensiuni width x width
	unsigned char **mat_gray;
	
}image;


void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */