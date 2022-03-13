 /*
 * OfficeHours.c
 *
 *  Created on: Mar 1, 2021
 *  Author: Luis Oliveros
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

int numStudents; //Number of students that visit office hours
int officeCapacity; //Number of students allowed in office at once
int currentStudentId; //Keeps track of current student in office

//semaphores that control access to student and professor actions.
sem_t OfficeSpots;
sem_t StudentCanAskQuestion;
sem_t ProfessorCanSpeak;
sem_t StudentCanSpeak;

void *Professor();void *Student(int id);
void AnswerStart(int studentId);
void AnswerDone(int studentId);
void EnterOffice(int id);
void LeaveOffice(int id);
void QuestionStart(int id);
void QuestionDone(int id);

int main(int argc, char *argv[])
{
	numStudents = 3;   //atoi(argv[1]);
	officeCapacity = 2; //atoi(argv[2]);

	//Initializing semaphore variables.
   sem_init(&OfficeSpots, 0,  officeCapacity);
   sem_init(&StudentCanAskQuestion, 0, 1);
   sem_init(&ProfessorCanSpeak, 0, 0);
   sem_init(&StudentCanSpeak, 0, 0);

   pthread_t profThread;//creates Professor thread
   //Initializes Professor thread and runs Professor() function.
   pthread_create(&profThread, NULL, Professor, NULL);
   pthread_t studThreads[numStudents];//creates multiple Student threads

   int i;
   for (i = 0; i < numStudents; i++)
   {
	   //Initializes student thread and runs Student() function with i parameter.
       pthread_create(&studThreads[i], NULL, (void *)Student, (void *)i);
   }
   for (i = 0; i < numStudents; i++)
   {
	   //Waits until specific Student thread is done before exiting
	   pthread_join(studThreads[i], NULL);
   }

   printf("Office hours are now over. End of run.\n");
   return 0;
}

/*
 * The Professor function loops a professor thread through the AnswerStart()
 * and AnswerDone() functions. It waits for feedback on the specified semaphores
 * to proceed.
 */
void *Professor()
{
   while(1)
   {
       sem_post(&StudentCanSpeak); //Lets current student speak
       sem_wait(&ProfessorCanSpeak); //Waits till professor can speak
       AnswerStart(currentStudentId);
       AnswerDone(currentStudentId);
       sem_post(&StudentCanSpeak); //lets current student speak
   }
}

/*
 * The Student function loops multiple student threads for each of the specific
 * amount of student thread questions. Calls the EnterOffice(), QuestionDone(),
 * QuestionStart(), and LeaveOffice() functions. Waits on specified semaphore
 * signals to continue.
 */
void *Student(int id)
{
   int studentId = id;
   int numQs = (studentId % 4) + 1;
   int rand = (random() % 1000) + 1;
   int i;

   sem_wait(&OfficeSpots); //Student waits till an office spot opens
   EnterOffice(studentId);
   usleep(rand); //Allow for randomness of events in thread access

   for(i = 0; i < numQs; i++)
   {
       sem_wait(&StudentCanAskQuestion); //Waits for a specific student
       currentStudentId = studentId; //Updates current student
       sem_wait(&StudentCanSpeak);//Waits to let student speak
       QuestionStart(studentId);
       sem_post(&ProfessorCanSpeak); //Lets professor speak
       sem_wait(&StudentCanSpeak); //Waits till student can speak
       QuestionDone(studentId);
       usleep(rand);//Allow for randomness of events in thread access.
       sem_post(&StudentCanAskQuestion); //Lets another student speak
   }

   sem_post(&OfficeSpots); //Lets another student in the office
   LeaveOffice(studentId);
}

//All functions below print with a unique id number that represents
//the student thread accessing it currently.
void AnswerStart(int id)
{
   printf("Professor starts to answer question for student %d.\n", id);
}
void AnswerDone(int id)
{
   printf("Professor is done with answer for student %d.\n", id);
}
void EnterOffice(int id)
{
   printf("Student %d enters the office.\n", id);
}
void LeaveOffice(int id)
{
   printf("Student %d leaves the office.\n", id);
}
void QuestionStart(int id)
{
   printf("Student %d asks a question.\n", id);
}
void QuestionDone(int id)
{
   printf("Student %d is satisfied.\n", id);
}



