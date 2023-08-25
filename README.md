# API_project
#### Politecnico of Milan [2022-2023]
## Project requirements
Considering a highway as a sequence of service stations, each station is  identified by the distance from the start of the highway, the highway is runnable in both directions. <br>
Each station has some vehicles (caraterized by the autonomy of battery), with limitation of 512 cars for a single station. <br> 
A trip is identified by a sequence of stations where a driver stop, assuming that the driver changes the car each time he stops in a station, and he can't  change direction on the highway. <br>
Given a couple of stations, the goal of the project is to find the path, from the first station to the second, with fewer stops possible. If exist more path with the same number of stops, the algorithms has to choose, from right to left, the path with smaller station value. <br>
*example: 10-50-<span style="color: red">60</span>-90 is better than 10-30-<span style="color: red">85</span>-90, because 60<85.*

>**Note**: you can find the official requirements: [here](https://github.com/MatteoBriscini/API_project/blob/master/Deliveries/api_technicalSpecification.pdf).
### Command and expected response
>**Note**: this project is a part of an italian course, so commands are written in italian.
* ***Add-station distance cars-number car1-autonomy.... carN-autonomy:*** add a station on the specified position. <br>
    Expected response: *"aggiunta"* if the station is added or *"non aggiunta"* if the station is already present. <br>
    Example: (add a station on the position 10 with 3 cars)
    ```
      aggiungi-stazione 10 3 100 200 300
    ```
  <br>
* ***Remove-station distance:*** remove the station at the specified distance and all cars in that station. <br>
    Expected response: *"demolita"* if the station has been removed or *"non demolita"* if the station isn't present. <br>
    Example:
    ```
      demolisci-stazione 10
    ```
  <br>
* ***Add-car station-distance car-autonomy:*** add a car with specified autonomy to the specified station. <br>
    Expected response: *"aggiunta"* if the car has been added to the station or *"non aggiunta"* if the station isn't present. <br>
    Example: (add a car with 20 of autonomy in the station 10)
    ```
      aggiungi-auto 10 20
    ```
    >**Note**: is always possible to add two car with same autonomy in the same station.
  
    <br>
* ***Remove-car station-distance car-autonomy:*** remove a car with specified autonomy from the specified station. <br>
    Expected response: *"rottamata"* if the car has been removed from the station or *"non rottamata"* if the station or the car doesn't exist. <br>
    Example: (remove a car with 20 of autonomy from the station 10)
    ```
      rottama-auto 10 20
    ```
  <br>
* ***planning-route start-station-distance end-station-distance:*** ask to find a path from start station to the end. <br>
    Expected response: the sequence of stations or *"nessun percorso"* if doesn't exist a route. <br>
    Example: (search a path from the station 10 to the station 20)
    ```
      pianifica-percorso 10 20
    ```
  <br>
#### Simple example

Following you can find a short sequence of input commands and the correct output.
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

![alt text](https://github.com/MatteoBriscini/API_project/blob/master/Deliveries/example.png)
</details><br>

## Design and implementation choices

### Memory structure

* **Station:** the stations are memorized in a hash table. <br>
    The station position on the highway is used as hash value (hash function: stationPosition % hashSize). <br><br>
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

    >**Note**: the hash table is also used to save the path (saving the previous step on the prev value in the Station struct).

* **Car:** cars are simply saved in an int32_t array (in the Station struct), with 513 places, in fact a station can't have more than 512 cars. <br>
    When a car is added or removed, the algorithm has to take updated the biggestCar value (biggestCar value is needed to estimate the optimal path).
### Path find algorithm

To find the optimal path the implemented algorithm is a variation of the Dijkstra algorithm, adapted to work in a single dimensional space. <br>
For each station the algorithm take in to account the biggest car only (all the positions reachable from a car with range A are also reachable from another car with size B if B>A). <br>
We can split the full algorithm in 3 sub operations: creation of an ordered list, the actual Dijkstra algorithm, and the path print.<br>

* ***Creation of an ordered list:*** in this phase the algorithm explore the entire hash table, select the stations between the start end the final position, and insert the selected station on the list head. <br>
When the list is completed (the algorithm has explored the entire hash table), the list is reordered with a classic quick sort (average complexity of O(n*log(n)) ).<br><br>
* ***Adapted Dijkstra algorithm:*** Dijkstra works on the ordered list, and use, as support another list (called open list). <br>
 To initialize the algorithm the first element (the start position) in the ordered list is added to open list and removed from the ordered list.<br>
 The algorithm select the first element in the open list (station A), and the first element in the ordered list (station B), if A can reach B (the station B is considered reachable from A if the biggest car of A has enough battery to cover the distance between A & B)
 B is added to the open list and A is saved as B prev value in the hash table.<br>
 When the algorithm find the first element in the ordered list unreachable from A, A is removed from the open list.<br>
 All this proces is reiterated until the end position is reached. <br>
 The implemented algorithm is a bit different if the path is from left to right (growingPath), or right to left (descendingPath) in terms of: order of the ordered list, insertion order in the open list. <br>
  * ***growingPath:*** growing ordered list, insertion on the end of the list.
  
  <img src="https://github.com/MatteoBriscini/API_project/blob/master/Deliveries/growingPath.gif" width="700"/>
  
  * ***descendingPath:*** decreasing ordered list, custom list insertion.
    <details>
        <summary>Custom list insertion:</summary>
    
    Work with two pointer: 
    * ***First:*** always point the first element of the open list.
    * ***Last:*** elements are always added in the open list on last->next. <br> this pointer doesn't move until *First* and *Last* points to the same element, when this happens *Last* moved to the last element in the open list.
    
    </details><br>

* ***path's printer:*** the path is saved on the hash table in terms of previous steps, a recursive algorithm explores the path (from the end to the start) and print it.
    ``` 
    //recursive method used to print the path
    void pathPrinter(int start, int end){
        int prev = hashTake(end)->prev;
        if(prev!= start) pathPrinter(start, prev);
        printf("%d ",prev);
    }
    ``` 
## Project evaluation

For the project evaluation has been used an online tester based on unknown test case, any way the professor supported the project with 111 testing files (reported [here](https://github.com/MatteoBriscini/API_project/tree/master/test)). <br>
We suppose the online tester is similar in to complexity of the [open_108.txt](https://github.com/MatteoBriscini/API_project/blob/master/test/open_108.txt) test case.

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
Compile the program with the correct flags for the project:
```
    cc main.c -o main -Wall -Werror -std=gnu11 -O2 -lm
``` 

Run the chosen test and save the result in a text file:
```
    ./main < ./test/open_108.txt > open
``` 
Compare the result with the attended ones:
```
    diff ./open ./test/open_111.output.txt
``` 
Time evaluation (not mandatory):
```
    valgrind --tool=callgrind ./main < ./test/open_111.txt >open
    kcachegrind callgrind.out."sessionCode"
``` 
>**Note**: the use of valgrind effect the real program performance, the online tester don't use this method to evaluate the project. <br>
> The program speed can be also influenced by the specific machine performances.
