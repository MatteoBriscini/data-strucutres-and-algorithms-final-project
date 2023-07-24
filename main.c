#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>

struct Station{
    unsigned int pose;

    unsigned int leftMin;
    unsigned int leftReachMePose;
    unsigned int rightMax;
    unsigned int rightReachMePose;
    
    unsigned int biggestCar;
    struct Car* aviableCar;
};
struct Car{
    unsigned int avaiableCar;
    struct Car* next;
};

int capacity = 47;  //hash size
struct Station** hash;
struct Station* garbage;

/* prototypes */
void parser();
char parserSupport(int index, char string[]);
void path(char data[]);
void removeCar(char data[]);
void addCar(char data[]);
void removeStation(char data[]);
void addStation(char data[]);

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


/* program */



int main(){
    hash = (struct Station**)malloc(sizeof(struct Station*)* capacity);

    parser();
}

/* parser */

void addCarSupport(int station, char *data){
    char* car = (char*)malloc(sizeof(char));
    int i=0;
    do{
        car[i] = *(data);
        i++;
        data = data +1;
        if(*(data) == ' ')break;
        car = (char*)realloc(car, (i+1) *sizeof(char));
    }while((*(data+1)!='\0'));

    struct Station* node = hashTake(station);
    unsigned int* range =  &node->biggestCar;
    addCarAction(range, atoi(car), node->aviableCar, node->pose);

    free(car);
    if(*data == ' ') return addCarSupport(station, data+1);
}

void addStation(char* data){ //aggiungi-stazione
    char* input = (char*)malloc(sizeof(char));
    int i=0;
    do{
        input[i] = *(data);
        i++;
        data = data +1;
        if(*(data) == ' ')break;
        input = (char*)realloc(input, (i+1) *sizeof(char));
    }while((*(data+1)!='\0'));
    if(hashFind(atoi(input))!= -1){ printf("non aggiunta\n"); return;}

    //printf("%d\n", atoi(input));

    reachMeInit(hashInsert(atoi(input)));

    if(*data == ' '){
        addCarSupport(atoi(input), data+1);
    }
    free(input);
    printf("aggiunta\n");
} 

void removeStation(char* data){ //demolisci-stazione
    int pose = atoi(data);
    if(hashFind(pose)!=-1){
        hashRemove(hashFind(pose));
        printf("demolita\n");
    }
    else printf("non demolita\n");
} 

void addCar(char* data){ //aggiungi-auto
    char* station = (char*)malloc(sizeof(char));
    int i=0;
    do{
        station[i] = *(data);
        i++;
        data = data +1;
        if(*(data) == ' ')break;
        station = (char*)realloc(station, (i+1) *sizeof(char));
    }while((*(data+1)!='\0'));
    if(hashFind(atoi(station))== -1){ printf("non aggiunta\n"); return;}
    addCarSupport(atoi(station), data+1);

    free(station);
    printf("aggiunta\n");
}
void removeCar(char* data){ //rottama-auto
    char* station = (char*)malloc(sizeof(char));
    int i=0;
    do{
        station[i] = *(data);
        i++;
        data = data +1;
        if(*(data) == ' ')break;
        station = (char*)realloc(station, (i+1) *sizeof(char));
    }while((*(data+1)!='\0'));
    if(hashFind(atoi(station))== -1){ printf("non rottamata\n"); return;}

    char* car = (char*)malloc(sizeof(char));
    i=0;
    do{
        car[i] = *(data);
        i++;
        data = data +1;
        if(*(data) == ' ')break;
        car = (char*)realloc(car, (i+1) *sizeof(char));
    }while((*(data+1)!='\0'));

    struct Station* node = hashTake(atoi(station));

    unsigned int* range =  &node->biggestCar;
    if(isCar(node->biggestCar, atoi(car), node->aviableCar->avaiableCar)==0){ printf("non rottamata\n"); return;}
    removeCarAction(range, atoi(car), node->aviableCar, 0);
    printf("rottamata\n");

    free(station);
}
void path(char* data){ //pianifica-percorso
    char* num = (char*)malloc(sizeof(char));
    int start = 0;
    int end = 0;
    int i=0;
    do{
        num[i] =  *(data);
        i++;
        data = data +1;
        if(*(data) == ' ')break;
        num = (char*)realloc(num, (i+1) *sizeof(char));
    }while((*(data+1)!='\0'));
    start = atoi(num);
    end = atoi(data);
    
    printf("%d %d\n",start,  end);

    free(num);
    
    printStationHash();
}

//read input and call correct function
void parser(){
    while (1){
        char input[255];
        char *pointer = input; 
        if(fgets(input, sizeof(input), stdin) == NULL){
            return;
        }
        if(input[0] == 'p'){
            path(pointer+19);
        }else if (input[0] == 'a'){
            if(input[9]=='s')addStation(pointer+18);
            else addCar(pointer+14);
        }else if(input[0] == 'd'){
            removeStation(pointer+19);
        }else{
            removeCar(pointer+13);
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
    hash[hashIndex]=garbage;
}

/* car manage */

//add the car to the bitMask
void addCarAction(unsigned int* range, unsigned int car, struct Car* bitMask, unsigned int stationPose){
    if(car>*range){
        updateCarBitMaskPassed(*range, car, bitMask);
        ReachMeUpdate(*range, car, stationPose);
        *range = car;
    }
    if(isCar(*range,car,bitMask->avaiableCar) && bitMask->next==NULL){
        bitMask->next = (struct Car*)malloc(sizeof(struct Car*));
        return addCarAction(range,car,bitMask->next, stationPose);
    }
    else if(isCar(*range,car,bitMask->avaiableCar) && bitMask->next!=NULL){
        return addCarAction(range,car,bitMask->next, stationPose);
    }
    bitMask->avaiableCar = (unsigned int) bitMask->avaiableCar + carBitmaskOffset(*range, car);
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
    printf ("%d ", bitMask->avaiableCar);
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

/*
    idealmente verrebbe richiamata quando viene aggiunto un veicolo e il range e` superato 
    unico problema nei metodi di aggiunta del veicolo non ho ha disposizione la pose della stazione 
*/
void ReachMeUpdate(unsigned int oldRange, unsigned int newRange, unsigned int pose){  
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
                node->leftMin=pose;
            }
            //printf("updated right: %d, %d, %d, %d\n", pose, i, node->leftReachMePose, node->leftMin);
        }
    }
}

void printStationHash(){
    for(int i=0; i< capacity; i++){
        if(hash[i]!=NULL){
            printf("pose: %d rb:%d lb:%d\n", hash[i]->pose, hash[i]->rightReachMePose, hash[i]-> leftReachMePose);
        }
    }
}