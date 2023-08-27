#include <stdio.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <stdint.h>

#define carCapacity 513
const int capacity = 18313;  //hash table capacity


struct Station{             //memory struct for the hash table
    unsigned int pose;
    int prev;   
    int32_t biggestCar;
    int32_t cars[carCapacity];
    struct Station* next;
};

struct ListEl{              //memory struct for the ordered list and open list used in the dijkstra algorithm
    int pose;
    struct ListEl* next;
};


struct Station** hash;      //hash table

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

int addCarAction(int32_t carValue, struct Station* station, int startIndex);
void removeCarAction(int32_t carValue, struct Station* Station);
void removeBiggestCar(int32_t carValue, struct Station* station);

struct ListEl* findLast(struct ListEl *head);
struct ListEl* growingPartition(struct ListEl *head, struct ListEl *last);
void growingQuickSort(struct ListEl *head, struct ListEl *last);
struct ListEl* descendingPartition(struct ListEl *head, struct ListEl *last);
void descendingQuickSort(struct ListEl *head, struct ListEl *last);

void pathPrinter(int start, int end);
void listInsert(struct ListEl **list, int value);
void printList(struct ListEl *list);
void growingPath(int start, int end);
int growingDijkstra(struct ListEl *sortedList, int end);
void descendingPath(int start, int end);
int descendingDijkstra(struct ListEl *sortedList, int end);

/* program */



int main(){
    hash =  (struct Station**)calloc(capacity,sizeof(struct Station*)); //alloc the hash table 

    parser();  //start to read from the standard input

    return 0;
}

/*################################################################*/
/*              input parser                                      */
/*################################################################*/

//add one or more to the given station
void addCarSupport(struct Station* station, int startIndex){
    char tmp;
    char* car = (char*)malloc(sizeof(char));
    int i=0;
    do{
        tmp = getc(stdin);
        car[i] = tmp;
        i++;
        if(tmp == ' '){
            if(atoi(car)>station->biggestCar) station->biggestCar = atoi(car);

            addCarSupport(station, addCarAction(atoi(car), station, startIndex)); //if there is an other car to add to the same station recall the same function in a recursive way
            
            free(car);
            return;
            }
        car = (char*)realloc(car, (i+1) *sizeof(char));
    }while((tmp!='\n'));  //read the standard input until the command is finished

    if(atoi(car)>station->biggestCar) station->biggestCar = atoi(car);
    addCarAction(atoi(car), station, startIndex);

    free(car);
}

