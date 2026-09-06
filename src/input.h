#ifndef INPUT_H
#define INPUT_H

#include <tamtypes.h>

typedef struct
{
    int up;
    int down;
    int left;
    int right;

    int cross;
    int circle;
    int square;
    int triangle;

    int start;
    int select;

    int l1;
    int r1;
    int l2;
    int r2;

} InputState;

int input_init(void);
void input_update(void);
void input_shutdown(void);

const InputState *input_get(void);

#endif
