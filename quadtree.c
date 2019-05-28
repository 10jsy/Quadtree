// Quad-tree encoding of binary image
// Jea Seong Yoon 6491245

// Header files included to allow use of functions
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

// Key values defined for use throughout the program and allows simple modification
#define MinWidth 1
#define MaxWidth 64
#define Black 0
#define White 1
#define Grey 2

// Structure of node set here for use within the quadtree. Contains pointers to 4 child nodes (NW/NE/SW/SE) of the same structure, as well as Colour, X, Y and Size
typedef struct Node{
    struct Node* Child[4];
    int Colour;
    int CoordX;
    int CoordY;
    int Size;
} Node;

// Function to generate dynamic 2D array to store values. Requires an int parameter of Image Width to set a square array with dimensions Image Width by Image Width. Returns this array
int **GenerateArray(int size){
    int **ret;

    ret = malloc(sizeof(int**) * size);
    for (int i = 0; i < size; i++){
            ret[i] = malloc(sizeof(int*) * size);
        for (int  j = 0; j < size; j++){
            ret[i][j] = j;
        }
    }
  return (ret);
}

// Function to check whether or not the input parameter ImageWidth lies within the max and min boundary defined above, then return true or false depending on whether or not it is a power of two
bool PowerOfTwoMinMaxWidthCheck(int ImgWidth){

    if( ImgWidth>=(MinWidth) && ImgWidth<=(MaxWidth) ){
        return ImgWidth && (! ( ImgWidth & (ImgWidth-1) ) );
    }
    else{
        fprintf(stderr, "Incorrect image width, try again");
        exit(1);
    }
}

// Function to read in data from input file and assign the data to a 1D array initially. The first and second element of this 1D array correspond to ImgWidth and NumBlackNodes respectively. Includes error checks throughout to ensure correct input from file.
// Calls on boolean function above to test for validity of ImgWidth
// Calls on GenerateArray function above to create a 2D array then assigns Black nodes according to the x and y values within the 1D array of initial data.
// File closed and exit, this function is called in main().
int ReadAndInitialise(char *file_name){

    int TempInt, a, i, j, ImgWidth, NumBlackNodes, XCounter = 0, YCounter = 0, PoTMMW;
    int *InputDataArr = NULL;
    int *X;
    int *Y;
    size_t InputDataArrSize = 0;
    FILE *file;

    file = fopen("input.txt", "r");

    if(file == NULL){
        fprintf(stderr, "No such file exists, try again\n");
        exit(1);
    }

    while (fscanf(file, "%d", &a) == 1){ // while input exists, run loop
            int *temparr = realloc(InputDataArr, (InputDataArrSize + 1) * sizeof(int));

            if (temparr != NULL){

                    InputDataArr = temparr;
                    InputDataArr[InputDataArrSize++] = a;
            }

    }

    ImgWidth = InputDataArr[0];

    PoTMMW = PowerOfTwoMinMaxWidthCheck(ImgWidth);
    if(PoTMMW != 1){
        fprintf(stderr, "Image width input is not a power of two or does not lie within the min max range, try again");
        exit(1);
    }

    NumBlackNodes = InputDataArr[1];

    if(NumBlackNodes == 0){
        printf("No black nodes in this input file");
        exit(0);
    }

    if(NumBlackNodes == (ImgWidth*ImgWidth)){
        printf("Entire image is black. Black terminal node at position (0,0) with size %i\n", ImgWidth);
        exit(0);
    }

//    printf("%d\n", ImgWidth);
//    printf("%d\n", NumBlackNodes);

    for(i = 2; i < InputDataArrSize; i++){
        if (i % 2 == 0){
            XCounter++;
        }

        else{
            YCounter++;
        }
    }

//    printf("%d\n", XCounter);
//    printf("%d\n", YCounter);

    if ((XCounter != NumBlackNodes) || (YCounter != NumBlackNodes)){
        fprintf(stderr, "Number of X and Y coordinates do not match the input number of black nodes, try again");
        exit(1);
    }

    int **InitialArray = GenerateArray(ImgWidth);

    for(i = 1; i < InputDataArrSize/2; i++){
        InitialArray[InputDataArr[2 * i]][InputDataArr[2 * i + 1]] = Black;
//        printf("X and Y: (%d, %d)\n", InputDataArr[2 * i], InputDataArr[2 * i + 1]);
    }

    fclose(file);
    return (**InitialArray);
}

