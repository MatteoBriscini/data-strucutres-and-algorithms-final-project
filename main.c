#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <stdint.h>

#define carCapacity 513
const int capacity = 17191;  //hash size > 57639 


struct Station{
    int valid : 1;
    unsigned int pose;    
    int32_t biggestCar;
    int32_t cars[carCapacity];
};



struct Station** hash;
struct Station* garbage;

/* prototypes */
void ignoreAllTheLine();
void parser();
char parserSupport(int index, char string[]);
void path();
void removeCar();
void addCar();
void removeStation();
void addStation();

struct Station* hashInsert(unsigned int pose);
int hashFind(int pose);
struct Station* hashTake(int pose);
void hashRemove(int hashIndex);
void printStationHash();

void addCarAction(int32_t carValue, struct Station* Station);
void removeCarAction(int32_t carValue, struct Station* Station);
/* program */



int main(){
    hash =  (struct Station**)calloc(capacity,sizeof(struct Station*));

    parser();
    
    //printStationHash();
    return 0;
}

/*################################################################*/
/*              input parser                                      */
/*################################################################*/

void addCarSupport(struct Station* station){
    char tmp;
    char* car = (char*)malloc(sizeof(char));
    int i=0;
    do{
        tmp = getc(stdin);
        car[i] = tmp;
        i++;
        if(tmp == ' '){
            addCarSupport(station);
            break;
            }
        car = (char*)realloc(car, (i+1) *sizeof(char));
    }while((tmp!='\n'));

    if(atoi(car)>=2147483648 || atoi(car)<0) exit(9);

    if(atoi(car)>station->biggestCar) station->biggestCar = atoi(car);
    addCarAction(atoi(car), station);



    //printCars(station->aviableCar);
    free(car);
}

void addStation(){ //aggiungi-stazione
    char tmp;
    char* input = (char*)malloc(sizeof(char));
    int i=0;
    do{
        tmp=fgetc(stdin);
        if(tmp== ' ' || tmp=='\n'){
            struct Station* node = hashInsert(atoi(input));

            if(node==NULL){
                ignoreAllTheLine();
                return;
            }

            if(tmp== ' '){
                //node->aviableCar = (struct Car**)calloc(carCapacity,sizeof(struct Car*));
                addCarSupport(node);
            }
            break;
        }
        input[i] = tmp;
        i++;
        input = (char*)realloc(input, (i+1) *sizeof(char));
    }while(tmp!='\n');



    free(input);
    printf("aggiunta\n");
} 

void removeStation(){ //demolisci-stazione
    char input[15];
    if(fgets(input, sizeof(input), stdin)==NULL)return;    
    int pose = atoi(input);
    pose=hashFind(pose);
    if(pose!=-1){
        hashRemove(pose);
        printf("demolita\n");
    }
    else printf("non demolita\n");
} 

void addCar(){ //aggiungi-auto
    char tmp;
    char* station = (char*)malloc(sizeof(char));
    int i=0;
    do{
        tmp = fgetc(stdin);
        station[i] = tmp;
        i++;
        if(tmp == ' ')break;
        station = (char*)realloc(station, (i+1) *sizeof(char));
    }while(tmp!='\0');
    struct Station* node = hashTake(atoi(station));
    if(node==NULL){
        ignoreAllTheLine();
        printf("non aggiunta\n"); 
        return;
    }
    addCarSupport(node);

    //printf("%d\n", atoi(station));

    free(station);
    printf("aggiunta\n");
}
void removeCar(){ //rottama-auto
    char input;
    char* station = (char*)malloc(sizeof(char));
    int i=0;
    do{
        input=fgetc(stdin);
        if(input == ' ')break;
        station[i] = input;
        i++;
        station = (char*)realloc(station, (i+1) *sizeof(char));
    }while(input!='\n');
    struct Station* node = hashTake(atoi(station));
    if(node==NULL){ 
        ignoreAllTheLine();
        printf("non rottamata\n"); 
        return;
    }

    char* car = (char*)malloc(sizeof(char));
    i=0;
    do{
        input =  fgetc(stdin);
        if(input == ' ')break;
        car[i] = input;
        i++;
        car = (char*)realloc(car, (i+1) *sizeof(char));
    }while(input!='\n');

    //printf("%d %d\n", atoi(station), atoi(car));

   // unsigned int* range =  &node->biggestCar;
    removeCarAction(atoi(car), node);
    //if(atoi(car)==node->biggestCar && node->biggestCar!=0) printf("update necessario");

    free(station);
    free(car);
}
void path(){ //pianifica-percorso
    char input[15];
    char* num = (char*)malloc(sizeof(char));
    unsigned int start = 0;
    unsigned int end = 0;
    int i=0;
    do{
        input[0] = fgetc(stdin);
        if(input[0] == ' ')break;
        num[i] =  input[0];
        i++;
        num = (char*)realloc(num, (i+1) *sizeof(char));
    }while((input[0] !='\n'));
    start = atoi(num);
    if(fgets(input, sizeof(input), stdin)==NULL)return;  
    end=atoi(input);
    
    //printf("%d %d\n", start, end);

    if(start<end){printf("nessun percorso\n");}//growFirstStep(start,end);
    else{printf("nessun percorso\n");}

    free(num);
}

