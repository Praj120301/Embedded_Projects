
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include <math.h>
#include<stdio.h>


// fft Definitions
#define SAMPLING_FREQUENCY 8000     // Sampling frequency in Hz
#define SIGNAL_LENGTH 1024       // Length of the input signal
#define BITS 10 //2^10=1024




float signal_real[SIGNAL_LENGTH];                 // Input signal
int  signal_FixedP_real[SIGNAL_LENGTH];            // Fixed-point representation of input signal
int  signal_FixedP_imag[SIGNAL_LENGTH];
int magnitude_int[SIGNAL_LENGTH];


long bit_rev_real[SIGNAL_LENGTH];
long bit_rev_imag[SIGNAL_LENGTH];

float magnitude[SIGNAL_LENGTH/2];
int magnitude_fixed[SIGNAL_LENGTH/2];
long x;
long y;
// Function to generate an input signal (two sine waves combined)
void generateSignal(float *signal, int length)
{
    float t;
    int i;
    for (i = 0; i < length; i++) {
        t = (float)i / SAMPLING_FREQUENCY;
        signal_real[i] = (sinf(2 * M_PI * 200 * t) + sinf(2 * M_PI * 600 * t)) * 0.5;
        signal_FixedP_real[i] = (int)(signal_real[i] * 32768);
        signal_FixedP_imag[i]=0;
    }

}
int bit_reverse(int val,int bits)
{
    int rev,i;
    rev=0;
    for(i=0;i<bits;i++)
    {
        rev=rev<<1;//left shift first
        rev=(rev)|(val&1);//set or not set last bit of rev
        val=val>>1;//right shift;
    }
    return rev;
}
void bit_reversal_input()
{
    int i;int checkbitreversal=0;
    for( i=0;i<1024;i++)
    {
        checkbitreversal=bit_reverse(i,10);
       // printf("%d ",checkbitreversal);

        bit_rev_real[i]=(int)signal_FixedP_real[checkbitreversal];
        bit_rev_imag[i]=signal_FixedP_imag[checkbitreversal];

    }
}
void complex_mul(long ar,long ai,long br,long bi,long *res_r,long *res_i)
{

       long real_temp1 = ar * br;  // Q30
       long real_temp2 = ai * bi;  // Q30
       long imag_temp1 = ar * bi;  // Q30
       long imag_temp2 = ai * br;  // Q30


       long real_out = real_temp1 - real_temp2; // Q30
       long imag_out = imag_temp1 + imag_temp2; // Q30


       real_out = (real_out + (1 << 14)) >> 15; // Q15
       imag_out = (imag_out + (1 << 14)) >> 15; // Q15


       if (real_out > 32767) real_out = 32767;
       if (real_out < -32768) real_out = -32768;
       if (imag_out > 32767) imag_out = 32767;
       if (imag_out < -32768) imag_out = -32768;


       *res_r = real_out;
       *res_i = imag_out;

}
typedef struct {
                    long re;
                    long im;
                }twiddle;
                twiddle initial_twiddle,ongoing_twiddle;
                long even_real,odd_real;
                long even_imag,odd_imag;

void fixed_point_fft()
{
    int stages=BITS;//no of stages
    int i,k,j,m;//iterates over stages
    int groups;//no of items in each group
    int group_half;
    float theta;

    for( i=1;i<=stages;i++)
    {
       groups=1<<i;//2^stage so for iter 1 groups=2,iter groups=4
       group_half=groups/2;//mentions first half of group and second half of group
        printf("\n Stage %d no of items per group %d no of iters in one half of a group %d",i,groups,group_half);//works

       theta=2*M_PI/groups;//2pi/N where N varies based on group 2,4,8...

       initial_twiddle.re=(long)(cosf(theta)*32768);
       initial_twiddle.im=(long)((-1*sinf(theta))*32768);
       for(k=0;k<SIGNAL_LENGTH;k=k+groups)//iterates over groups
       {
           ongoing_twiddle.re=32767;
           ongoing_twiddle.im=00000;

               for(j=0;j<group_half;j++)//ultiplies butterflies in group
               {
                   complex_mul(ongoing_twiddle.re,ongoing_twiddle.im,bit_rev_real[k+j+group_half],bit_rev_imag[k+j+group_half],&odd_real,&odd_imag);
                   even_real=bit_rev_real[k+j];
                   even_imag=bit_rev_imag[k+j];

                   bit_rev_real[k + j]              = even_real+odd_real;
                   bit_rev_imag[k + j]              = even_imag+odd_imag;
                   bit_rev_real[k + j + group_half] = even_real-odd_real;
                   bit_rev_imag[k + j + group_half] = even_imag-odd_imag;
                   long temp_re, temp_im;
                   complex_mul(ongoing_twiddle.re, ongoing_twiddle.im,
                                       initial_twiddle.re, initial_twiddle.im,
                                       &temp_re, &temp_im);
                           ongoing_twiddle.re = temp_re;
                           ongoing_twiddle.im = temp_im;//basically ongoing twiddle increases per group so initialy it is W=2pi/N then W=2pi/N square and then cube
           }
       }

        for ( m = 0; m < SIGNAL_LENGTH; m++) {
            bit_rev_real[m] >>= 1;
            bit_rev_imag[m] >>= 1;
    }//divide everything by 2 to avoid overflow adds
    }
//    printf("\n %d %d ",initial_twiddle.re,initial_twiddle.im);
}
void test_complex_mul() {
    long a_re = 32767;
    long a_im = 0;
    long b_re = 32767;
    long b_im = 0;

    long result_re = 0, result_im = 0;

    complex_mul(a_re, a_im, b_re, b_im, &result_re, &result_im);


}
\
// Main
void main(void)
{
    // Initialize device clock and peripherals
    Device_init();

    // Generate the input signal
    generateSignal(signal_real, SIGNAL_LENGTH);
//
    bit_reversal_input();
//
    fixed_point_fft();

    int i;
    for ( i = 0; i < SIGNAL_LENGTH/2 ; i++) {
  //        float real_f = (float) / 32768.0f;
  //        float imag_f = (float)bit_rev_imag[i] / 32768.0f;

          magnitude[i] = sqrtf((bit_rev_real[i] * bit_rev_real[i])+( bit_rev_imag[i] * bit_rev_imag[i]));
          magnitude_fixed[i]=(int)(magnitude[i]*1);
      }


}



