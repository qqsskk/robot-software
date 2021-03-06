#include <ch.h>
#include <hal.h>

/* We have an external attenuation of 0.5 before reaching the analog input */
static const float ADC_GAIN = 2 * 3.3 / 4096;

static const ADCConversionGroup group = {
    .circular = false,
    .num_channels = 2,
    .end_cb = NULL,
    .error_cb = NULL,
    .cfgr = 0,
    /* Sample every channel for 600 ADC clock cycles (SMP=5) .*/
    .smpr = {ADC_SMPR1_SMP_AN0(5) | ADC_SMPR1_SMP_AN1(5), 0},
    /* We want to sample 2 channels: Channel 0 and 1 */
    .sqr = {ADC_SQR1_NUM_CH(2) | ADC_SQR1_SQ1_N(1) | ADC_SQR1_SQ2_N(2),
            0, 0, 0},
};

void analog_start(void)
{
    adcStart(&ADCD1, NULL);
}

void analog_input_read(float voltages[2])
{
    adcsample_t samples[2 * 2];
    adcAcquireBus(&ADCD1);

    /* Blocking conversion */
    adcConvert(&ADCD1, &group, samples, 2);

    /* TODO: understand why this is needed and remove it */
    chThdSleepMilliseconds(10);
    adcReleaseBus(&ADCD1);

    for (int i = 0; i < 2; i++) {
        voltages[i] = samples[i] * ADC_GAIN;
    }
}