void ignoreChar(int number){
    for(int i=0; i<number;i++)getc(stdin);
}
void ignoreAllTheLine(){
    while (fgetc(stdin)!='\n'){}
}
//read input and call correct function
void parser(){
    char input;
    while (1){
        input=fgetc(stdin);
        if(input==EOF)return;
        //printf("%c ", input);

        if(input=='a'){
            ignoreChar(8);
            input=fgetc(stdin);
            if(input=='s'){         //aggiungi-stazione
                //printf("stazione ");
                ignoreChar(8);
                addStation();
            }
            else{                   //aggiungi-auto
                //printf("auto ");
                ignoreChar(4);
                addCar();
            }
        }else if(input== 'p'){      //pianifica-percorso
            ignoreChar(18);
            path();
        }else if (input=='d'){      //demolisci-stazione
            ignoreChar(18);
            removeStation();
        }else {                   //rimuovi-auto
            ignoreChar(12);
            removeCar();
        }

    }
}

/*################################################################*/
/*              station hash manage                               */
/*################################################################*/

//find an element in hash return his index
int hashFind(int pose){
    int hashIndex = pose % capacity;
    int relativeIndex = 0;
    while (hash[hashIndex+relativeIndex] != NULL) {
        if(hash[hashIndex+relativeIndex]->pose==pose){
            if(hash[hashIndex+relativeIndex]->valid==-1)return hashIndex+relativeIndex;
            else return -1;
        }
        relativeIndex ++;
        if(hashIndex+relativeIndex == capacity) relativeIndex = -hashIndex;
        if(relativeIndex==0)exit(32);     //the hash is full
    };
    
    return -1;
}

//take an elemento from the hash return null if it isn't present
struct Station* hashTake(int pose){
    int hashIndex = pose % capacity;
    int relativeIndex = 0;
    while (hash[hashIndex+relativeIndex] != NULL) {
        if(hash[hashIndex+relativeIndex]->pose==pose){
            if(hash[hashIndex+relativeIndex]->valid==-1)return hash[hashIndex+relativeIndex];
            else return NULL;
            }
        relativeIndex ++;
        if(hashIndex+relativeIndex == capacity) relativeIndex = -hashIndex;
        if(relativeIndex==0)exit(31);     //the hash is full
    };
    
    return NULL;
}

//add an element to hash table
struct Station* hashInsert(unsigned int pose){

    int hashIndex = pose % capacity;
    int relativeIndex = 0;

    while(hash[hashIndex+relativeIndex]!=NULL){
        if(hash[hashIndex+relativeIndex]->pose==pose){
            printf("non aggiunta\n");

            //printStationHash();

            return NULL;
        }

        if(hash[hashIndex+relativeIndex]->valid==0){
            hash[hashIndex+relativeIndex]->valid =1;
            hash[hashIndex+relativeIndex]->pose = pose;
            hash[hashIndex+relativeIndex]->biggestCar = 0;
            for(int i = 0; i<carCapacity;i++)hash[hashIndex+relativeIndex]->cars[i]=0;
            //printStationHash();

            return hash[hashIndex+relativeIndex];
        }

        relativeIndex++;
        if(hashIndex+relativeIndex == capacity) relativeIndex = -hashIndex;   

        if(relativeIndex==0)exit(30);
    }

    struct Station* node = (struct Station*)malloc(sizeof(struct Station));
    node->pose = pose;
    node->biggestCar = 0;
    node->valid = 1;

    for(int i = 0; i<carCapacity;i++)node->cars[i]=0;

    hash[hashIndex+relativeIndex] = node;

    //printStationHash();

    return node;
}

void hashRemove(int hashIndex){
    hash[hashIndex]->valid = 0;

    //printStationHash();
}

void printStationHash(){
    for(int i=0; i< capacity; i++){
        if(hash[i]!=NULL){
            printf("pose: %d car:%d valid:%d\n", hash[i]->pose, hash[i]->biggestCar, hash[i]->valid);
        }
    }
}

/*################################################################*/
/*              car management                                    */
/*################################################################*/

void carPrinter(struct Station* station){
    printf("%d\n", station->pose);
    for(int i = carCapacity-1; i>=0;i--){
        printf("%d | ", station->cars[i]);
    }
    printf("\n\n");
}

void addCarAction(int32_t carValue, struct Station* station) {
    if(carValue==0)carValue=-1;
    for(int i = 0; i<carCapacity; i++){
        if(station->cars[i]==0){
            station->cars[i]=carValue;
            return;
        }
    }
    exit(20);
}

void removeCarAction(int32_t carValue, struct Station* station){
    if(carValue==0)carValue=-1;
    for(int i = carCapacity-1; i>=0;i--){
        if(station->cars[i]==carValue){
            station->cars[i]=0;
            printf("rottamata\n");
            return;
        }
    }
    printf("non rottamata\n");
    return;
}

