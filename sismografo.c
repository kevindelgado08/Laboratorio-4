
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>


/* Set STM32 to 168 MHz. */

static void clock_setup (void)
{
rcc_clock_setup_pll (&rcc_hse_8mhz_3v3 [RCC_CLOCK_3V3_168MHZ]);
rcc_periph_clock_enable(RCC_SPI5);
//Habilitar clock de los puertos para el L3GD20: 

rcc_periph_clock_enable (RCC_GPIOF | RCC_GPIOC | RCC_GPIOA);

//Habilitar clock de los puertos para el LCD SPI: 

rcc_periph_clock_enable (RCC_GPIOD | RCC_GPIOC);

//Habilitar clock de los puertos para el USB: 

rcc_periph_clock_enable (RCC_GPIOB | RCC_GPIOC);

//Habilitar clock de los puertos para el LED: 

rcc_periph_clock_enable (RCC_GPIOB | RCC_GPIOC);

//Habilitar clock de los puertos para el USB: 

rcc_periph_clock_enable (RCC_GPIOA | RCC_GPIOC);

//Habilitar clock de los puertos para el USART: 

rcc_periph_clock_enable(RCC_USART1);
}

static void gyro_setup (void)
{

// GPIO'S Gyroscopio

gpio_mode_setup (GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1); // Pin para CS
gpio_mode_setup (GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO8); // Pin de MISO
//gpio_mode_setup (GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO1|GPIO2); Pines para interrupciones
gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7 | GPIO9); // PIN SCK y MOSI
gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO9);

//Configuración SPI para giroscopio:

spi_set_master_mode(SPI5);
spi_set_baudrate_prescaler(SPI5, SPI_CR1_BR_FPCLK_DIV_64);
spi_set_clock_polarity_0(SPI5);
spi_set_clock_phase_0(SPI5);
spi_set_full_duplex_mode(SPI5);
spi_set_unidirectional_mode(SPI5); 
//spi_set_data_size(SPI5, SPI_CR1_DFF_8BIT);
spi_enable_software_slave_management(SPI5);
spi_send_msb_first(SPI5);
spi_set_nss_high(SPI5);
spi_enable(SPI5);
}

static void LCD_setup (void)
{

// GPIO'S LCD
gpio_mode_setup (GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2); // Pin para CS
gpio_mode_setup (GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13); // Pin de MISO
gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7 | GPIO9); // PIN SCL y SDI/SDO(MOSI)
gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO9); //preguntar si esto va acá o en el main 

//Configuración SPI para LCD:

spi_init_master(SPI5, SPI_CR1_BAUDRATE_FPCLK_DIV_64,
				SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
				SPI_CR1_CPHA_CLK_TRANSITION_1,
				SPI_CR1_DFF_8BIT,
				SPI_CR1_MSBFIRST);
spi_enable_ss_output(SPI5);
spi_enable(SPI5);
}

static void usart_setup (void)
{

//GPIO'S USART

/* Setup GPIO pins for USART1 transmit. */
gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

/* Setup USART1 TX pin as alternate function. */
gpio_set_af(GPIOA, GPIO_AF7, GPIO9);

//Configuración de USART:

	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_enable(USART1);
}


static void LED_setup (void)
{
//GPIO'S LED

//gpio_mode_setup (GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
//gpio_mode_setup (GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
gpio_mode_setup (GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13 | GPIO14);
gpio_set(GPIOG, GPIO13);
//Configuración de LED:

}

static void button_setup (void)
{

//GPIO'S Pushbuttons

gpio_mode_setup (GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

//Configuración de Pushbuttons:

}

int main (void)
{
clock_setup();

gyro_setup();
button_setup();
LED_setup();
usart_setup();
LCD_setup();

while (1) {

;
}
return 0;
}
