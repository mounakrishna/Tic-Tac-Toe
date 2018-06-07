# Tic-Tac-Toe
### Let's play tic tac toe using leds.

##### The components needed are
    1. Arduino.
    2. 9 LEDs.
    3. 3 push buttons.
    4. 3 capacitors (0.1uf).
    5. Some resistors (near 10K ohms).
    6. Jumper wires or wires.

##### The connections are
The +ve of leds are connected to 9 pins of arduino and -ve connected to ground.
The push buttons are used as pull ups by connecting one pin to arduino and other to ground. The pin is kept high. An RC circuit is connected paralled to the button to debounce the switch.
