#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <stdint.h>

#define carCapacity 513
const int capacity = 18313;  //hash size: > 57639 


struct Station{
    unsigned int pose;    
    int32_t biggestCar;
    int32_t cars[carCapacity];
    struct Station* next;
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
struct Station* hashTake(int pose);
void hashRemove(int pose);
void printStationHash();

void addCarAction(int32_t carValue, struct Station* Station);
void removeCarAction(int32_t carValue, struct Station* Station);
void removeBiggestCar(int32_t carValue, struct Station* station);
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

    //if(atoi(car)>=2147483648 || atoi(car)<0) exit(9);

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
                addCarSupport(node);
            }
            break;
        }
        input[i] = tmp;
        i++;
        input = (char*)realloc(input, (i+1) *sizeof(char));
    }while(tmp!='\n');

    //printStationHash();

    free(input);
    printf("aggiunta\n");
} 

void removeStation(){ //demolisci-stazione
    char input[15];
    if(fgets(input, sizeof(input), stdin)==NULL)return;    
    int pose = atoi(input);
    hashRemove(pose);

    //printStationHash();
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

    if(atoi(car)==node->biggestCar && node->biggestCar!=0) removeBiggestCar(atoi(car), node);
    else removeCarAction(atoi(car), node);
    

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

//take an elemento from the hash return null if it isn't present
struct Station* hashTake(int pose){
    int hashIndex = pose % capacity;

    if(hash[hashIndex]==NULL)return NULL;
    if(hash[hashIndex]->pose==pose) return hash[hashIndex];

    struct Station* hashEl = hash[hashIndex];

    while (hashEl->next!=NULL){
        if(hashEl->next->pose==pose){
            return hashEl->next;
        }

        hashEl=hashEl->next;        
    }

    return NULL;
}

//add an element to hash table
struct Station* hashInsert(unsigned int pose){
    int hashIndex = pose % capacity;

    if(hash[hashIndex]==NULL){
        hash[hashIndex]= (struct Station*)malloc(sizeof(struct Station));
        hash[hashIndex]->pose = pose;
        hash[hashIndex]->biggestCar = 0;
        hash[hashIndex]->next = NULL;
        for(int i = 0; i<carCapacity;i++)hash[hashIndex]->cars[i]=0;
        return hash[hashIndex];
    }

    if(hash[hashIndex]->pose==pose){
            printf("non aggiunta\n");
            return NULL; 
    }

    struct Station* hashEl = hash[hashIndex];
    //int i=0;

    while(hashEl->next!=NULL){
        if(hashEl->next->pose==pose){
            printf("non aggiunta\n");
            return NULL;
        }

        hashEl=hashEl->next;

        //i++;
        //if(i>3)exit(30);
    }
    hashEl->next = (struct Station*)malloc(sizeof(struct Station));
    hashEl->next->pose = pose;
    hashEl->next->biggestCar = 0;
    hashEl->next->next = NULL;
    for(int i = 0; i<carCapacity;i++)hashEl->next->cars[i]=0;

    return hashEl->next;
}

void hashRemove(int pose){
    int hashIndex = pose % capacity;
    if(hash[hashIndex]==NULL){
        printf("non demolita\n");
        return;
    }
    if(hash[hashIndex]->pose==pose){
        struct Station* tmp = hash[hashIndex];
        hash[hashIndex] = tmp->next;
        free(tmp);

        printf("demolita\n");
        return;
    }

    struct Station* hashEl = hash[hashIndex];

    while (hashEl->next!=NULL)
    {
        
        if(hashEl->next->pose==pose){
            struct Station* tmp = hashEl->next;
            hashEl->next = hashEl->next->next;
            free(tmp);

            printf("demolita\n");
            return;

        }

        hashEl=hashEl->next;
    }
    

    printf("non demolita\n");
}

void printStationHash(){
    for(int i=0; i< capacity; i++){
        struct Station* hashEl = hash[i];
        if(hashEl!=NULL){
            while (hashEl!=NULL){
                printf("pose: %d car:%d | ", hashEl->pose, hashEl->biggestCar);
                hashEl=hashEl->next;
            }
            printf("\n");
        }
    }
}

/*################################################################*/
/*              car management                                    */
/*################################################################*/

void carPrinter(struct Station* station){
    printf("%d\n", station->pose);
    for(int i = carCapacity-1; i>=0;i--){
       if(station->cars[i]!=0) printf("%d | ", station->cars[i]);
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

void removeBiggestCarSupport(int32_t carValue, struct Station* station, int32_t maxValue, int i){
    while(i>=0){
        if(station->cars[i]!=carValue && station->cars[i]>maxValue)maxValue=station->cars[i];
        if(station->cars[i]==carValue)return;

        i--;
    }
    station->biggestCar=maxValue;
}

void removeBiggestCar(int32_t carValue, struct Station* station){
    if(carValue==0)carValue=-1;
    int32_t maxValue = 0;
    for(int i = carCapacity-1; i>=0;i--){
        if(station->cars[i]!=carValue && station->cars[i]>maxValue)maxValue=station->cars[i];
        if(station->cars[i]==carValue){
            station->cars[i]=0;
            printf("rottamata\n");
            
            removeBiggestCarSupport(carValue,station,maxValue,i);;

            return;
        }
    }

    printf("non rottamata\n");
    return; 
}

/*################################################################*/
/*              path fider                                        */
/*################################################################*/