/* Kannika's Batch OS Model
I designed this Batch OS Model to demonstrate how Batch Operating System functions.
Currently, this model supports running batches of Python, C, and, Java program files.
However, this model can be easily modified to support the execution of other program files.
This model is designed to run in a Linux hosted environment. 

Batch OS executes jobs in batches, so this model reflects the design by grouping files 
of the same programming language, and executing the collected files all at once.  
ie: All Python files will be collected and executed together, then all C files 
    will be collected and executed together and so on.

Batch OS does not support interactive programs that require user's input during execution.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int runPythonBatch();
int runCBatch();
int runJavaBatch();

int main () {

	int option = 0;
	int jobnum = 0;	//Keep Track of the number of jobs executed

	printf("\n-------------------Welcome to Kannika's Batch OS Model!-------------------\n");
	printf("This model simulates the functionality of a Batch Operating System.\n");
	printf("Please place your Python, C, and Java program files in the Jobs directory.\n");

	while(1){
		printf("\n----------Option Menu----------\n");
		printf("[1]: Print all jobs \n");
		printf("[2]: Execute all Python jobs \n");
		printf("[3]: Execute all C jobs \n");
		printf("[4]: Execute all Java jobs \n");
		printf("[5]: Execute all jobs \n");
		printf("[0]: Exit \n"); 

		printf("Please select an option to proceed: "); 
		
		scanf("%d", &option);

		if(option == 1){
			system("ls -lrt ./Jobs/");

		}else if(option == 2){
			jobnum = runPythonBatch(jobnum);

		}else if(option == 3){
			jobnum = runCBatch(jobnum);

		}else if(option == 4){
			jobnum = runJavaBatch(jobnum);
			
		}else if(option == 5){
			jobnum = runPythonBatch(jobnum);
			jobnum = runCBatch(jobnum);
			jobnum = runJavaBatch(jobnum);

		}else{
			if(option == 0){
				printf("\n %d Total jobs were executed. \n", jobnum);
				printf("\nExiting Kannika's Batch OS Model...\n");
				sleep(2);
				printf("        *********Thank you!*********             \n\n");
				break;
			}else{
				printf("\n----Please select a valid option!----\n\n");
			}
		}
		
	}
	return(0);
}

int runPythonBatch(int jobnum){
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char pycommand[100] = "";

	printf("\nRunning Python Batch... \n");

	// Collect Python program files
	system("ls ./Jobs/*.py > pyfiles.txt");
	FILE *pyfile = fopen("pyfiles.txt", "r");

	// Loop through each list Python program
	while((read = getline(&line, &len, pyfile)) != -1){
		jobnum++;

		// Prepare Python Command
		printf("Executing Job %d: %s \n", jobnum, line);	
		strcpy(pycommand, "python3 ");
		strcat(pycommand, line);

		// Execute Python Command
		system(pycommand);
		printf("\n\nJob %d completed execution\n", jobnum);		
	}

	// Cleanup after execution
	fclose(pyfile);
	system("rm pyfiles.txt");
	printf("Python Batch Complete\n\n");

	return jobnum;

}

int runCBatch(int jobnum){
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char ccommand[100] = "";

	// Collect C program files
	printf("\nRunning C Batch... \n");
	system("ls ./Jobs/*.c > cfiles.txt");
	FILE *cfile = fopen("cfiles.txt", "r");

	// Loop through each list C program
	while((read = getline(&line, &len, cfile)) != -1){
		jobnum++;
		
		// Prepare C Command
		printf("Executing Job %d: %s \n", jobnum, line);
		strcpy(ccommand, "gcc -o cprogramexecutable ");
		strcat(ccommand, line);

		// Execute C Command
		system(ccommand);
		system("./cprogramexecutable");
		printf("\n\nJob %d completed execution\n", jobnum);		
	}

	// Cleanup after execution
	fclose(cfile);
	system("rm cfiles.txt");
	system("rm cprogramexecutable");
	printf("C Batch Complete\n\n");

	return jobnum;
}

int runJavaBatch(int jobnum){
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char jvcommand1[100] = "";
	char jvcommand2[100] = "";
	char jvname[100] = "";

	// Collect Java program files
	printf("\nRunning Java Batch... \n");
	system("cd ./Jobs/; ls *.java > ../jvfiles.txt");
	FILE *jvfile = fopen("jvfiles.txt", "r");

	// Loop through each list Java program
	while((read = getline(&line, &len, jvfile)) != -1){
		jobnum++;

		// Prepare Java Command
		printf("Executing Job %d: %s \n", jobnum, line);
		strcpy(jvcommand1, "cd ./Jobs/; javac ");
		strcat(jvcommand1, line);
		strcpy(jvcommand2, "cd ./Jobs/; java ");
		memcpy(jvname, line, strlen(line)-6);
		strcat(jvcommand2, jvname);

		// Execute Java Command
		system(jvcommand1);
		system(jvcommand2);
		printf("\n\nJob %d completed execution\n", jobnum);		
	}

	// Cleanup after execution
	fclose(jvfile);
	system("rm jvfiles.txt");
	printf("Java Batch Complete\n\n");
	
	return jobnum;
}
	
	
