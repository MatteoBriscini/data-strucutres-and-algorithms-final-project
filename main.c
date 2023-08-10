#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <stdint.h>

#define carCapacity 513
const int capacity = 18313;  //hash size: > 57639 


struct Station{
    unsigned int pose;
    int prev;   
    int32_t biggestCar;
    int32_t cars[carCapacity];
    struct Station* next;
};

struct ListEl{
    int pose;
    struct ListEl* next;
};



struct Station** hash;
struct Station* garbage;

/* prototypes */
void ignoreAllTheLine();
int ignoreUntilSpace();
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

void pathPrinter(int start, int end);
void listInsert(struct ListEl *list, int value);
void printList(struct ListEl *list);
void growingPath(int start, int end);
int growingDijkstra(struct ListEl *sortedList, int end);
void descendingPath(int start, int end);
int descendingDijkstra(struct ListEl *sortedList, int end);
void descendingDijkstraP2(struct ListEl *openList, int end);

/* program */



int main(){
    hash =  (struct Station**)calloc(capacity,sizeof(struct Station*));

    parser();

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

    if(atoi(car)>=2147483647 || atoi(car)<0) exit(9);

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

            if(tmp== ' ' && ignoreUntilSpace()!=-1){
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


    if(start<end) growingPath(start,end);
    else if (start == end) printf("%d\n", start);
    else descendingPath(start, end);  

    free(num);
}

void ignoreChar(int number){
    for(int i=0; i<number;i++)getc(stdin);
}
int ignoreUntilSpace(){
    char c = fgetc(stdin);
    while (c!=' '){
        if(c=='\n')return-1;
        c = fgetc(stdin);
    }
    return 0;
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
        hash[hashIndex]->prev = 0;
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
    hashEl->next->prev =0;
    for(int i = 0; i<carCapacity;i++)hashEl->next->cars[i]=0;

    return hashEl->next;
}

//remove an element from the hash table
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
                printf("pose: %d car:%d prev: %d | ", hashEl->pose, hashEl->biggestCar, hashEl->prev);
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

/*          growing path            */

void pathPrinter(int start, int end){
    int prev = hashTake(end)->prev;
    if(prev!= start) pathPrinter(start, prev);
    printf("%d ",prev);
}

void printList(struct ListEl *list){
    while(list!=NULL){
        printf("%d", list->pose);
        printf(" : *%d*", hashTake(list->pose)->biggestCar);
        printf(" | ");
        list=list->next;
    }
    printf("\n");


}

void listInsert(struct ListEl *list, int value){

    struct ListEl *el = (struct ListEl*)malloc(sizeof(struct ListEl));

    while(list->next!=NULL){
        if(list->next->pose>value){
            el = list->next;
            list->next = (struct ListEl*)malloc(sizeof(struct ListEl));
            list->next->pose = value;
            list->next->next=el;
            return;
        }
        list=list->next;
    }
    list->next= (struct ListEl*)malloc(sizeof(struct ListEl));
    list->next->pose = value;
    list->next->next=NULL;

}

struct ListEl* listCostructor(int start, int end){

    struct ListEl *head = (struct ListEl*)malloc(sizeof(struct ListEl));
    head->pose = start;
    head->next=NULL;

    for(int i=0; i<capacity; i++){
        struct Station* hashEl = hash[i];
        while (hashEl!=NULL){        
            if(hashEl->pose>start && hashEl->pose<=end){
                listInsert(head, hashEl->pose);          
            }
        hashEl=hashEl->next;
        }
    }

    return head;
}

void growingPath(int start, int end){
    struct ListEl* head = listCostructor(start, end);

    /*
    printList(head);
    printf("\n");
    printStationHash();
    */

    if(growingDijkstra(head, end)==1){
        pathPrinter(start,end);
        printf("%d\n", end);
    }
    //exit(100);
}

int growingDijkstra(struct ListEl *sortedList, int end){
    struct ListEl *first = (struct ListEl*)malloc(sizeof(struct ListEl));   //add the first el in the order list to the open list
    first->pose = sortedList->pose;
    first->next = NULL;
    struct ListEl *last = first;
    struct ListEl *tmp = sortedList;
    int maxPose=0;

    sortedList = sortedList->next;      //free the el in the order list
    free(tmp);
    tmp=sortedList;

    int range = (hashTake(first->pose))->biggestCar;

    //struct ListEL *tmp = sortedList;

    while (sortedList!=NULL && sortedList->pose<=end){
        if(first->pose+range>maxPose && first->pose+range>=sortedList->pose){
            while(1){
                last->next = (struct ListEl*)malloc(sizeof(struct ListEl)); //add the el as last in the list
                last->next->pose=sortedList->pose;
                if(sortedList->pose>maxPose)maxPose=sortedList->pose;  

                hashTake(last->next->pose)->prev=first->pose; //save the previous step in the hash

                sortedList = sortedList->next;      //free the el in the order list
                free(tmp);
                tmp=sortedList;

                if(sortedList==NULL || (first->pose)+range<sortedList->pose){
                    break;
                    }

                last=last->next;        //skip to the next el
            }

            last=last->next;        //skip to the next el

            /*
            printf("open list:  ");
            printList(first);
            printf("order list: ");
            printList(sortedList);
            printf("%d\n", last->pose);
            printf("\n");
            */

        }

        tmp=first;          //free first el in the open list
        first=first->next;
        free(tmp);
        tmp=sortedList;

        if(first==NULL){
            printf("nessun percorso\n");
            return 0;
        }

        range = (hashTake(first->pose))->biggestCar;
    }

    return 1;
}

/*          descendin path          */

void descendingListInsert(struct ListEl *list, int value){

    struct ListEl *el = (struct ListEl*)malloc(sizeof(struct ListEl));

    while(list->next!=NULL){
        if(list->next->pose<value){
            el = list->next;
            list->next = (struct ListEl*)malloc(sizeof(struct ListEl));
            list->next->pose = value;
            list->next->next=el;
            return;
        }
        list=list->next;
    }
    list->next= (struct ListEl*)malloc(sizeof(struct ListEl));
    list->next->pose = value;
    list->next->next=NULL;
}

struct ListEl* descendingListCostructor(int start, int end){
    struct ListEl *head = (struct ListEl*)malloc(sizeof(struct ListEl));
    head->pose = start;
    head->next=NULL;   

    for(int i=0; i<capacity; i++){
        struct Station* hashEl = hash[i];
        while (hashEl!=NULL){  
            if(hashEl->pose<start && hashEl->pose>=end){
                descendingListInsert(head, hashEl->pose);          
            }
        hashEl=hashEl->next;
        }
    }

    return head;
}

void descendingPath(int start, int end){

    struct ListEl* head = descendingListCostructor(start, end);

    if(descendingDijkstra(head, end)==1){
        pathPrinter(start,end);
        printf("%d\n", end);
    }

    //printStationHash();
}

int descendingDijkstra(struct ListEl *sortedList, int end){
    struct ListEl *first = (struct ListEl*)malloc(sizeof(struct ListEl));   //add the first el in the order list to the open list
    first->pose = sortedList->pose;
    first->next = NULL;
    struct ListEl *last = first;
    struct ListEl *tmp = sortedList;
    int minPose=first->pose;

    sortedList = sortedList->next;      //free the el in the order list
    free(tmp);

    int range = (hashTake(first->pose))->biggestCar;

    while (sortedList!=NULL && sortedList->pose>=end){
        if(first->pose-range<=minPose && first->pose-range<=sortedList->pose){
            while(1){

                if(last->next==NULL){
                    last->next = (struct ListEl*)malloc(sizeof(struct ListEl));
                    last->next->pose=sortedList->pose;
                    last->next->next=NULL;
                }else{
                    tmp = last->next;
                    last->next = (struct ListEl*)malloc(sizeof(struct ListEl));
                    last->next->pose=sortedList->pose;
                    last->next->next=tmp;
                }

                hashTake(last->next->pose)->prev=first->pose; //stave the previous step in the hash                

                if(sortedList->pose<minPose){
                    minPose=sortedList->pose; 
                } 

                tmp=sortedList;
                sortedList = sortedList->next;      //free the el in the order list
                free(tmp);
                tmp=sortedList;

                if(sortedList==NULL || (first->pose)-range>sortedList->pose){
                    break;
                }  
            }
        }

        if(first->pose==last->pose){
                while (last->next!=NULL){
                    last=last->next;
                }
        }

        /*
        printf("order list: ");
        printList(sortedList);
        printf("open list: ");
        printList(first);
        printf("%d\n", minPose);
        printf("first: %d last: %d\n", first->pose, last->pose);
        printf("\n");*/

        tmp=first;          //free first el in the open list
        first=first->next;
        free(tmp);

        if(first==NULL){
            printf("nessun percorso\n");
            return 0;
        }

        range = (hashTake(first->pose))->biggestCar;
    }

    //if(first!=NULL)descendingDijkstraP2(first, end);

    return 1;
}

void descendingDijkstraP2(struct ListEl *openList, int end){
    struct ListEl *tmp = openList;
    //struct Station *endStation = hashTake(end);
    //struct Station *selectedStation = hashTake(openList->pose);

    printf("open list: ");
    printList(openList);

    while (openList->pose>end && openList!=NULL)
    {
        //if(openList->pose-selectedStation->biggestCar<=end && endStation->prev>openList->pose)endStation->prev=openList->pose;

        openList=openList->next;
        free(tmp);
        tmp=openList;
    }
    

}