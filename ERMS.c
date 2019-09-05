#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>

/*****CONSTANT DEFINITIONS*****/
#define MAXQUEUE 50

/***** STRUCTURE DECLARATIONS*****/
typedef struct
{
  void **heapAry;
  int last;
  int size;
  int (*compare) (void *argu1, void *argu2);
  int maxSize;
} HEAP;

typedef struct
{
	char *name;
	int priority;
	int id;
	char *arrivalTime;
} PATIENT;

/***** STANDARD HEAP FUNCTIONS*****/
HEAP *heapCreate (int maxSize, int (*compare) (void *patient1, void *patient2));
bool heapInsert (HEAP * heap, void *dataPtr);
bool heapDelete (HEAP * heap, int deleteLoc);
int compare (void *patient1, void *patient2);
static void _reheapUp (HEAP * heap, int childLoc);
static void _reheapDown (HEAP * heap, int root);
void process (HEAP * heap);

/***** USER INTERFACE FUNCTIONS*****/
int getChoice (void);
PATIENT *getPatient (void);
void printHeap (void *heapArray);
bool pullPatient (HEAP* heap, int *smallest, int index);
void getCriticality (void *patient, int target);
void printLow(HEAP *heap, int numPatients);


int main (void)
{

  setbuf (stdout, NULL);  //for scanf

  HEAP *patientHeap;  //Heap definition

  
  //statements
  puts ("Welcome to the emergency room management system (ERMS).");
  patientHeap = heapCreate (MAXQUEUE, compare);
  
  process (patientHeap);
  free(patientHeap);
  return 0;
}
/***** STANDARD HEAP FUNCTIONS*****/
HEAP * heapCreate (int maxSize, int (*compare) (void *patient1, void *patient2)){
  HEAP *heap;
  heap = (HEAP *) malloc (sizeof (HEAP));
  if (!heap)
    {
      puts("Error allocating memory for heap in heapCreate function.");
      return NULL;
    }
  //define heap parameters
  heap->last = -1;
  heap->compare = compare;

  //force maxsize to 0
  heap->maxSize = (int) pow (2, ceil (log2 (maxSize))) - 1;
  heap->heapAry = (void *) calloc (heap->maxSize, sizeof (void *));
  return heap;
}

bool heapInsert (HEAP * heap, void *dataPtr){

    if (heap->size == 0)
    {
        heap->size = 1;
        heap->last = 0;
        heap->heapAry[heap->last] = dataPtr;
        return true;
    }				//end if size == 0

    if (heap->last == heap->maxSize - 1)
        return false;
  
    ++(heap->last);
    ++(heap->size);
    heap->heapAry[heap->last] = dataPtr;

    _reheapUp (heap, heap->last);
    return true;
}

bool heapDelete (HEAP * heap, int deleteLoc){
  if (heap->size == 0)
    {
      puts ("Error: nothing in the heap to delete.");
      return false;
    }
  heap->heapAry[deleteLoc] = heap->heapAry[heap->last];
  (heap->last)--;
  (heap->size)--;
  _reheapDown (heap, 0);
  return true;
}

int compare (void *patient1, void *patient2){
    //local definitions
    PATIENT p1, p2;

    /***** STATEMENTS *****/
    //Set temporary PATIENT variables
    p1 = *(PATIENT *) patient1;
    p2 = *(PATIENT *) patient2;

    //prioritize the patients
    if (p1.priority <p2.priority)
    	return -1;
    else if (p1.priority > p2.priority)
    	return 1;

    else
    	if (p1.id >p2.id)
    		return 2;
    	else
    		return 3;
}

void _reheapUp (HEAP * heap, int childLoc){
    //local definitions
    int parent;
    void **heapAry;
    void *hold;

    if (childLoc)
    {
        heapAry = heap->heapAry;
        parent = (childLoc - 1) / 2;

        if (heap->compare (heapAry[childLoc], heapAry[parent]) > 0)
        {
            hold = heapAry[parent];
            heapAry[parent] = heapAry[childLoc];
            heapAry[childLoc] = hold;
            _reheapUp (heap, parent);
        }
    }
    return;
}

void _reheapDown (HEAP * heap, int root){

    //local definitions
    void *hold, *leftData, *rightData;
    int largeLoc, last;

    //statements
    last = heap->last;
    if ((root * 2 + 1) <= last)
    {
        leftData = heap->heapAry[root * 2 + 1];
        if ((root * 2 + 2) <= last)
            rightData = heap->heapAry[root * 2 + 2];
        else
            rightData = NULL;

        if ((!rightData) || heap->compare (leftData, rightData) > 0)
            largeLoc = root * 2 + 1;
        else
            largeLoc = root * 2 + 2;
        if (heap->compare (heap->heapAry[root], heap->heapAry[largeLoc]) < 0)
        {
            hold = heap->heapAry[root];
            heap->heapAry[root] = heap->heapAry[largeLoc];
            heap->heapAry[largeLoc] = hold;
            _reheapDown (heap, largeLoc);
        }
    }
  return;
}

