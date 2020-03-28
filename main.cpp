#include <mbed.h>
#include <iostream>
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

Serial pc( USBTX, USBRX );

AnalogIn Ain(A0);
DigitalOut Redled(LED_RED);
DigitalOut Greenled(LED_GREEN);
AnalogOut Aout(DAC0_OUT);
DigitalIn SW(SW3);


int sample = 500;

float ADCdata[500] = {};

void display_on_seven(int frequency);

float period = 1/(float)sample;

int main()
{
    int true_f = 0;
    double time;
    float V_last = 1;     // record last voltage information
    int frequency = 0;
    clock_t t1;
    clock_t t2;
    bool flag = true;
    
    t1 = clock();

    Greenled = 1;
    Redled = 1;
    int data = 0;
    while (time < 3)
    {
        float A_record = Ain;
        
        if (data < sample)
            ADCdata[data++] = A_record * 3.3;

        if (V_last < 1 && V_last < A_record * 3.3 && A_record * 3.3 >= 1)        // a cycle passes
        {
            frequency++;
        }    

        V_last = A_record * 3.3;

        t2 = clock();
        time = (double)(t2 - t1) / CLOCKS_PER_SEC;


        
        wait(period);
    } 
    Greenled = 1;
    Redled = 0;
    wait(1);
    for (int i = 0; i < sample; i++)        // send data to pc
        pc.printf("%1.3f\r\n", ADCdata[i]);

    true_f = (double)frequency / 3;         // analyze completed
    Greenled = 0;
    Redled = 1;

    double n = 0;
    while (true)
    {
        Aout = (sin(M_PI*n) * 0.5) + 0.5;     // sample signal
        n += period;
        if (n >= sample)    n = 0;
        if (SW == 0)        // switch on
        {

            Greenled = 1;
            Redled = 0;
            display_on_seven(true_f);
        }
        else                // switch off
        {
            Greenled = 0;
            Redled = 1;
        }
        wait(period);
    }
}

void display_on_seven(int frequency)
{
    for (int i = 100; i >= 1; i /= 10)
    {
        int temp = (frequency / i) % 10;
        if (i != 1)
            display = table[temp];
        else display = table[temp] - 0x80;
        wait(0.5);
    }
    wait(0.5);
    display = 0x00;

    return;
}