#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CODE_SIZE 1023
#define NUMBER_OF_SATELLITES 24
#define NUMBER_OF_REGISTERS 10
#define NUMBER_OF_USED_SATELLITES 4

signed int fileValue[CODE_SIZE];
int registerSumFirstValues[NUMBER_OF_SATELLITES] = {2, 3, 4, 5, 1 ,2, 1, 2, 3, 2, 3, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 1, 4};
int registerSumSecondValues[NUMBER_OF_SATELLITES] = {6, 7, 8, 9, 9, 10, 8, 9, 10, 3, 4, 6, 7, 8, 9, 10, 4, 5, 6, 7, 8, 9, 3, 6};

int readInFile(char* filename){
    FILE *file = fopen(filename, "r");

    int value = 0;
    int i = 0;

    fscanf (file, "%d", &value);
    while (!feof (file))
    {
        fileValue[i] = value;
        i++;
        fscanf (file, "%d", &value);
    }

    fclose(file);

    return 0;
}

int *getTopSum(){
    int registerValues[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int *result = malloc(CODE_SIZE * sizeof(int));

    for (int i = 0; i < CODE_SIZE; i++) {
        *(result + i) = registerValues[9];
        int newFrontValue = registerValues[2] ^ registerValues[9];
        for(int i = 8; i >= 0; i--){
            registerValues[i + 1] = registerValues[i];
        }
        registerValues[0] = newFrontValue;
    }

    return result;
}

int *getBottomSum(int satelliteID){
    int registerValues[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int *result = malloc(CODE_SIZE * sizeof(int));
    int registerSumFirstValue = *(registerSumFirstValues + (satelliteID - 1));
    int registerSumSecondValue = *(registerSumSecondValues + (satelliteID - 1));

    for (int i = 0; i < CODE_SIZE; i++) {
        int newFrontValue = ((((registerValues[1] ^ registerValues[2]) ^ registerValues[5]) ^ registerValues[7]) ^ registerValues[8]) ^ registerValues[9];

        int newResultValue = registerValues[registerSumFirstValue - 1] ^ registerValues[registerSumSecondValue - 1];
        *(result + i) = newResultValue;

        for(int i = 8; i >= 0; i--){
            registerValues[i + 1] = registerValues[i];
        }

        registerValues[0] = newFrontValue;
    }

    return result;
}

int *chipCode(int satelliteID){
    int *top = getTopSum();
    /*int topValues[CODE_SIZE];

    for (int j = 0; j < CODE_SIZE; ++j) {
        topValues[j] = *(top + j);
    }*/

    int *bottom = getBottomSum(satelliteID);

    /*int bottomValues[CODE_SIZE];

    for (int j = 0; j < CODE_SIZE; ++j) {
        bottomValues[j] = *(bottom + j);
    }*/

    int *result = malloc(CODE_SIZE * sizeof(int ));

    for(int i = 0; i < CODE_SIZE; i++){
        int newResultValue = *(top + i) ^ *(bottom + i);

        if(newResultValue == 0){
            newResultValue = -1;
        }

        *(result + i) = newResultValue;
    }

    return result;
}

int crossCorrelate(int* first, int* second){
    int value = 0;

    for(int i = 0; i < CODE_SIZE; i++){
        value = value + (*(first + i) * *(second + i));
    }

    return value;
}

int *shiftCode(int *code, int count){
    int *result;
    result = (int *) malloc(CODE_SIZE * sizeof(int));

    int j = 0;

    for(int i = count; i < CODE_SIZE; i++){
        *(result + j) = *(code + i);
        j++;
    }

    for (int i = 0; i < count; i++) {
        *(result + j) = *(code + i);
        j++;
    }

    return result;
}

int getPositiveNoiseValue(){
    return (pow(2, (NUMBER_OF_REGISTERS + 2) / 2) - 1) * (NUMBER_OF_USED_SATELLITES - 1) ;
}

int getNegativeNoiseValue(){
    return (-1 * pow(2, (NUMBER_OF_REGISTERS + 2) / 2) - 1) * (NUMBER_OF_USED_SATELLITES - 1);
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        printf("Please enter input file name as a parameter\n");
    }

    char *filename = argv[1];

    int res = readInFile(filename);

    if(res < 0){
        return res;
    }

    for(int s = 1; s < 25; s++) {
        int *code = chipCode(s);
        for (int i = 0; i < CODE_SIZE; i++) {
            int *shiftedCode = shiftCode(fileValue, i);
            int correlation = crossCorrelate(code, shiftedCode);
            //TODO: Wert für Korrelation finden
            if(correlation > CODE_SIZE + getNegativeNoiseValue()){
                printf("Satellite %d has sent bit 1 (delta = %d)\n", s, i);
            }else if(correlation < ((-1) * CODE_SIZE + getPositiveNoiseValue())){
                printf("Satellite %d has sent bit 0 (delta = %d)\n", s, i);
            }
        }
    }
    return 0;
}