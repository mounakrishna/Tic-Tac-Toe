#include<avr/io.h>
#include<avr/interrupt.h>
                                              /*LED MATRIX
                                                 B3   | B0  |  D5
                                                (0,2) |(1,2)| (2,2)
                                                ------+-----+------
                                                 B4   | B1  |  D6 
                                                (0,1) |(1,1)| (2,1)
                                                ------+-----+------
                                                 B5   | B2  |  D7
                                                (0,0) |(1,0)| (2,0)
                                              */ 

void blink_glow();
void winner();
void i_check();
void j_check();

int turn=1,i=0,j=0,b=0,start=0;                //'start' variable is used to start or stop the selection. 'turn' variable is used to know the turn no.              
int a[3][3] = { 4,4,4,                         //1 = x, 0 = o, 4 = not occupied.
                4,4,4,
                4,4,4 };
int no[3][3] = { 5,2,7,                       //Variable that stores the pin no. of corresponding coordinate.                                          
                 4,1,6,                
                 3,0,5 };                                     

int main()
{  
  DDRB =0xff;                                                             
  DDRD = 0b11100000;                                                      
  PORTD = 0b00011100;                    //D4(left button)= To fix the position, D3(middle button)= Move top, D2(right button)= Move right.                                                
  PORTB = 0x00;                                                                         

  TCCR1B |= (1<<CS12) | (1<<CS10);       //To blink the cursor led using Timer1 overflow interrupt.
  TCNT1 = 64000;
  TIMSK1 |= (1<<TOIE1);                  //Enabling timer1 interrupt.
  EIMSK |= (1<<INT0) | (1<<INT1);        //Enabling external interrupts.
  EICRA |= (1<<ISC01)| (1<<ISC11);       //Falling edge.
  PCICR |= (1<<PCIE2);
  PCMSK2 |= (1<<PCINT20);                //D4 pin configured as Pin Change Interrupt.
  sei();                                 //Enabling global interrupts.
  
  blink_glow();                          //Calling function to blink or glow the led for O or X respectively.
}

void blink_glow()
{
  TCCR0B |= (1<<CS01);
  int p=0,l,m;
  while(1)
  {
    if(TCNT0 > 200)             //Start of code to blink the led.
    {
      TCNT0=0;
      p++;
      if(p==8000)
      {
        for(l=0;l<3;l++)
        {
          for(m=0;m<3;m++)
          {
            if(m<2 && a[l][m]==0)
              PORTB ^= (1<<no[l][m]);                 
            else if(a[l][m]==0)
              PORTD ^= (1<<no[l][m]);
          }
        }     
        p=0;                 
      }                         //End of code to blink the led.
    }
        for(l=0;l<3;l++)        //Start of code to glow the led.
        {
          for(m=0;m<3;m++)
          {
            if(m<2 && a[l][m]==1)
              PORTB ^= (1<<no[l][m]);                 
            else if(a[l][m]==1)
              PORTD ^= (1<<no[l][m]);
          }
        }                       //End of code to glow the led.
   }                             
}

void winner()
{
  int q,r,s,k=0; 
    for(q=0;q<3;q++)
    {
      if((a[q][0]+a[q][1]+a[q][2])==3 || (a[0][q]+a[1][q]+a[2][q])==3 || (a[0][0]+a[1][1]+a[2][2])==3 || (a[2][0]+a[1][1]+a[0][2])==3)       //Checks whether X wins or not.
      {
         for(r=0;r<3;r++)      
         {
          for(s=0;s<3;s++)
          {
            a[r][s]=1;            //To glow all the leds.
            k++;                  //To know whether the winner has been decided or not. if k>0 winner is decided.
          }
         }
      }
      else if((a[q][0]+a[q][1]+a[q][2])==0 || (a[0][q]+a[1][q]+a[2][q])==0 || (a[0][0]+a[1][1]+a[2][2])==0 || (a[2][0]+a[1][1]+a[0][2])==0)  //Checks whether O wins or not.  
      {
        for(r=0;r<3;r++)
         {
          for(s=0;s<3;s++)
          {
            a[r][s]=0;         //To blink all the leds.
            k++;               //To know whether the winner has been decided or not. if k>0 winner is decided.
          }
         }
      }
     }
    if(turn==10 && k==0)            //Condition to start from first if draw.
     {
       for(r=0;r<3;r++)
         {
          for(s=0;s<3;s++)
          {
            a[r][s]=4;
          }
         }
        PORTB=0x00;
        PORTD=0b00011100;
        turn=1;
     }
}
   
void i_check()               //Function that checks whether the led is already X or O and shift the cursor to top accordingly.
{
  while(1)
  {
    if(a[i][j]==0 || a[i][j]==1)
      i++;
    else if(i>2)
    {
      i=0;j++;
      if(j>2)
      {
        i=0;j=0;
      }
    }
    else
      break;
  }
}

void j_check()            //Function that checks whether the led is already X or O and shift the cursor to right accordingly.
{
  while(1)
  {
    if(a[i][j]==0 || a[i][j]==1)
    {
      j++;
    }
    else if(j>2)
    {
      j=0;i++;
      if(i>2)
      {
        i=0;j=0;
      }
    } 
    else
      break;
  }
}

ISR(INT0_vect)                   //Cursor shifts right.
{
  if(start==0)
  {
    j_check();
    start++;
  }
  else
  {
  if(j<2)                        //To turn OFF the previous cursor led.
    PORTB &= ~(1<<no[i][j]);
  else
    PORTD &= ~(1<<no[i][j]);
    j++;                          
    j_check();
  }
} 
ISR(INT1_vect)                   //Cursor shifts top.
{
  if(start==0)
  {
    i_check();
    start++;
  }
  else 
  {  
    if(j<2)                      //To turn OFF the previous cursor led.
      PORTB &= ~(1<<no[i][j]);   
    else
      PORTD &= ~(1<<no[i][j]);  
    i++; //Cursor shifts top.
    i_check(); 
  }
}

ISR(PCINT2_vect)               //Cursor stops.
{
  if(start==1) {
  if(turn%2==0)     //a[i][j] is 0 for o.               
    a[i][j]=0;
  else              //a[i][j] is 1 for x.
    a[i][j]=1;
    
  turn++;           //Turn no.
  if(turn>4)
  { 
    winner();       //To check the winner.
  }
  i=0;j=0;
  start=0;
}  
}
ISR(TIMER1_OVF_vect)               //to blink the cursor led.
{
  if(start==1)
  {
    if(b==0)
    {
     if(j<2)
       PORTB |= (1<<no[i][j]);
     else
       PORTD |= (1<<no[i][j]); 
     b++;  
    }
    else
    {
     if(j<2)
       PORTB &= ~(1<<no[i][j]);
     else
       PORTD &= ~(1<<no[i][j]);
     b=0;  
    }
 
  }
  TCNT1 = 64000;
}

