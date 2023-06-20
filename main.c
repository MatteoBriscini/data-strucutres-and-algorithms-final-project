#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>

struct Station{
    unsigned int pose;
    unsigned int biggestCar;
    unsigned int leftReachablePose;
    unsigned int rightReachablePose;
    struct Car* aviableCar;
};
struct Car{
    unsigned int avaiableCar;
    struct Car* next;
};

const int capacity = 47;  //hash size
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
void addCarAction(unsigned int* range, unsigned int car, struct Car* bitMask);
void removeCarAction(unsigned int* range, unsigned int car, struct Car* bitMask, unsigned int count);
void printCars(struct Car* bitMask);

void hashInsert(unsigned int pose,unsigned int range);
int hashFind(int pose);
struct Station* hashTake(int pose);
void hashRemove(int hashIndex);

void createReachablePose(int hashIndex);

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
    addCarAction(range, atoi(car), node->aviableCar);

    //printf("%d: %d",node->biggestCar,node->aviableCar->avaiableCar);

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
    hashInsert(atoi(input), 0);
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

    printf("%d: %d\n",node->biggestCar,node->aviableCar->avaiableCar);

    unsigned int* range =  &node->biggestCar;
    if(isCar(node->biggestCar, atoi(car), node->aviableCar->avaiableCar)==0){ printf("non rottamata\n"); return;}
    removeCarAction(range, atoi(car), node->aviableCar, 0);

    printf("%d: %d\n",node->biggestCar,node->aviableCar->avaiableCar);
    printf("rottamata\n");

    free(station);
}
void path(char* data){ //pianifica-percorso

}

//read input and call correct function
void parser(){
    char delim[] = " ";
    while (1){
        char input[255];
        char *pointer = input; 
        fgets(input, sizeof(input), stdin);
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

//add an element to hash table (to add void station set range to 0) 
void hashInsert(unsigned int pose,unsigned int range){
    struct Station* node = (struct Station*)malloc(sizeof(struct Station));
    node->pose = pose;
    node->biggestCar = range;
    node->aviableCar= (struct Car*)malloc(sizeof(struct Car*));
    if(range!=0)node->aviableCar->avaiableCar= 1;

    int hashIndex = pose % capacity;
    int relativeIndex = 0;

    while(hash[hashIndex+relativeIndex]!=NULL){         
        if(relativeIndex == hashIndex)break;                //hash piena, andra` riallocata una piu grossa
        if(relativeIndex == capacity) relativeIndex = -hashIndex;
        relativeIndex++;
    }

    hash[hashIndex+relativeIndex] = node;
}

void hashRemove(int hashIndex){
    hash[hashIndex]=garbage;
}

/* car manage */

//add the car to the bitMask
void addCarAction(unsigned int* range, unsigned int car, struct Car* bitMask){
    if(car>*range){
        updateCarBitMaskPassed(*range, car, bitMask);
        *range = car;
    }
    if(isCar(*range,car,bitMask->avaiableCar) && bitMask->next==NULL){
        bitMask->next = (struct Car*)malloc(sizeof(struct Car*));
        return addCarAction(range,car,bitMask->next);
    }
    else if(isCar(*range,car,bitMask->avaiableCar) && bitMask->next!=NULL){
        return addCarAction(range,car,bitMask->next);
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
