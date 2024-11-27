#include "constants.h"
#include "lexer.h"

enum status {
    COMMA, NUMBER, DONE
};


/* checks the validation of first group operations */
int groupOneFirstArg(char *word, int operation);

int groupOneSecondArg(char *word, int operation);

int firstGroupOps(int operation, char *line);

/* covers cases of registers and labels */
int validRegister(char *line);

int validLabel(char *line);

int validRegisterAddress(char *line);

/* validate not,clr,inc,dec */
int secondGroupOps(char *line, int operation);


int thirdGroupOps(char *line);

/* function to validate the numbers recieved by .data instruction */
int validData(char *line);

/*funtion to check the string recieved by .string instruction */
int validString(char *string);

/* function to validate that the input recieved by .entry is a label */
int validEntryOrExtern(char *line);
