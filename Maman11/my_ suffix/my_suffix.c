#include <stdio.h>
#include <string.h>
#define MAX_STRING 127
int suffix(char str[], char c);

int main()
{
    char str[MAX_STRING +1 ];
    char c;
    int index;
    int amount;
    int temp;
    puts("Hey!, please enter a char and a string\nYou can enter them together or seperated by enter");
    c = getchar();
    temp = getchar();
    while(temp == '\n')/*to cover the case when the user entererd the string and char seprated by enters*/  
    {
    	temp = getchar();
    }
    str[0] = temp;
    for (index = 1; index <= MAX_STRING + 1 ; index++)/*loop for scaning the string*/
    {

        str[index] = getchar();
        if(str[index] == '\n' || index == MAX_STRING +1)
            {
                str[index] = '\0';
                break;
            }
    }
    printf("You entered the char: \'%c\' and the string: %s.\nLets see what suffix we got:",c,str);
    amount = suffix(str,c);
    printf("\nAs u can se we got %d suffix\n",amount);
    return 0;
}	

int suffix(char str[], char c)
{
    int word_counter = 0;
    int index;
    int start_of_suffix;
    for(start_of_suffix =0;start_of_suffix < strlen(str); start_of_suffix++)/* loop for scaning the string*/
    {
        if(str[start_of_suffix] == c)/*for every time the char is in the string a innner loop will pring all the string from the char to the end*/
        {
            word_counter++;
            printf("\n");
            for (index = start_of_suffix; index < strlen(str);index++)
            {
                printf("%c",str[index]);
            }
        }

    }
    return word_counter;
}


