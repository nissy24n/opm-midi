#include <stdlib.h>
#include <basic.h>
#include <iocslib.h>

int a, t, o, vvc, i, j;
unsigned int b, ss, vv[2] = {0, 0};
unsigned int ch[8] = {0, 0, 0, 0, 0, 0, 0, 0};

unsigned int init[25][2] = {
    {0x20,   0b11111010}, /* PAN/FL/CON */
    {0x40, ( 0<<4)+( 6)}, /* DT1/MUL */
    {0x48, ( 0<<4)+( 7)}, /* DT1/MUL */
    {0x50, ( 0<<4)+( 3)}, /* DT1/MUL */
    {0x58, ( 0<<4)+( 1)}, /* DT1/MUL */
    {0x60,           51}, /* TL      */
    {0x68,           58}, /* TL      */
    {0x70,           47}, /* TL      */
    {0x78,            2}, /* TL      */
    {0x80, ( 2<<6)+(31)}, /* KS/AR   */
    {0x88, ( 3<<6)+(25)}, /* KS/AR   */
    {0x90, ( 2<<6)+(30)}, /* KS/AR   */
    {0x98, ( 1<<6)+(24)}, /* KS/AR   */
    {0xa0, ( 0<<7)+( 2)}, /* AME/D1R */
    {0xa8, ( 0<<7)+( 9)}, /* AME/D1R */
    {0xb0, ( 0<<7)+( 2)}, /* AME/D1R */
    {0xb8, ( 0<<7)+( 7)}, /* AME/D1R */
    {0xc0, ( 0<<6)+( 8)}, /* DT2/D2R */
    {0xc8, ( 0<<6)+( 9)}, /* DT2/D2R */
    {0xd0, ( 0<<6)+( 1)}, /* DT2/D2R */
    {0xd8, ( 0<<6)+( 6)}, /* DT2/D2R */
    {0xe0, ( 6<<4)+( 8)}, /* D1L/RR  */
    {0xe8, ( 5<<4)+( 8)}, /* D1L/RR  */
    {0xf0, ( 5<<4)+( 8)}, /* D1L/RR  */
    {0xf8, ( 4<<4)+( 9)}  /* D1L/RR  */
};

static void keyon(unsigned int, unsigned int);
static void keyoff(unsigned int);

void main(){
    printf("OPM-MIDI version 0.0.1\n");
    printf("Copyright 2023 nobu24\n");

    /* OPM init */
    for(i = 0;i < 8;i++){
        for(j = 0;j < 25;j++){
            OPMSET((int)init[j][0]+i, (int)init[j][1]);
        }
        OPMSET(0x08, i);
    }

    /* main loop */
    a = b_fopen("aux", "r");
    ss = 0;
    vvc = 0;
    while(1){
        b = b_fgetc(a);
        if(b>>7 == 1){
            ss = b;
            vvc = 0;
        } else {
            vv[vvc] = b;
            vvc++;
        }
        if(vvc < 2) continue;

        switch(ss>>4){
            case 8:
                keyoff(vv[0]);
                break;
            case 9:
                if(vv[1] > 0){
                    keyon(vv[0], vv[1]);
                } else {
                    keyoff(vv[0]);
                }
                break;
        }
        vvc = 0;
    }
    b_fclose(a);
    return;
}

/* keyon */
static void keyon(tt, ex)
unsigned int tt, ex;
{
    for(i = 0;i < 8;i++){
        if(ch[i] == 0){
            ch[i] = tt;
            t = (tt-3)%12;
            if(t==0 || t==1) {tt=tt-12;t=t+13;}
            if(t>=2 && t<=8) t--;
            if(t>=1 && t<=4) t--;
            o = (tt-3)/12-1;
            OPMSET(0x60+i, (int)(init[5][1]+(127-ex)/4));
            OPMSET(0x68+i, (int)(init[6][1]+(127-ex)/4));
            OPMSET(0x70+i, (int)(init[7][1]+(127-ex)/4));
            OPMSET(0x78+i, (int)(init[8][1]+(127-ex)/4));
            OPMSET(0x28+i, (o<<4)+t);
            OPMSET(0x08, 0x78+i);
            break;
        }
    }
    return;
}

/* keyoff */
static void keyoff(tt)
unsigned int tt;
{
    for(i = 0;i < 8;i++){
        if(ch[i] == tt){
            ch[i] = 0;
            OPMSET(0x08, 0x00+i);
            break;
        }
    }
    return;
}
