# API_project
#### Politecnico of Milan [2022-2023]
## Project requirements
Considering a highway as a sequence of service stations, each station is  identified by the distance from the start of the highway. <br>
Each station has some vehicles (identify by the battery autonomy), limited to 512 cars for a single station. <br> 
A trip is identified by a sequence of stations (where the driver stop), assuming that the driver can't  change direction during the trip, and he changes the car each time he stop in a station. <br>
given a couple of station the goal of the project is to find the path, from the start station to the end, with fewer stops possible. If exist more path with the same number of stops the project has to choose the ones that give priority at the stations with shorter distance from the highway's start. <br> 

>**Note**: you can find the official requirements hear.
### Command and expected response
>**Note**: this is a project of an italian course, so commands are write in italian.
* ***Add-station distance cars-number car1-autonomy.... carN-autonomy:*** add a station at the specified position. <br>
    Expected response: *"aggiunta"* if the station is added or *"non aggiunta"* if the station is already present. <br>
    Example: (add a station on the position 10 with 3 cars)
    ```
      aggiungi-stazione 10 3 100 200 300
    ```
* ***Remove-station distance:*** destroy the station at the specified distance and all the cars in that station. <br>
    Expected response: *"demolita"* if the station has been removed or *"non demolita"* if the station isn't present.
    Example:
    ```
      demolisci-stazione 10
    ```
* ***Add-car station's-distance car's-autonomy:*** add a car with specified autonomy to the specified station. <br>
    Expected response: *"aggiunta"* if the car has been added to the stations or *"non aggiunta"* if the station isn't present.
    Example: (add a car with 20 of autonomy in the station 10)
    ```
      aggiungi-auto 10 20
    ```
* ***Remove-car station's-distance car's-autonomy:*** remove a car with specified autonomy from the specified station. <br>
    Expected response: *"rottamata"* if the car has been removed from the station or *"non rottamata"* if the station or the car doesn't.
    Example: (remove a car with 20 of autonomy from the station 10)
    ```
      rottama-auto 10 20
    ```
* ***planning-route start-station's-distance end-station's-distance:*** ask to find a path from start station to the end. <br>
    Expected response: the sequence of stations or *nessun percorso* if doesn't exist a routeroot.
    Example: (search a path from the station 10 to the station 20)
    ```
      pianifica-percorso 10 20
    ```
#### Little example

Following you can find a short sequence of input command with the correct output.
<details>
<summary>see more:</summary>

    //  input                             output
    aggiungi-stazione 20 3 5 10 15  //-> aggiunta
    aggiungi-stazione 4 3 1 2 3     //-> aggiunta
    aggiungi-stazione 30 0          //-> aggiunta
    demolisci-stazione 3            //-> non demolita
    demolisci-stazione 4            //-> demolita
    pianifica-percorso 30 20        //-> nessun percorso
    aggiungi-auto 30 40             //-> aggiunta
    aggiungi-stazione 50 3 20 25 7  //-> aggiunta
    rottama-auto 20 8               //-> non rottamata
    rottama-auto 9999 5             //-> non rottamata
    pianifica-percorso 20 50        //-> 20 30 50

</details><br>

## Design and implementation choices

### Memory struct

