#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>

struct Station{
    unsigned int pose;

    int leftMin;
    unsigned int leftReachMePose;
    unsigned int rightMax;
    unsigned int rightReachMePose;
    
    unsigned int biggestCar;
    struct Car* aviableCar;
};
struct Car{
    unsigned long avaiableCar;
    struct Car* next;
};

int capacity = 2129;  //hash size
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

void updateCarBitMaskPassed(unsigned int oldRange,unsigned int newRange, struct Car* bitMask);
void updateCarBitMaskRemoved(unsigned int* oldRange, struct Car* bitMask);
int carBitmaskOffset(unsigned int range, unsigned int carValue);
int isCar(unsigned int range, unsigned int carValue, unsigned int bitMask);
void addCarAction(unsigned int* range, unsigned int car, struct Car* bitMask, unsigned int stationPose);
void removeCarAction(unsigned int* range, unsigned int car, struct Car* bitMask, unsigned int count);
void printCars(struct Car* bitMask);

struct Station* hashInsert(unsigned int pose);
int hashFind(int pose);
struct Station* hashTake(int pose);
void hashRemove(int hashIndex);

void reachMeInit(struct Station* station);
void ReachMeUpdate(unsigned int oldRange, unsigned int newRange, unsigned int pose);
void printStationHash();

void growFirstStep(unsigned int start, unsigned int end);
void gorwQuadraticaly(unsigned int  start, unsigned int end, unsigned int* result);

/* program */



int main(){
    hash = (struct Station**)malloc(sizeof(struct Station*)* capacity);

    parser();
    
    //printStationHash();
    return 0;
}

/* parser */

void addCarSupport(int station){
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
    //printf("%d\n",atoi(car));

    if(atoi(car)!=0){
        struct Station* node = hashTake(station);
        unsigned int* range =  &node->biggestCar;
        addCarAction(range, atoi(car), node->aviableCar, node->pose);
    }
    free(car);
}

