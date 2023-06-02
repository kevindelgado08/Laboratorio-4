
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include "gfx.c"
#include "lcd-spi.c"
#include "sdram.c"
#include "clock.h"
#include <stdint.h>

#define GYR_RNW			(1 << 7) /* Write when zero */
#define GYR_MNS			(1 << 6) /* Multiple reads when 1 */
#define GYR_WHO_AM_I		0x0F
#define GYR_OUT_TEMP		0x26
#define GYR_STATUS_REG		0x27
#define GYR_CTRL_REG1		0x20
#define GYR_CTRL_REG1_PD	(1 << 3)
#define GYR_CTRL_REG1_XEN	(1 << 1)
#define GYR_CTRL_REG1_YEN	(1 << 0)
#define GYR_CTRL_REG1_ZEN	(1 << 2)
#define GYR_CTRL_REG1_BW_SHIFT	4
#define GYR_CTRL_REG4		0x23
#define GYR_CTRL_REG4_FS_SHIFT	4
#define GYR_OUT_X_L		0x28
#define GYR_OUT_X_H		0x29
#define GYR_OUT_Y_L		0x2A
#define GYR_OUT_Y_H		0x2B
#define GYR_OUT_Z_L		0x2C
#define GYR_OUT_Z_H		0x2D
#define CONSOLE_UART	USART1

static void clock_setup_enable (void)
{
rcc_clock_setup_pll (&rcc_hse_8mhz_3v3 [RCC_CLOCK_3V3_84MHZ]);
rcc_periph_clock_enable(RCC_SPI5);
//Habilitar clock de los puertos para el L3GD20: 

rcc_periph_clock_enable (RCC_GPIOF);
rcc_periph_clock_enable (RCC_GPIOA);
rcc_periph_clock_enable (RCC_GPIOC);
//Habilitar clock de los puertos para el LCD SPI: 

rcc_periph_clock_enable (RCC_GPIOD);
rcc_periph_clock_enable (RCC_GPIOC);
//Habilitar clock de los puertos para el USB: 

rcc_periph_clock_enable (RCC_GPIOB);
rcc_periph_clock_enable (RCC_GPIOC);

//Habilitar clock de los puertos para el LED: 

rcc_periph_clock_enable (RCC_GPIOB);
rcc_periph_clock_enable (RCC_GPIOC);
//Habilitar clock de los puertos para el USB: 

rcc_periph_clock_enable (RCC_GPIOC);
rcc_periph_clock_enable (RCC_GPIOA);

//Habilitar clock de los puertos para el USART: 

rcc_periph_clock_enable(RCC_GPIOA);
rcc_periph_clock_enable(RCC_USART1);
}

static void gyro_setup (void)
{


// GPIO'S Gyroscopio

gpio_mode_setup (GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1); // Pin para CS
//gpio_mode_setup (GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, ); // Pin de MISO
//gpio_mode_setup (GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO1|GPIO2); Pines para interrupciones
gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7 | GPIO8| GPIO9); // PIN SCK y MOSI
gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO8 | GPIO9);
gpio_set_output_options(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO7 | GPIO9);
//Configuración SPI para giroscopio:

spi_set_master_mode(SPI5);
spi_set_baudrate_prescaler(SPI5, SPI_CR1_BR_FPCLK_DIV_256);
spi_set_clock_polarity_0(SPI5);
spi_set_clock_phase_0(SPI5);
spi_set_full_duplex_mode(SPI5);
spi_set_unidirectional_mode(SPI5); 
spi_set_dff_8bit(SPI5);
spi_enable_software_slave_management(SPI5);
spi_enable_ss_output(SPI1);
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


rcc_periph_clock_enable(RCC_GPIOA);
rcc_periph_clock_enable(RCC_USART1);
//GPIO'S USART

/* Setup GPIO pins for USART1 transmit. */
gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);

/* Setup USART1 TX pin as alternate function. */
gpio_set_af(GPIOA, GPIO_AF7, GPIO9 |GPIO10);

//Configuración de USART:

	usart_set_baudrate(CONSOLE_UART, 115200);
	usart_set_databits(CONSOLE_UART, 8);
	usart_set_stopbits(CONSOLE_UART, USART_STOPBITS_1);
	usart_set_mode(CONSOLE_UART, USART_MODE_TX_RX);
	usart_set_parity(CONSOLE_UART, USART_PARITY_NONE);
	usart_set_flow_control(CONSOLE_UART, USART_FLOWCONTROL_NONE);
	usart_enable(CONSOLE_UART);
}
static void my_usart_print_int(uint32_t usart, int32_t value)
{
	int8_t i;
	int8_t nr_digits = 0;
	char buffer[25];

	if (value < 0) {
		usart_send_blocking(usart, '-');
		value = value * -1;
	}

	if (value == 0) {
		usart_send_blocking(usart, '0');
	}

	while (value > 0) {
		buffer[nr_digits++] = "0123456789"[value % 10];
		value /= 10;
	}

	for (i = nr_digits-1; i >= 0; i--) {
		usart_send_blocking(usart, buffer[i]);
	}

	usart_send_blocking(usart, '\r');
	usart_send_blocking(usart, '\n');
}

void console_putc(char c)
{
	uint32_t	reg;
	do {
		reg = USART_SR(CONSOLE_UART);
	} while ((reg & USART_SR_TXE) == 0);
	USART_DR(CONSOLE_UART) = (uint16_t) c & 0xff;
}

char console_getc(int wait)
{
	uint32_t	reg;
	do {
		reg = USART_SR(CONSOLE_UART);
	} while ((wait != 0) && ((reg & USART_SR_RXNE) == 0));
	return (reg & USART_SR_RXNE) ? USART_DR(CONSOLE_UART) : '\000';
}