* **Station:** the station are memorized in a hash table. <br>The station pose on the highway is used as hash value (hash function: stationPose % hashSize). <br><br>
    <details>
        <summary>The struct for each element of the hash table is:</summary>
    
    ``` 
    struct Station{
        unsigned int pose;       //position on the highway
        int prev;               //previous station in the path
        int32_t biggestCar;     //biggest car present in the station
        int32_t cars[carCapacity];      //all cars present in the station (carCapacity = 513)
        struct Station* next;       //next element in the single linked list (in case of collision)
    };
    ```
    </details><br>
    Collision are managed with the open hash idea, so element with the same hash value are saved on a single linked list.
  
    >**Note**: the hash table has a dimension of 18313 elements.

    >**Note**: the hash table is also used to save the path (saving the previous step on the perv value, in the Station's struct).

* **Car:** cars are simply saved in an int32_t array (in the Station struct), with 513 places, in fact a station can't have more than 512 cars. <br>
    When a car is added or removed the algorithm has to take updated the biggestCar value (useful when have to estimate the optimal path).
### Path find algorithm

To find the optimal path the implemented algorithm is a variation of the Dijkstra algorithm, adapted to work in a single dimensional space. <br>
For each station the algorithm only take into consideration the biggest car (all the position reachable from a car with a range of A are reachable from a car with size B if B>A). <br>
We can split the full algorithm in 3 sub operation: creation of an ordered list, the actual Dijkstra algorithm, and the path's print:<br>

* ***Creation of an ordered list:*** in this phase the algorithm move through the entire hash table, select the stations between the start end the final position, and insert in the head of a list. <br>
When the list is completed (the algorithm has explored the entire hash table), the list is reordered with a classic quick sort (average complexity of O(n*log(n)) ).<br><br>
* ***Adapted Dijkstra algorithm:*** Dijkstra work on the ordered list, and use, as support another list (called open list). <br>
 To initialize the algorithm the first element (the start position) in the ordered list is added to open list and removed from the ordered list.<br>
 The algorithm select the first element in the open list (station A), and the first in the ordered list B, if A can reach B (the station B is considered reachable from A if the biggest car of A is big enough to cover the distance between A & B)
 B is added to the open list and A is saved as B prev value in the hash table.<br>
 when the algorithm reach the first element in the ordered list unreachable from A, A is removed from the open list.<br>
 all this process is reiterated until the end position is reached. <br>
 The implemented algorithm is a bit different if the end is bigger than the start (growingPath), or the start is bigger than the end (descendingPath), in terms of order of the ordered list and insertion order in the open list. <br>
  * ***growingPath:*** growing ordered list, insertion on the end of the list.
  * ***descendingPath:*** decreasing ordered list, custom list insertion.
    <details>
        <summary>Custom list insertion:</summary>
    
    Work with two pointer: 
    * ***First:*** always point the first element of the open list.
    * ***Last:*** elements are always added in the open list on last->next, this pointer doesn't move until first and last point to the same element, when this happens last is moved to the last element in the open list.
    
    </details><br>

* ***path's printer:*** the path is saved on the hash table in terms of previous steps, a recursive algorithm explore the path (from the end to the start) and print it.
    ``` 
    //recursive method used to print the path
    void pathPrinter(int start, int end){
        int prev = hashTake(end)->prev;
        if(prev!= start) pathPrinter(start, prev);
        printf("%d ",prev);
    }
    ``` 
## Project evaluation

For the evaluation we used an online tester based on un unknown test cases, any way the professor give us 111 test case (reported here). <br>
We suppose the online is similar in to complexity of the 108 test case.

| Evaluation | maximums used memory | maximums used time |
|------------|----------------------|--------------------|
| 18         | 19,0s                | 128 MiB            |
| 21         | 15,0s                | 118 MiB            |
| 24         | 10,0s                | 108 MiB            |
| 27         | 6,0s                 | 98 MiB             |
| 30         | 4,0s                 | 88 MiB             |
| 30_lode    | 1,0s                 | 78 MiB             |

### Our project results
| maximums used memory | maximums used time |
|----------------------|--------------------|
| 0,662s               | 41 MiB             |
### Reproduce a local test
On Linux:<br>
compile the program with the correct project's flags:
```
    cc main.c -o main -Wall -Werror -std=gnu11 -O2 -lm
``` 

run the desired test and save the result in a text file:
```
    ./main < ./test/open_108.txt > open
``` 
compare the result with the attended ones:
```
    diff ./open ./test/open_111.output.txt
``` 
time evaluation (not mandatory):
```
    valgrind --tool=callgrind ./main < ./test/open_111.txt >open
    kcachegrind callgrind.out."sessionCode"
``` 
>**Note**: the use of valgrind effect the real program performance, the online tester didn't use this method to evaluate the project. <br>
> the program speed can also been influenced by the performance of the specific machine.