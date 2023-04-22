
#include <command.h>
#include "console.h"

extern int serial_getc (void);
extern void serial_putc (const char c);
extern int serial_tstc(void);

/** U-Boot INITIAL CONSOLE-COMPATIBLE FUNCTION *****************************/

int getc(void)
{
	/* Send directly to the handler */
	return serial_getc();
}

int tstc(void)
{
	/* Send directly to the handler */
	return serial_tstc();
}

void putc(const char c)
{
/* Send directly to the handler */
    serial_putc(c);
}


/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;	/* see disable_ctrl() */
static int ctrlc_was_pressed = 0;
int ctrlc(void)
{
	if (!ctrlc_disabled ) {
		if (tstc()) {
			switch (getc()) {
			case 0x03:		/* ^C - Control C */
				ctrlc_was_pressed = 1;
				return 1;
			default:
				break;
			}
		}
	}
	return 0;
}

/* pass 1 to disable ctrlc() checking, 0 to enable.
 * returns previous state
 */
int disable_ctrlc(int disable)
{
	int prev = ctrlc_disabled;	/* save previous state */

	ctrlc_disabled = disable;
	return prev;
}

int had_ctrlc (void)
{
	return ctrlc_was_pressed;
}

void clear_ctrlc(void)
{
	ctrlc_was_pressed = 0;
}