void console_puts(char *s)
{
	while (*s != '\000') {
		console_putc(*s);
		/* Add in a carraige return, after sending line feed */
		if (*s == '\n') {
			console_putc('\r');
		}
		s++;
	}
}

int console_gets(char *s, int len)
{
	char *t = s;
	char c;

	*t = '\000';
	/* read until a <CR> is received */
	while ((c = console_getc(1)) != '\r') {
		if ((c == '\010') || (c == '\127')) {
			if (t > s) {
				/* send ^H ^H to erase previous character */
				console_puts("\010 \010");
				t--;
			}
		} else {
			*t = c;
			console_putc(c);
			if ((t - s) < len) {
				t++;
			}
		}
		/* update end of string with NUL */
		*t = '\000';
	}
	return t - s;
}

static void LED_setup (void)
{
//GPIO'S LED
gpio_mode_setup (GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13 | GPIO14);

}

static void button_setup (void)
{
//GPIO'S Pushbuttons
gpio_mode_setup (GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
}

int main (void)
{
int8_t gyr_x, gyr_y, gyr_z;

clock_setup();
sdram_init();
clock_setup_enable();
usart_setup();
gyro_setup();
button_setup();
LED_setup();
lcd_spi_init();
LCD_setup();

//Configuración inicial del giroscopio: 
gpio_clear(GPIOC, GPIO1);
spi_send(SPI5, GYR_CTRL_REG1);
spi_read(SPI5);
spi_send(SPI5, GYR_CTRL_REG1_PD | GYR_CTRL_REG1_XEN |
		GYR_CTRL_REG1_YEN | GYR_CTRL_REG1_ZEN |
		(3 << GYR_CTRL_REG1_BW_SHIFT));
spi_read(SPI5);
gpio_set(GPIOC, GPIO1);
gpio_clear(GPIOC, GPIO1);
spi_send(SPI5, GYR_CTRL_REG4);
spi_read(SPI5);
spi_send(SPI5, (1 << GYR_CTRL_REG4_FS_SHIFT));
spi_read(SPI5);
gpio_set(GPIOC, GPIO1);

console_puts(" \n");
console_puts("Dispositivo habilitado: \n");

//Configuración inicial de la pantalla:
gfx_init(lcd_draw_pixel, 240, 320);
gfx_fillScreen(LCD_GREY);
gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
gfx_setTextSize(2);
gfx_setCursor(15, 25);
gfx_puts("Sismografo");
gfx_setTextSize(0.9);
gfx_setCursor(15, 60);
gfx_puts("Ejes x, y, z:");
gfx_setCursor(15, 80);
console_puts("Coordenadas: ");
lcd_show_frame();
	
while (1) {

	//Configuración de Pantalla Principal: 

	gfx_fillScreen(LCD_GREY);
	gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
	gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
	gfx_setCursor(145, 18.5);
	gfx_puts("100 %");
	gfx_fillRoundRect(190, 15, 30, 15, 4, LCD_GREEN); //Bateria
	//falta un if para mostrar cuando se descarga la bateria
	gfx_drawRoundRect(190, 15, 30, 15, 4, LCD_BLACK);
	gfx_drawRect(220, 18.9, 2, 7, LCD_BLACK);
	gfx_setTextSize(2);
	gfx_setCursor(15, 40);
	gfx_puts("Coordenadas:");
	gfx_setTextSize(0.9);
		
// Configuración del giroscopio: 
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_WHO_AM_I);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_STATUS_REG | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gpio_set(GPIOC, GPIO1);

	//Eje x:
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_X_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gyr_x=spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_X_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gyr_x|= spi_read(SPI5) << 8;
	gfx_setTextSize(1);
	gfx_setCursor(15, 80);
	gfx_puts("Eje x: ");
	gfx_puts(gyr_x);
	gpio_set(GPIOC, GPIO1);

	//Eje y:
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Y_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gyr_y=spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Y_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gyr_y|= spi_read(SPI5) << 8;
	gfx_setCursor(15, 100);
	gfx_puts("Eje y: ");
	gfx_puts(gyr_y);
	gpio_set(GPIOC, GPIO1);
	
	//Eje z:
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Z_L | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gyr_z=spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);

	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, GYR_OUT_Z_H | GYR_RNW);
	spi_read(SPI5);
	spi_send(SPI5, 0);
	gyr_z|= spi_read(SPI5) << 8;
	gfx_setCursor(15, 120);
	gfx_puts("Eje z: ");
	gfx_puts(gyr_z);
	gfx_setTextSize(1.5);
	gfx_setCursor(15, 140);
	gfx_puts("Comunicacion: ");

	//Funcionamiento de botones y habilitación de USART: 
	if (gpio_get(GPIOA, GPIO0)){
		gfx_puts("ON");
		gpio_set(GPIOG, GPIO13);
		gpio_clear(GPIOG, GPIO14);
		console_puts(" \n");
		console_puts("Enviando información: \n");
		console_puts("Eje x: ");
		my_usart_print_int(CONSOLE_UART, gyr_x);
		console_puts("Eje y: ");
		my_usart_print_int(CONSOLE_UART, gyr_y);
		console_puts("Eje z: ");
		my_usart_print_int(CONSOLE_UART, gyr_z);
		console_puts(" ");
	}	
	else 
	{	
		gfx_puts("OFF");
		gpio_set(GPIOG, GPIO14);
		gpio_clear(GPIOG, GPIO13);
	}
	gpio_set(GPIOC, GPIO1);
	
	msleep(1000);
	lcd_show_frame();
}

return 0;
}
