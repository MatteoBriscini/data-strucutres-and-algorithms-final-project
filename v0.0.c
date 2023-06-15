#include <stdio.h>
#include <stdbool.h> 

struct station{
    unsigned int pose;
    unsigned int range;
    unsigned int reachablePose;
    unsigned int aviableCar;
};

/* prototypes */
int updateCarBitMaskPassed(unsigned int oldRange,unsigned int newRange, unsigned long bitMask);
int updateCarBitMaskRemoved(unsigned int oldRange, unsigned int newRange, unsigned long  int bitMask);
int carBitmaskOffset(unsigned int range, unsigned int carValue);
bool isCar(unsigned int range, unsigned int carValue, unsigned int bitMask);

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
    struct station tmp = {32,32,0,1};
    tmp.aviableCar = tmp.aviableCar + carBitmaskOffset(tmp.range, 27);
    tmp.aviableCar = tmp.aviableCar + carBitmaskOffset(tmp.range, 29);
    printf("%d", tmp.aviableCar);
        printf("\n");
    printf("%d\n", isCar(tmp.range,32,tmp.aviableCar));
        printf("\n");
    printf("%d\n", isCar(tmp.range,27,tmp.aviableCar));
        printf("\n");
    printf("%d\n", isCar(tmp.range,23,tmp.aviableCar));
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