// Function to create a quadtree recursively. Input parameters are 2D array and node.
// Switch case from 0 to 3 to ensure all 4 quadrants are examined, and within these cases, each of the quadrants are examined to check if they are white black or grey. If white, break, if black, print its location and size, and if grey, recursively call the function.
// Called in main().
void Quadtree(int **Array, Node *Node){

	int i, j, quad;
	int ImageWidth;

	for(quad = 0; quad < 4; quad++){
		switch(quad){
			case 0:
				// NW
				ImageWidth = Node->Size;
				for(i=0 ; i<ImageWidth/2 ; i++){
					for(j=0 ; j<ImageWidth/2 ; j++){
						if(Array[i][j] == White){
							printf("Entire quadrant is white");
							break;
						}
						else if(Array[i][j] == (ImageWidth/2)*(ImageWidth/2)){ //or == 0
							printf("Entire quadrant is black");
							printf("Black terminal node at position (%i,%i) with size %i\n)", Array[0], Array[0], ImageWidth/2);
							break;
						}
						else{
                            Node->Child[0]->CoordX = i;
                            Node->Child[0]->CoordY = j;
                            Node->Child[0]->Size = ImageWidth/2;
							for (i = 0; i < ImageWidth; i++){
                                int *cp = *Array;
                                for (j = 0; j < ImageWidth; j++){
                                    printf("%d, ", *cp);
                                    *(cp)++;
                                }

                                Array++;
							}
							Quadtree(Array[i][j], Node->Child[0]);
						}
					}
				}
			case 1:
				// NE
				ImageWidth = Node->Size;
				for(i=ImageWidth/2 ; i<ImageWidth ; i++){
					for(j=0 ; j<ImageWidth/2 ; j++){
						if(Array[i][j] == White){
							printf("Entire quadrant is white");
							break;
						}
						else if(Array[i][j] == (ImageWidth/2)*(ImageWidth/2)){
							printf("Entire quadrant is black");
							printf("Black terminal node at position (%i,%i) with size %i\n)", Array[ImageWidth/2], Array[0], ImageWidth/2);
							break;
						}
						else{
							Node->Child[1]->CoordX = i;
                            Node->Child[1]->CoordY = j;
                            Node->Child[1]->Size = ImageWidth/2;
                            for (i=ImageWidth/2 ; i<ImageWidth ; i++){
                                int *cp = *Array;
                                for (j=0 ; j<ImageWidth/2 ; j++){
                                    printf("%d, ", *cp);
                                    *(cp)++;
                                }

                                Array++;
							}
							Quadtree(Array[i][j], Node->Child[1]);
						}
					}
				}

			case 2:
				// SW
				ImageWidth = Node->Size;
				for(i=0 ; i<ImageWidth/2 ; i++){
					for(j=ImageWidth/2 ; j<ImageWidth ; j++){
						if(Array[i][j] == White){
							printf("Entire quadrant is white");
							break;
						}
						else if(Array[i][j] == (ImageWidth/2)*(ImageWidth/2)){
							printf("Entire quadrant is black");
							printf("Black terminal node at position (%i,%i) with size %i\n)", Array[0], Array[ImageWidth/2], ImageWidth/2);
							break;
						}
						else{
							Node->Child[2]->CoordX = i;
                            Node->Child[2]->CoordY = j;
                            Node->Child[2]->Size = ImageWidth/2;
                            for (i=0 ; i<ImageWidth/2 ; i++){
                                int *cp = *Array;
                                for (j=ImageWidth/2 ; j<ImageWidth ; j++){
                                    printf("%d, ", *cp);
                                    *(cp)++;
                                }

                                Array++;
							}
							Quadtree(Array[i][j], Node->Child[2]);
						}
					}
				}
			case 3:
				// SE
				ImageWidth = Node->Size;
				for(i=ImageWidth/2 ; i<ImageWidth ; i++){
					for(j=ImageWidth/2 ; j<ImageWidth ; j++){
						if(Array[i][j] == White){
							printf("Entire quadrant is white");
							break;
						}
						else if(Array[i][j] == (ImageWidth/2)*(ImageWidth/2)){
							printf("Entire quadrant is black");
							printf("Black terminal node at position (%i,%i) with size %i\n)", Array[ImageWidth/2], Array[ImageWidth/2], ImageWidth/2);
							break;
						}
						else{
							Node->Child[3]->CoordX = i;
                            Node->Child[3]->CoordY = j;
                            Node->Child[3]->Size = ImageWidth/2;
                            for (i=ImageWidth/2 ; i<ImageWidth ; i++){
                                int *cp = *Array;
                                for (j=ImageWidth/2 ; j<ImageWidth ; j++){
                                    printf("%d, ", *cp);
                                    *(cp)++;
                                }

                                Array++;
							}
							Quadtree(Array[i][j], Node->Child[3]);
						}
					}
				}
        }
	}
}

// Main driver of the program. Calls on all relevant functions.
int main(int argc, char *argv[]){

    int **Arr;
    Node Root;


    Arr = ReadAndInitialise(argv[1]);
    Quadtree(Arr, &Root);
}
