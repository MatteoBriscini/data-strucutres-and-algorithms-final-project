#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>

struct Station{
    unsigned int pose;
    unsigned int range;
    unsigned int reachablePose;
    unsigned int aviableCar;
};

const int capacity = 47;  //hash size
struct Station** hash;
struct Station* garbage;

/* prototypes */
int updateCarBitMaskPassed(unsigned int oldRange,unsigned int newRange, unsigned long bitMask);
int updateCarBitMaskRemoved(unsigned int oldRange, unsigned int newRange, unsigned long  int bitMask);
int carBitmaskOffset(unsigned int range, unsigned int carValue);
bool isCar(unsigned int range, unsigned int carValue, unsigned int bitMask);
void hashInsert(unsigned int pose,unsigned int range,unsigned int aviableCar);
int hashFind(int pose);
void hashRemove(int hashIndex);

/* program */

unsigned int power(int base, int esp){
    unsigned int result = 1;
    for(int i=0;i<esp;i++){
        result = result *base; 
    }
    return result;
}

void addStation(char data[]){ //aggiungi-stazione
} 

void removeStation(char data[]){ //demolisci-stazione
} 

void addCar(char data[]){ //aggiungi-auto
}
void removeCar(char data[]){ //rottama-auto
}

void path(char data[]){ //pianifica-percorso
}

//read input and call correct function
void parser(){
    while (1){
        char input[256];
        fgets(input, sizeof(input), stdin);
        if(input[0] == 'p'){
            path("percorso");
        }else if (input[0] == 'a'){
            if(input[9]=='s') addStation(input);
            else addCar(input);
        }else if(input[0] == 'd'){
            removeStation(input);
        }else{
            removeCar(input);
        }
    }
    
}

int main(){

    hash = (struct Station**)malloc(sizeof(struct Station*)* capacity);

    hashInsert(203, 3, 1);
    hashInsert(297, 3, 1);

    printf("%d", hashFind(203));
        printf("\n");
    printf("%d", hashFind(202));
        printf("\n");
    printf("%d", hashFind(297));
        printf("\n");

    hashRemove(hashFind(297));

    printf("%d", hashFind(297));
        printf("\n");

    printf("ciao");
                printf("\n");
}


/* car manage */

//called when new car added and is bigger than range
int updateCarBitMaskPassed(unsigned int oldRange,unsigned int newRange, unsigned long bitMask){
    return bitMask*power(2,newRange-oldRange)+1;
}
//call when bigger car is been removed
int updateCarBitMaskRemoved(unsigned int oldRange, unsigned int newRange, unsigned long  int bitMask){
    return (bitMask-1)/power(2,oldRange-newRange);
}
//offeset to update bitMask when car added or remove [if add bitMask = bitMask + offset | if remove bitMask = bitMask - offset ]
int carBitmaskOffset(unsigned int range, unsigned int carValue){
    return power(2,range-carValue);
}
//to verify if there is the car in the aviableCard bitMask 
bool isCar(unsigned int range, unsigned int carValue, unsigned int bitMask){
    if((power(2,range-carValue)&bitMask)!=0) return true;
    return false;
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

//add an element to hash table (to add void station set range and avaiableCar to 0)
void hashInsert(unsigned int pose,unsigned int range,unsigned int aviableCar){
    struct Station* node = (struct Station*)malloc(sizeof(struct Station));
    node->pose = pose;
    node->range = range;
    node->aviableCar =  aviableCar;

    int hashIndex = pose % capacity;
    int relativeIndex = 0;

    while(hash[hashIndex+relativeIndex]!=NULL){         
        if(relativeIndex == hashIndex)break;
        if(relativeIndex == capacity) relativeIndex = -hashIndex;
        relativeIndex++;
    }

    hash[hashIndex+relativeIndex] = node;
}

void hashRemove(int hashIndex){
    hash[hashIndex]=garbage;
}