void addStation(){ // command: "aggiungi-stazione"
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
                addCarSupport(node, 0);       
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

void removeStation(){ // command: "demolisci-stazione"
    char input[15];
    if(fgets(input, sizeof(input), stdin)==NULL)return;    
    int pose = atoi(input);
    hashRemove(pose);
} 

void addCar(){ // command: "aggiungi-auto"
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
    addCarSupport(node, 0);

    free(station);
    printf("aggiunta\n");
}
void removeCar(){ // commanc: "rottama-auto"
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

    //if car have to remove is the biggest one could be necessary to update the biggest Car value
    if(atoi(car)==node->biggestCar && node->biggestCar!=0) removeBiggestCar(atoi(car), node);
    else removeCarAction(atoi(car), node);
    

    free(station);
    free(car);
}
void path(){ //command: "pianifica-percorso"
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

    if(start<end) growingPath(start,end);
    else if (start == end) printf("%d\n", start);
    else descendingPath(start, end);  

    free(num);
}

//ignore a specified number of characters
void ignoreChar(int number){
    for(int i=0; i<number;i++)getc(stdin);
}

//ignore the command until find a space
int ignoreUntilSpace(){
    char c = fgetc(stdin);
    while (c!=' '){
        if(c=='\n')return-1;
        c = fgetc(stdin);
    }
    return 0;
}

//ignore the rest of the command
void ignoreAllTheLine(){
    while (fgetc(stdin)!='\n'){}
}

//read the first char of a command and call the correct function to interpret it
void parser(){
    char input;
    while (1){
        input=fgetc(stdin);
        if(input==EOF)return;

        if(input=='a'){
            ignoreChar(8);
            input=fgetc(stdin);
            if(input=='s'){         // command: "aggiungi-stazione"
                ignoreChar(8);
                addStation();
            }
            else{                   // command: "aggiungi-auto"
                ignoreChar(4);
                addCar();
            }
        }else if(input== 'p'){      // command: "pianifica-percorso"
            ignoreChar(18);
            path();
        }else if (input=='d'){      // command: "demolisci-stazione"
            ignoreChar(18);
            removeStation();
        }else {                   // command: "rimuovi-auto"
            ignoreChar(12);
            removeCar();
        }

    }
}

/*################################################################*/
/*              station hash manage                               */
/*################################################################*/

//take an elemento (station) from the hash table, return null if it isn't present
struct Station* hashTake(int pose){
    int hashIndex = pose % capacity;

    if(hash[hashIndex]==NULL)return NULL;  
    if(hash[hashIndex]->pose==pose) return hash[hashIndex];  //return the element in the cell if is the searched one

    //go there only if there is a collision
    struct Station* hashEl = hash[hashIndex];

    while (hashEl->next!=NULL){    //move through the list until reach the last cell or until find the searched value
        if(hashEl->next->pose==pose){
            return hashEl->next;
        }

        hashEl=hashEl->next;        
    }

    return NULL;
}

//add an element (station) to hash table, print "non aggiunta" if the element is already present in the hash table
struct Station* hashInsert(unsigned int pose){
    int hashIndex = pose % capacity;

    if(hash[hashIndex]==NULL){  //insert the element in the hash table if the cell is free
        hash[hashIndex]= (struct Station*)malloc(sizeof(struct Station));
        hash[hashIndex]->pose = pose;
        hash[hashIndex]->biggestCar = 0;
        hash[hashIndex]->next = NULL;
        hash[hashIndex]->prev = 0;
        for(int i = 0; i<carCapacity;i++)hash[hashIndex]->cars[i]=0;
        return hash[hashIndex];
    }

    //go there only if there is a collision

    if(hash[hashIndex]->pose==pose){    //if the element already exist
            printf("non aggiunta\n");
            return NULL; 
    }

    struct Station* hashEl = hash[hashIndex];
    //int i=0;

    while(hashEl->next!=NULL){  //move through the list until reach the first free cell

        if(hashEl->next->pose==pose){    //if the element already exist
            printf("non aggiunta\n");
            return NULL;
        }

        hashEl=hashEl->next;
    }

    //add the station in the free cell
    hashEl->next = (struct Station*)malloc(sizeof(struct Station));
    hashEl->next->pose = pose;
    hashEl->next->biggestCar = 0;
    hashEl->next->next = NULL;
    hashEl->next->prev =0;
    for(int i = 0; i<carCapacity;i++)hashEl->next->cars[i]=0; //initialize the car array all to zero 

    return hashEl->next;
}

//remove an element (station) from the hash table
void hashRemove(int pose){
    int hashIndex = pose % capacity;

    if(hash[hashIndex]==NULL){      //if the cell is free you the searched station isn't present
        printf("non demolita\n");
        return;
    }

    if(hash[hashIndex]->pose==pose){      //if the cell has the serched value remove it
        struct Station* tmp = hash[hashIndex];
        hash[hashIndex] = tmp->next;
        free(tmp);

        printf("demolita\n");
        return;
    }

    //go there only if there is a collision

    struct Station* hashEl = hash[hashIndex];

    while (hashEl->next!=NULL){     //move through the list until reach the last cell   
        if(hashEl->next->pose==pose){       //if it find the searched value remove it
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

//add the car to the first free cell in the cars' array
int addCarAction(int32_t carValue, struct Station* station, int startIndex) {
    if(carValue==0)carValue=-1;
    int i = startIndex;
    while(i<carCapacity){
        if(station->cars[i]==0){
            station->cars[i]=carValue;
            return i;
        }
        i++;
    }
    exit(20); //exit if the array is full
}

//remove the car from the cars' array if it's present
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

//remove the biggest car from the cars' array, and verify if it's necessary to update the biggestCar value
void removeBiggestCar(int32_t carValue, struct Station* station){
    if(carValue==0)carValue=-1;
    int32_t maxValue = 0;
    for(int i = carCapacity-1; i>=0;i--){
        if(station->cars[i]!=carValue && station->cars[i]>maxValue)maxValue=station->cars[i];
        if(station->cars[i]==carValue){
            station->cars[i]=0;
            printf("rottamata\n");
            
            removeBiggestCarSupport(carValue,station,maxValue,i); //have to inspect the reast of the array to find the new biggestCar value

            return;
        }
    }

    printf("non rottamata\n");
    return; 
}

/*################################################################*/
/*             sorting algorithms                                 */
/*################################################################*/

//find the last element in a list, is necessary in the partition algorithm
struct ListEl* findLast(struct ListEl *head){
    struct ListEl* tmp = head;
    while(tmp!=NULL && tmp->next!=NULL){
        tmp = tmp->next;
    }
    return tmp;
}

//partition algorithm for the growing quick sort algorithm
struct ListEl* growingPartition(struct ListEl *head, struct ListEl *last){
    struct ListEl* pivot = head;
    struct ListEl* el = head;
    int tmp = 0;

    while (el !=NULL && el!=last)
    {
        if(el->pose < last->pose){
            pivot = head;
            tmp=head->pose;
            head->pose=el->pose;
            el->pose=tmp;
            head = head->next;
        }
        el = el->next;
    }

    tmp=head->pose;
    head->pose=last->pose;
    last->pose=tmp;
    return pivot;
}

void growingQuickSort(struct ListEl *head, struct ListEl *last){
    if(head==last)return;

    struct ListEl* pivot = growingPartition(head, last);

    if(pivot->next!=NULL)growingQuickSort(pivot->next, last);
    if(head!=pivot)growingQuickSort(head,pivot);
}

//partition algorithm for the descending quick sort algorithm
struct ListEl* descendingPartition(struct ListEl *head, struct ListEl *last){
    struct ListEl* pivot = head;
    struct ListEl* el = head;
    int tmp = 0;

    while (el !=NULL && el!=last)
    {
        if(el->pose > last->pose){
            pivot = head;
            tmp=head->pose;
            head->pose=el->pose;
            el->pose=tmp;
            head = head->next;
        }
        el = el->next;
    }

    tmp=head->pose;
    head->pose=last->pose;
    last->pose=tmp;
    return pivot;
}

void descendingQuickSort(struct ListEl *head, struct ListEl *last){
    if(head==last)return;

    struct ListEl* pivot = descendingPartition(head, last);

    if(pivot->next!=NULL)descendingQuickSort(pivot->next, last);
    if(head!=pivot)descendingQuickSort(head,pivot);
}


/*################################################################*/
/*              path finder                                       */
/*################################################################*/

//entry at the top of the list, used to insert element in the ordered list
void listInsert(struct ListEl **list, int value){
    struct ListEl *el = (struct ListEl*)malloc(sizeof(struct ListEl));
    el->pose = value;
    if(*list==NULL){
        *list=el;
        el->next=NULL;
        return;
    }
    el->next=*list;
    *list=el;
    return;
}

/*          growing path            */

//recursive method used to print the path
void pathPrinter(int start, int end){
    int prev = hashTake(end)->prev;
    if(prev!= start) pathPrinter(start, prev);
    printf("%d ",prev);
}

void printList(struct ListEl *list){
    while(list!=NULL){
        printf("%d", list->pose);
        printf(" | ");
        list=list->next;
    }
    printf("\n");
}

//ordered list constructor for the growing path dijkstra algorithm
struct ListEl* listConstructor(int start, int end){

    struct ListEl *head = (struct ListEl*)malloc(sizeof(struct ListEl));
    head->pose = start;
    head->next=NULL;

    for(int i=0; i<capacity; i++){   //insert all the values between start and end station in the ordered list
        struct Station* hashEl = hash[i];
        while (hashEl!=NULL){        
            if(hashEl->pose>start && hashEl->pose<=end){
                listInsert(&head, hashEl->pose);          
            }
        hashEl=hashEl->next;
        }
    }

    growingQuickSort(head, findLast(head));  //reorder the ordered list with quick sort algorithm
    return head;
}

void growingPath(int start, int end){
    struct ListEl* head = listConstructor(start, end);
    if(growingDijkstra(head, end)==1){  //if the dijkstra return value is different than one there isn't a path
        pathPrinter(start,end);
        printf("%d\n", end);
    }
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

/*          descending path          */


struct ListEl* descendingListConstructor(int start, int end){
    struct ListEl *head = (struct ListEl*)malloc(sizeof(struct ListEl));
    head->pose = start;
    head->next=NULL;   

    for(int i=0; i<capacity; i++){   //insert all the values between start and end station in the ordered list
        struct Station* hashEl = hash[i];
        while (hashEl!=NULL){  
            if(hashEl->pose<start && hashEl->pose>=end){
                listInsert(&head, hashEl->pose);          
            }
        hashEl=hashEl->next;
        }
    }

    descendingQuickSort(head, findLast(head));  //reorder the ordered list with quick sort algorithm
    return head;
}

void descendingPath(int start, int end){
    struct ListEl* head = descendingListConstructor(start, end);
    if(descendingDijkstra(head, end)==1){  //if the dijkstra return value is different than one there isn't a path
        pathPrinter(start,end);
        printf("%d\n", end);
    }
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

                hashTake(last->next->pose)->prev=first->pose; //save the previous step in the hash                

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

        if(first->pose==last->pose){  //move the last pose pointer
                while (last->next!=NULL){
                    last=last->next;
                }
        }

        tmp=first;          //free first el in the open list
        first=first->next;
        free(tmp);

        if(first==NULL){
            printf("nessun percorso\n");
            return 0;
        }

        range = (hashTake(first->pose))->biggestCar;
    }


    return 1;
}