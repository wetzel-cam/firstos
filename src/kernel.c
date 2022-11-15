#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check for incorrect target */
#if defined(__linux__)
#error "Not using the cross compiler"
#endif

#if !defined(__i386__)
#error "Needs to be compiled with a ix86-elf compiler (i.e. you are using the wrong compiler)"
#endif

/* Hardware text mode color constants */
enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 3,
  VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
  return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str)
{
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize(void)
{
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t*) 0xB8000;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_setcolor(uint8_t color)
{
  terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  
  if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
        terminal_row = 0;
  }

  if (c == '\n') {
    terminal_putentryat(' ', terminal_color, --terminal_column, terminal_row); // Stops the newline char from being displayed
    terminal_column = 0;
    ++terminal_row;
  }
}

void terminal_write(const char* data, size_t size)
{
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
  terminal_write(data, strlen(data));
}

char* to_char(size_t var)
{
  static size_t val;

  // Sets val back to zero on the ASCII table, then uses var to get offset value
  val = 48 + var;

  return (char*)&val;
}

char* to_str(size_t var)
{
  static size_t str_var;

  str_var = var;

  return (char*)&str_var;
}

void kernel_main(void)
{
  /* Initialize terminal interface */
  terminal_initialize();

  terminal_writestring("Hellorld!\n");
  
/*
  char* letter = "a";
  terminal_write(letter, 1);
  
  char c = 99;
  terminal_write(&c, 1);

  size_t one = 49;
  terminal_write((char*)&one, 1);

  size_t two = 2;
  terminal_write(to_char(two), 1);
  
  for (size_t i = 0; i < 40; i++) {
    terminal_write(to_char(i), 1);
    terminal_write("\n", 1);
  }
 */ 
/*
  for (size_t i = 2; i < VGA_HEIGHT; i++) {
    terminal_write(to_int(i), strlen(to_int(i)));
    terminal_writestring("a line\n");
  }
  terminal_writestring("another one\n");
  */
}