/***** PROCESSES PROGRAM *****/
void process (HEAP * heap){
    //local definitions
    PATIENT *patient;
    bool result;
    int choice; //stores user decision
    int numPatients; //stores number of patients in tree
    int smallIndex; //used to store the index of the smallest (most) critical status
    numPatients = 0;

    //statements
    do
    {
        choice = getChoice ();
        switch (choice)
        {
            case 1:
                patient = getPatient ();
                numPatients++;
                patient->id = patient->priority * 10000 + (10000 - numPatients);
                result = heapInsert (heap, patient);
                if (!result)
                {
                    puts ("Error inserting into heap\n");
                    exit (101);
                }
            break;
            case 2:
            	if (numPatients==0)
            	{
            		puts("All patients have been discharged.");
            		break;
            	}
            	smallIndex=0;
            	pullPatient(heap, &smallIndex, 0);
                puts("The next patient with the highest priority is:");
                printf("%20.20s\tPriority\tID\tArrival time\n", "Name");
                printHeap (heap->heapAry[smallIndex]);
                heapDelete(heap, smallIndex);
                numPatients--;
            break;
            case 3:
            	if (numPatients==0)
            	{
            		puts("Error: there are no patients in the queue.");
            		break;
            	}
				printf("%20.20s\tPriority\tID\tArrival time\n", "Name");
            	printLow(heap, numPatients);
                break;
            case 4:
                puts ("Thank you! Goodbye.");
            break;
        }
    }  while (choice != 4);
}

/***** READS USER INPUT (DECISION) *****/
int getChoice (void){
	//local definitions
	int choice, test;
	bool valid;

	//statements
	printf ("\n========== MENU ===========\n");
	printf ("1\tEnter a new patient\n");
	printf ("2\tRetreive patient with highest critical status\n");
	printf ("3\tPrint a List of patients in queue (ordered by critical status)\n");
	printf ("4\tQuit\n");
	printf ("==========================\n");
	printf ("Please enter your choice:  ");

	do
	{
		test = scanf("%d", &choice);
		if (test == 1){	
			switch (choice)
			{
				case 1:
				case 2:
				case 3:
				case 4:
					valid = true;
				break;
				default:
					puts ("Error: enter a number between 1-4.");
					valid = false;
				break;
			}
		}
		else{
			printf("Error: non-numeric number read. \nRestrict input to 1-4.\n");
			valid = false;
			while ( (test = getchar()) != EOF && test != '\n' );
		}
	} while (!valid);

	return choice;
}

/***** CREATES A NEW PATIENT AND ASSIGNS THE DATA*****/
PATIENT * getPatient (void){

	//local definitions
	PATIENT *patient;
    bool valid;
    time_t rawtime;
    struct tm * timeinfo;

    //memory allocation
    patient = (PATIENT*) malloc (sizeof (PATIENT));
    patient->arrivalTime = malloc(256*sizeof(char));
    patient->name = malloc(256* sizeof(char));

    if (!patient)
    {
    	printf ("Error allocating memory in getPatient function.\n");
        exit (200);
    }

    //set arrival time
    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strcpy(patient->arrivalTime, asctime (timeinfo));

    // set Name and priority
    printf ("Enter the name of the patient: ");
    scanf("%c");
	fgets(patient->name, 100, stdin);			// this reads in the newline character at the end (enter button)

	patient->name[strlen(patient->name)-1]='\0';					// this deletes the new line character
    printf ("\nEnter patient priority: ");
    do
    {
    	scanf ("%d", &patient->priority);
    	switch (patient->priority)
    	{
    		case 1:
    		case 2:
    		case 3:
    		case 4:
    		case 5:
    			valid = true;
    		break;
    		default:
    			printf ("Error: restrict input to 1-5.\n");
    			valid = false;
    		break;
    	}
    } while (!valid);

    return patient;
}

/***** PRINTS THE HEAP *****/
void printHeap(void* heapArray){
    PATIENT p;
    p=*(PATIENT*)heapArray;
    printf("%20.20s\t%d\t\t%d\t%s",p.name, p.priority, p.id, p.arrivalTime);
}

/***** PULLS THE NEXT PATIENT, PRINTS THEM, THAN REHEAPS DOWN*****/
bool pullPatient (HEAP* heap, int *smallest, int index){

    //statements

    if (index > heap->last) //if we reach the last patient
        return true; //base statement to end recursion

    switch (compare(heap->heapAry[*smallest], heap->heapAry[index]))
    {
    	case 1: //patient is more critical
    		*smallest = index;
    	break;
    	case 3: //patient has same critical but arrived earlier
    		*smallest = index;
    	break;
    }
    pullPatient (heap, smallest, index+1); //pull the next patient and check
return true;
}

/***** PRINTS PATIENTS IF THEIR CRITICALITY MATCHES THE TARGET*****/
void getCriticality (void *patient, int target){
    //local definitions
    PATIENT p1;

    /***** STATEMENTS *****/
    //Set temporary PATIENT variables
    p1 = *(PATIENT *) patient;

    //prioritize the patients
    if (p1.priority == target)
        printHeap (patient);
}

/***** CALLS GET GETCRITICALITY FUNCTION TO PRINT PATIENTS*****/
void printLow(HEAP *heap, int numPatients)
{
	int n;

	for (n=0;n<numPatients;n++)
		getCriticality(heap->heapAry[n], 1);

	for (n=0;n<numPatients;n++)
		getCriticality(heap->heapAry[n], 2);

	for (n=0;n<numPatients;n++)
		getCriticality(heap->heapAry[n], 3);

	for (n=0;n<numPatients;n++)
		getCriticality(heap->heapAry[n], 4);

	for (n=0;n<numPatients;n++)
		getCriticality(heap->heapAry[n], 5);
}

