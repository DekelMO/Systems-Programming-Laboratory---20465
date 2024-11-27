#include <stdio.h>
#define  SIZE_OF_INT 8*sizeof(int)
unsigned int my_add(int a, int b);

int main()
{
    unsigned int a,b,sum;
    printf("Hello, please enter 2 numbers\n");
    scanf("%u%u",&a,&b);
    sum = my_add(a,b);
    printf("the inputs are: a = %u and b = %u\nThe sum is %d \n",a,b,sum);
    return 0;
}
unsigned int my_add(int a, int b)
{
    unsigned int size_of_input = 8*sizeof(int);/*max size of the input*/
    unsigned int carry = 0;
    unsigned int bit_sum = 0;
    unsigned int a_bit;
    unsigned int b_bit;
    unsigned int sum =0;
    unsigned int i;
    char binary_a[SIZE_OF_INT+1] ;/*strings for the binary representation*/ 
    char binary_b[SIZE_OF_INT+1] ;
    char binary_sum[SIZE_OF_INT+1] ;

    for(i=0; i< size_of_input;i++)/*loop to run on all the bits of the inputs*/
    {
        a_bit= a&1;/*getting the value of the first bit*/
        binary_a[size_of_input-i-1] = a_bit ? '1' : '0';/*insert the bit to the string start from the end and moving backward for the string will be readable from left to rite   */
        b_bit = b&1;
        binary_b[size_of_input-i-1] = b_bit ? '1' : '0';
        bit_sum = a_bit ^ b_bit ^carry;/* check the bit of the sum,result 1 if all of them are 1 or only one of them is 1*/
        binary_sum[size_of_input-i-1] = bit_sum?'1':'0';
        carry = (a_bit && b_bit)||(a_bit && carry) || (b_bit && carry);/*calculate if a bit need to be carry for the next bit calculation*/
        sum += bit_sum << i;/*after calculating a bit its value in decimal is calaculated according to its place and sums up to bits that already calculated*/
        a = a>>1;
        b = b>>1;
    }
    binary_a[size_of_input] = '\0';
    binary_b[size_of_input] = '\0';
    binary_sum[size_of_input] = '\0';
    printf("a in decimal:%s\n",binary_a);
    printf("b in decimal:%s\n",binary_b);
    printf("The sum of a + b = %s\n",binary_sum);
    return sum;
}



