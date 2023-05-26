
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

/* Set STM32 to 168 MHz. */

static void clock_setup (void)
{
rcc_clock_setup_pll (&rcc_hse_8mhz_3v3 [RCC_CLOCK_3V3_168MHZ]) ;
rcc_periph_clock_enable (RCC_GPIOG) ;
/* Enable GPIOG clock. */
}

//buscar puertos a utilizar para la pantalla y el giroscopio

static void gpio_setup (void)
{
/* Set GPIO13-14 (in GPIO port G) to ’output push-pull’. */
gpio_mode_setup (GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13|GPIO14) ;
}

static void spi_setup (void)
{
/* Set GPIO13-14 (in GPIO port G) to ’output push-pull’. */
gpio_mode_setup (GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13|GPIO14) ;
}

int main (void)
{
int i;
clock_setup();
gpio_setup();
gpio_set(GPIOG, GPIO13);

/* Set two LEDs for wigwag effect when toggling. */
/* Blink the LEDs (PG13 and PG14) on the board. */

while (1) {

/* Toggle LEDs. */
gpio_toggle (GPIOG, GPIO13|GPIO14);

}
return 0;
}