void addStation(){ //aggiungi-stazione
    char tmp;
    char* input = (char*)malloc(sizeof(char));
    int i=0;
    do{
        tmp=fgetc(stdin);
        if(tmp== ' ' || tmp=='\n'){
            if(hashFind(atoi(input))!= -1){ printf("non aggiunta\n"); return;}
            reachMeInit(hashInsert(atoi(input)));
            if(tmp== ' ')addCarSupport(atoi(input));
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
    if(hashFind(pose)!=-1){
        hashRemove(hashFind(pose));
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
    if(hashFind(atoi(station))== -1){
        ignoreAllTheLine();
        printf("non aggiunta\n"); 
        return;
    }
    addCarSupport(atoi(station));

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
    if(hashFind(atoi(station))== -1){ 
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

    struct Station* node = hashTake(atoi(station));

    unsigned int* range =  &node->biggestCar;
    if(isCar(node->biggestCar, atoi(car), node->aviableCar->avaiableCar)==0){ printf("non rottamata\n"); return;}
    removeCarAction(range, atoi(car), node->aviableCar, 0);
    printf("rottamata\n");

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

    if(start<end){growFirstStep(start,end);}
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
        printf("%c\n", input);

        if(input=='a'){
            ignoreChar(8);
            input=fgetc(stdin);
            if(input=='s'){         //aggiungi-stazione
                printf("stazione\n");
                ignoreChar(8);
                addStation();
            }
            else{                   //aggiungi-auto
                printf("auto\n");
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

/* hash manage */

//find an element in hash return his index
int hashFind(int pose){
    int hashIndex = pose % capacity;
    int relativeIndex = 0;
    while (hash[hashIndex+relativeIndex] != NULL) {
        if(hash[hashIndex+relativeIndex]->pose==pose)return hashIndex+relativeIndex;
        if(relativeIndex == hashIndex)break;
        if(relativeIndex == capacity) relativeIndex = -hashIndex;
        relativeIndex ++;
    };
    
    return -1;
}

//take an elemento from the hash return null if it isn't present
struct Station* hashTake(int pose){
    int hashIndex = pose % capacity;
    int relativeIndex = 0;
    while (hash[hashIndex+relativeIndex] != NULL) {
        if(hash[hashIndex+relativeIndex]->pose==pose)return hash[hashIndex+relativeIndex];
        if(relativeIndex == hashIndex)break;
        if(relativeIndex == capacity) relativeIndex = -hashIndex;
        relativeIndex ++;
    };
    
    return NULL;
}

//add an element to hash table
struct Station* hashInsert(unsigned int pose){
    struct Station* node = (struct Station*)malloc(sizeof(struct Station));
    node->pose = pose;
    node->leftReachMePose = 0;
    node->rightReachMePose = 0;
    node->leftMin =-1;
    node->aviableCar= (struct Car*)malloc(sizeof(struct Car*));

    int hashIndex = pose % capacity;
    int relativeIndex = 0;

    while(hash[hashIndex+relativeIndex]!=NULL){         
        if(relativeIndex == hashIndex)break;                //hash piena, andra` riallocata una piu grossa
        if(relativeIndex == capacity) relativeIndex = -hashIndex;
        relativeIndex++;
    }

    hash[hashIndex+relativeIndex] = node;
    return node;
}

void hashRemove(int hashIndex){
    free(hash[hashIndex]);
    hash[hashIndex]=NULL;
}

/* car manage */

//add the car to the bitMask
void addCarAction(unsigned int* range, unsigned int car, struct Car* bitMask, unsigned int stationPose){
    if(car>*range){
        updateCarBitMaskPassed(*range, car, bitMask);
        ReachMeUpdate(*range, car, stationPose);
        *range = car;
    }
    if(bitMask->next==NULL && isCar(*range,car,bitMask->avaiableCar)==1){
        bitMask->next = (struct Car*)malloc(sizeof(struct Car*));
        bitMask->next->avaiableCar =0;
        bitMask->next->next=NULL;
        return addCarAction(range,car,bitMask->next, stationPose);
    }
    else if(bitMask->next!=NULL && isCar(*range,car,bitMask->avaiableCar)==1){
        return addCarAction(range,car,bitMask->next, stationPose);
    }
    bitMask->avaiableCar = (unsigned int) bitMask->avaiableCar + carBitmaskOffset(*range, car);
    //printf("%d %lu \n", stationPose, bitMask->avaiableCar);
}

//remove the car from the bitmask (didn't controll if the car is present)
void removeCarAction(unsigned int* range, unsigned int car, struct Car* bitMask, unsigned int count){
    if(bitMask->next!=NULL && isCar(*range,car,bitMask->next->avaiableCar)){
        removeCarAction(range, car, bitMask->next, count+1);
        if(bitMask->next->avaiableCar==0){
            free(bitMask->next);
            bitMask->next=NULL;
        }
        return;
    }
    bitMask->avaiableCar = (unsigned int) bitMask->avaiableCar - carBitmaskOffset(*range, car);
    if(car==*range){
        if(count==0 && bitMask->avaiableCar == 0){
            *range=0;
            return;
        }
        updateCarBitMaskRemoved(range, bitMask);
    }
}

void printCars(struct Car* bitMask){
    printf ("| %lu |", bitMask->avaiableCar);
    if(bitMask->next!=NULL) return printCars(bitMask->next);
    printf("\n");
}

//called when new car added and is bigger than range
void updateCarBitMaskPassed(unsigned int oldRange,unsigned int newRange, struct Car* bitMask){
    bitMask->avaiableCar = (bitMask->avaiableCar)<<(newRange-oldRange);
    if(bitMask->next!=NULL)return updateCarBitMaskPassed(oldRange,newRange,bitMask->next);
}
void updateCarRemovedSupport(unsigned int oldeRange, unsigned int newRange, struct Car* bitMask){
    bitMask->avaiableCar = (bitMask->avaiableCar)>>(oldeRange-newRange);
    if(bitMask->next!=NULL)return updateCarRemovedSupport(oldeRange, newRange, bitMask->next);
}
//call when biggest car is been removed
void updateCarBitMaskRemoved(unsigned int* oldRange, struct Car* bitMask){
   unsigned int tmp = *oldRange;
   unsigned int newRange = tmp-1;

   while (newRange>0){ 
        if(isCar(tmp, newRange, bitMask->avaiableCar)==1){
            *oldRange = newRange;
            break;
        }
        else newRange= newRange-1;
    }
    return updateCarRemovedSupport(tmp, *oldRange, bitMask);
}
//offeset to update bitMask when car added or remove [if add bitMask = bitMask + offset | if remove bitMask = bitMask - offset ]
int carBitmaskOffset(unsigned int range, unsigned int carValue){
    return 1<<(range-carValue);
}
//to verify if there is the car in the aviableCard bitMask 
int isCar(unsigned int range, unsigned int carValue, unsigned int bitMask){
    if(((1<<(range-carValue))&bitMask)!=0) return 1;
    return 0;
}

/* reachable pose manage */
void reachMeInit(struct Station* station){ //when a new station is added its bitmusk must be created
    for(int i= 0; i < capacity; i++){
        if(hash[i] != NULL){
                if(hash[i]->pose < station->pose){ //left update
                    if(hash[i]->biggestCar>= station->pose-hash[i]->pose){
                        station->leftReachMePose = (unsigned int) station->leftReachMePose + (1<<(station->pose-hash[i]->pose-1));
                        if(station->leftMin>hash[i]->pose){
                            station->leftMin = hash[i]->pose;
                        }
                    }
                }
                if(hash[i]->pose > station->pose){ //right update
                    if(hash[i]->biggestCar>= hash[i]->pose-station->pose){
                        station->rightReachMePose = (unsigned int) station->rightReachMePose + (1 << (hash[i]->pose-station->pose-1));
                        if(station->rightMax<hash[i]->pose){
                            station->rightMax = hash[i]->pose;
                        }
                    }
                }
        }
    }
}

void ReachMeUpdate(unsigned int oldRange, unsigned int newRange, unsigned int pose){//update the bitmusk off al the station
    if(oldRange-newRange>capacity){
        for(int i=0;i<capacity;i++){
            if(hash[i]!=NULL){
                if(hash[i]->pose>=(pose-newRange)&&hash[i]->pose<(pose-oldRange)){//left update
                    hash[i]->rightReachMePose = (unsigned int) hash[i]->rightReachMePose + (1<< (pose-i-1));
                    if(hash[i]->rightMax<pose){
                        hash[i]->rightMax = pose;
                    }
                }
                if(hash[i]->pose<=(pose+newRange)&&hash[i]->pose>(pose+oldRange)){//right update
                    hash[i]->leftReachMePose = (unsigned int) hash[i]->leftReachMePose + (1<< (i-pose-1));
                    if(hash[i]->leftMin>pose){
                        hash[i]->leftMin = pose;
                    }
                }
            }
        }
        return;
    }
    int tmp=pose+oldRange-newRange;
    if(tmp<0){tmp=0;}
    for(int i = tmp; i<pose; i++){//left update
        if(hashFind(i)!=-1){
            struct Station* node = hashTake(i);            
            node->rightReachMePose = (unsigned int) node->rightReachMePose + (1<< (pose-i-1));
            if(node->rightMax<pose){
                node->rightMax = pose;
            }
            //printf("updated left: %d, %d, %d, %d\n", pose, i, node->rightReachMePose, node->rightMax);
        }
    }
    for(int i = pose+newRange; i>pose+oldRange; i--){//right update
        if(hashFind(i)!=-1){
            struct Station* node = hashTake(i);
            node->leftReachMePose = (unsigned int) node->leftReachMePose + (1<< (i-pose-1));
            if(node->leftMin>pose){
                node->leftMin = pose;
            }
            //printf("updated right: %d, %d, %d, %d\n", pose, i, node->leftReachMePose, node->leftMin);
        }
    }
}

void printStationHash(){
    for(int i=0; i< capacity; i++){
        if(hash[i]!=NULL){
            printf("pose: %d rb:%d rMax:%d lb:%d lMin:%d\n", hash[i]->pose, 
                hash[i]->rightReachMePose, hash[i]->rightMax ,
                hash[i]-> leftReachMePose, hash[i]->leftMin);
        }
    }
}

void correctLeftMin(struct Station* station){
    station->leftReachMePose=station->leftReachMePose-(1<<((station->pose-1)-(station->leftMin)));
    int i;
    for(i=(station->pose-2)-(station->leftMin); i>=0; i--){
        if (((1<<i)&station->leftReachMePose)!=0){
            if(hashFind(station->pose-1-i)!=-1)break;
        }
    }
    if(station->leftReachMePose==0){station->leftMin=-1;}
    else{station->leftMin=station->pose-1-i;}
}

/* plan route */



void growFirstStep(unsigned int start, unsigned int end){
    if(hashTake(end)->leftMin==start){//if you can reach directly
        printf("%d %d\n", start, end);
        return;
    }
    unsigned int result=0;
    int i=end;
    while (true){
        if(hashTake(i)->leftMin==-1){//if you can't move
            printf("nessun percorso\n");
            return;
        }
        if(hashFind(hashTake(i)->leftMin)==-1){//if the selected station was removed
            correctLeftMin(hashTake(i));
            if(hashTake(i)->leftMin==-1){//if you can't move
                printf("nessun percorso\n");
                return;
            }
        }
        i=hashTake(i)->leftMin;
        result=result+(1<<(end-i-1));
        if(hashTake(i)->leftMin<=start){
            break;
        }
    }
    //printf("res:%d act:%d\n", result, i);
    if(i!=start)gorwQuadraticaly(start, i, &result);
}

void gorwQuadraticaly(unsigned int  start, unsigned int end, unsigned int* result){
    printf("nessun percorso\n");
}