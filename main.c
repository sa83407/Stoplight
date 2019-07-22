#include <stdint.h>
#include "SysTick.h"
#include "msp432p401r.h"

#define SENSOR     (*((volatile uint8_t *) 0x40004C40)) //Port 5 Input
#define TRAFFIC    (*((volatile uint8_t *) 0x40004C23)) //Port 4 Output
#define PEDESTRIAN (*((volatile uint8_t *) 0x40004C03)) //Port 2 Output

struct State {
    uint32_t Out;
    uint32_t Time;
    const struct State *Next[15];
};
typedef const struct State STyp;

#define goN      &FSM[0]
#define waitN    &FSM[1]
#define goE      &FSM[2]
#define waitE    &FSM[3]
#define EandP     &FSM[4]
#define NandP     &FSM[5]
#define PwaitN   &FSM[6]
#define PwaitE   &FSM[7]
#define goP     &FSM[8]
#define hurryP1  &FSM[9]
#define hurryP2  &FSM[10]
#define hurryP3  &FSM[11]
#define hurryP4  &FSM[12]
#define hurryP5  &FSM[13]
#define hurryP6  &FSM[14]

STyp FSM[15] = {
               {0x61, 200, {goN, waitN, goN, waitN, NandP, NandP, NandP, NandP}},
               {0x62, 100,  {goE, goE, goE, goE, EandP, EandP, EandP, EandP}},
               {0x4C, 200, {goE, goE, waitE, waitE, EandP, EandP, EandP, EandP}},
               {0x54, 100,  {goN, goN, goN, goN, NandP, NandP, NandP, NandP}},
               {0x4C, 100, {goE, goE, waitE, waitE, PwaitE, PwaitE, PwaitE, PwaitE}},
               {0x61, 100, {goN, waitN, goN, waitN, PwaitN, PwaitN, PwaitN, PwaitN}},
               {0x62, 100, {goP, goP, goP, goP, goP, goP, goP, goP}},
               {0x54, 100, {goP, goP, goP, goP, goP, goP, goP, goP}},
               {0xA4, 300, {hurryP1, hurryP1, hurryP1, hurryP1, hurryP1, hurryP1, hurryP1, hurryP1}},
               {0x64, 100, {hurryP2, hurryP2, hurryP2, hurryP2, hurryP2, hurryP2, hurryP2, hurryP2}},
               {0x24, 100, {hurryP3, hurryP3, hurryP3, hurryP3, hurryP3, hurryP3, hurryP3, hurryP3}},
               {0x64, 100, {hurryP4, hurryP4, hurryP4, hurryP4, hurryP4, hurryP4, hurryP4, hurryP4}},
               {0x24, 100, {hurryP5, hurryP5, hurryP5, hurryP5, hurryP5, hurryP5, hurryP5, hurryP5}},
               {0x64, 100, {hurryP6, hurryP6, hurryP6, hurryP6, hurryP6, hurryP6, hurryP6, hurryP6}},
               {0x24, 100, {goN, goE, goN, goN, goP, goE, goN, goN}}
};


void main(void){
    STyp *Pt;
    uint32_t Input;
    uint32_t Temp;

    SysTick_Init();

    //2.0,2.1 BOARD LEDS
    P2DIR |= 0x03;

    // 4.0-4.5 external LEDS stoplights
    P4DIR |= 0x3F;   

    // 5.0-5.2 external pushbuttons
    P5DIR &= ~0x07; 

    Pt = goN;  //intial state

    while(1) {
        TRAFFIC = (TRAFFIC&~0x3F) | (Pt->Out);
        Temp = Pt->Out;
        Temp = Temp>>6;
        PEDESTRIAN = (PEDESTRIAN&~0x03) | (Temp);
        SysTick_Wait10ms(Pt->Time);
        Input = (SENSOR&0x07);
        Pt = Pt->Next[Input];
    }

}


