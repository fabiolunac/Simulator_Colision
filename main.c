#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/fpu.h"
#include "inc/hw_uart.h"
#include "driverlib/pwm.h"
#include "arm_math.h"


#define SIZE 100
#define NUM_TAPS 7
#define BLOCK_SIZE 10

uint32_t ui32SysClkFreq;

static float32_t iirStateF32[BLOCK_SIZE + NUM_TAPS - 1];

float32_t k[NUM_TAPS] =
{
    -0.9314,
    -0.5243,
    -0.5242,
    -0.4461,
    0.0837,
    -0.1534,
    0.0643
};

float32_t v[NUM_TAPS + 1] =
{
     0.0021,
    -0.0013,
     0.0437,
     0.0234,
    -0.1238,
    -0.1901,
    -0.1052,
    -0.0215

};

float32_t signal_input[SIZE];
float32_t signal_output[SIZE];

uint32_t numBlocks = SIZE/BLOCK_SIZE;

void IIR_filter_lattice(float32_t* input, float32_t* output, float32_t* k, float32_t* v, uint32_t blockSize, uint32_t num_taps, float32_t* firState, uint32_t num_blocks)
{
    static arm_iir_lattice_instance_f32 S;
    arm_iir_lattice_init_f32(&S, num_taps, (float32_t*)k, (float32_t*)v, firState, blockSize);

    uint32_t i;
    for(i = 0; i < num_blocks; i++)
    {
        arm_iir_lattice_f32(&S, input + (i * blockSize), output + (i * blockSize), blockSize);
    }
}

void generateSignal(void)
{
    signal_input[0] = 1;
}

int main(void)
{
    ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
            SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    FPUEnable();
    FPULazyStackingEnable();

    generateSignal();

    while(1)
    {
        IIR_filter_lattice(signal_input, signal_output, k, v, BLOCK_SIZE, NUM_TAPS, iirStateF32, numBlocks);
    }
}
