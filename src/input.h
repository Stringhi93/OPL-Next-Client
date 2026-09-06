#ifndef INPUT_H
#define INPUT_H

void input_init(void);
void input_update(void);
void input_shutdown(void);

int input_pressed(int button);

#define INPUT_UP       0
#define INPUT_DOWN     1
#define INPUT_LEFT     2
#define INPUT_RIGHT    3
#define INPUT_CROSS    4
#define INPUT_CIRCLE   5
#define INPUT_START    6
#define INPUT_SELECT   7

#endif
