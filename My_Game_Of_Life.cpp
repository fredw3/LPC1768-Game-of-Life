// An Implementation of Conway's 'Game of Life' using the ARM mbed Microcontroller in C Language

// by Freddie Whaley, SID 200883242

#include "mbed.h" // mbed microcontroller and Nokia N5110 LCD screen libraries included
#include "N5110.h"

//    VCC,SCE,RST,D/C,MOSI,SCLK,LED declared for N5110, various pins for the mbed too
N5110 lcd(p7,p8,p9,p10,p11,p13,p21);
AnalogIn ain1(p20);
DigitalIn bound_switch(p17);
DigitalIn pause_button(p19);
AnalogIn pot_level(p15);
DigitalIn reset_button(p16);
DigitalOut unused(p18);

void startup_screen(); // Functions are declared here
void seed_generator();
void conditional_buffer();
void hardwall_condition();
void periodic_condition();
void display();

int buffer[84][48]; // Main buffer initialised to store data

int main() // Main code
{
    lcd.init(); // Basic initialisations for the LCD display
    lcd.normalMode();
    lcd.setBrightness(0.5);
    startup_screen(); // Call to start the startup function

    seed_generator(); // Call to generate a random seed for the game

    while(reset_button < 1) {

        if (pause_button < 1) {

        conditional_buffer(); // Executes the rules of the GOL using conditions for the next refresh
        if (bound_switch == 1) {    // Allows the boundary switch to determine mode
            periodic_condition();
        } else {
            hardwall_condition();
        }
        display();

        float speed_check = pot_level.read()*3.3;   // Reads the input voltage on p15

        wait(speed_check);                          // Variable wait

        lcd.refresh();
        }
    }
    lcd.clear();
    main();
}







void seed_generator()   //Function for randomly generating pixels
{

        int LSB_noise = ain1.read_u16();  //Reads in the noise voltage on AnalogIn p20 and stores in variable 'LSB_noise'
        srand(LSB_noise);                 //Seeds random value generator with noise value

        for (int i=1; i<83; i++) {        //Sets all array values in buffer to the remainders of the random noise values
            for (int j=1; j<47; j++) {
                buffer[i][j]= rand()%2;
                if (buffer[i][j] > 0) {
                    lcd.setPixel(i,j);
                } else {
                    lcd.clearPixel(i,j);
                }
            }
        }
/*    lcd.setPixel(20,20); // Glider test initialisation
    lcd.setPixel(20,22);
    lcd.setPixel(21,21);
    lcd.setPixel(21,22);
    lcd.setPixel(22,21);
    lcd.refresh();
        
*/
}

void startup_screen()   // Function for the startup screen display
{

    lcd.printString("Welcome to", 0, 0); // Prints a startup message on the screen
    lcd.printString("Conway's", 0, 1);
    lcd.printString("Game of Life", 0, 2);
    lcd.printString("by Freddie W", 0, 4);
    lcd.refresh();
    wait(2.0);
    lcd.clear();
}

void conditional_buffer()   // Function for implementing the rules of the Game of Life to create subsequent array values
{
    for (int i=0; i<85; i++) {
        for (int j=0; j<49; j++) {
            unsigned int p = 0;        // Counter variable to check conditions of  neighbouring cells

            if (lcd.getPixel(i-1,j-1) != 0) {     // Game of Life rule checks depend on counter value:
                p++;                              // Top left cell
            }
            if (lcd.getPixel(i, j-1) != 0) {
                p++;                              // Top cell
            }
            if (lcd.getPixel(i+1, j-1) != 0) {
                p++;                              // Top right
            }
            if (lcd.getPixel(i-1, j) != 0) {
                p++;                              // Direct left
            }
            if (lcd.getPixel(i+1, j) != 0) {
                p++;                              // Direct right
            }
            if (lcd.getPixel(i-1, j+1) != 0) {
                p++;                              // Bottom left
            }
            if (lcd.getPixel(i, j+1) != 0) {
                p++;                              // Bottom
            }
            if (lcd.getPixel(i+1, j+1) != 0) {
                p++;                              // Bottom right
            }


            if (p == 2) {                         // 2 neighbours
                if (lcd.getPixel(i, j) != 0) {
                    buffer[i][j] = 1;             // Alive condition
                } else {
                    buffer[i][j] = 0;             // Dead condition
                }

            } else if (p == 3) {                  // 3 neighbours always results in the cell being alive
                buffer[i][j] = 1;
            } else {
                buffer[i][j] = 0;
            }


        }
    }
}

void hardwall_condition()              // Sets a hard wall (alive) in the bordering pixels of the array (screen)
{
    for (int i=0; i<84; i++) {
        buffer[i][0] = 1;
        buffer[i][47] = 1;
    }
    for (int j=0; j<48; j++) {
        buffer[0][j] = 1;
        buffer[83][j] = 1;
    }
}



void periodic_condition()               // Sets a periodic wall so pixels are 'reflected'
{

    for (int i=1; i<83; i++) {
        unsigned int p = 0;

        if (lcd.getPixel(i-1,46) != 0) {     // Game of Life rules modified for bottom row of display
            p++;
        }
        if (lcd.getPixel(i, 46) != 0) {
            p++;
        }
        if (lcd.getPixel(i+1, 46) != 0) {
            p++;
        }
        if (lcd.getPixel(i-1, 47) != 0) {
            p++;
        }
        if (lcd.getPixel(i+1, 47) != 0) {
            p++;
        }
        if (lcd.getPixel(i-1, 0) != 0) {
            p++;
        }
        if (lcd.getPixel(i, 0) != 0) {
            p++;
        }
        if (lcd.getPixel(i+1, 0) != 0) {
            p++;
        }


        if (p == 2) {
            if (lcd.getPixel(i, 47) != 0) {
                buffer[i][47] = 1;
            } else {
                buffer[i][47] = 0;
            }

        } else if (p == 3) {
            buffer[i][47] = 1;
        } else {
            buffer[i][47] = 0;
        }


    }

    for (int i=1; i<83; i++) {                      // GOL rules for top row
        unsigned int p = 0;

        if (lcd.getPixel(i-1,47) != 0) {
            p++;
        }
        if (lcd.getPixel(i, 47) != 0) {
            p++;
        }
        if (lcd.getPixel(i+1, 47) != 0) {
            p++;
        }
        if (lcd.getPixel(i-1, 0) != 0) {
            p++;
        }
        if (lcd.getPixel(i+1, 0) != 0) {
            p++;
        }
        if (lcd.getPixel(i-1, 1) != 0) {
            p++;
        }
        if (lcd.getPixel(i, 1) != 0) {
            p++;
        }
        if (lcd.getPixel(i+1, 1) != 0) {
            p++;
        }


        if (p == 2) {
            if (lcd.getPixel(i, 0) != 0) {
                buffer[i][0] = 1;
            } else {
                buffer[i][0] = 0;
            }

        } else if (p == 3) {
            buffer[i][0] = 1;
        } else {
            buffer[i][0] = 0;
        }


    }

    for (int j=1; j<47; j++) {
        unsigned int p = 0;

        if (lcd.getPixel(82,j-1) != 0) {     // GOL rules for rightmost row of display
            p++;
        }
        if (lcd.getPixel(83, j-1) != 0) {
            p++;
        }
        if (lcd.getPixel(0, j-1) != 0) {
            p++;
        }
        if (lcd.getPixel(82, j) != 0) {
            p++;
        }
        if (lcd.getPixel(0, j) != 0) {
            p++;
        }
        if (lcd.getPixel(82, j+1) != 0) {
            p++;
        }
        if (lcd.getPixel(83, j+1) != 0) {
            p++;
        }
        if (lcd.getPixel(0, j+1) != 0) {
            p++;
        }


        if (p == 2) {
            if (lcd.getPixel(83, j) != 0) {
                buffer[83][j] = 1;
            } else {
                buffer[83][j] = 0;
            }

        } else if (p == 3) {
            buffer[83][j] = 1;
        } else {
            buffer[83][j] = 0;
        }


    }

    for (int j=1; j<47; j++) {                  // GOL rules for leftmost row
        unsigned int p = 0;

        if (lcd.getPixel(83,j-1) != 0) {
            p++;
        }
        if (lcd.getPixel(0, j-1) != 0) {
            p++;
        }
        if (lcd.getPixel(1, j-1) != 0) {
            p++;
        }
        if (lcd.getPixel(83, j) != 0) {
            p++;
        }
        if (lcd.getPixel(1, j) != 0) {
            p++;
        }
        if (lcd.getPixel(83, j+1) != 0) {
            p++;
        }
        if (lcd.getPixel(0, j+1) != 0) {
            p++;
        }
        if (lcd.getPixel(1, j+1) != 0) {
            p++;
        }


        if (p == 2) {
            if (lcd.getPixel(0, j) != 0) {
                buffer[0][j] = 1;
            } else {
                buffer[0][j] = 0;
            }

        } else if (p == 3) {
            buffer[0][j] = 1;
        } else {
            buffer[0][j] = 0;
        }


    }


    unsigned int p = 0;
    if (lcd.getPixel(82,46) != 0) {     // Game of Life corner conditions
        p++;
    }
    if (lcd.getPixel(83, 46) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 46) != 0) {
        p++;
    }
    if (lcd.getPixel(82, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(82, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(83, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 0) != 0) {
        p++;
    }


    p = 0;
    if (p == 2) {
        if (lcd.getPixel(83, 47) != 0) {
            buffer[83][47] = 1;
        } else {
            buffer[83][47] = 0;
        }

    } else if (p == 3) {
        buffer[83][47] = 1;
    } else {
        buffer[83][47] = 0;
    }


    p = 0;
    if (lcd.getPixel(83,46) != 0) {    
        p++;
    }
    if (lcd.getPixel(0, 46) != 0) {
        p++;
    }
    if (lcd.getPixel(1, 46) != 0) {
        p++;
    }
    if (lcd.getPixel(83, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(1, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(83, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(1, 0) != 0) {
        p++;
    }


    if (p == 2) {
        if (lcd.getPixel(0, 47) != 0) {
            buffer[0][47] = 1;
        } else {
            buffer[0][47] = 0;
        }

    } else if (p == 3) {
        buffer[0][47] = 1;
    } else {
        buffer[0][47] = 0;
    }


    p = 0;
    if (lcd.getPixel(82,47) != 0) {     
        p++;
    }
    if (lcd.getPixel(83, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(82, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(82, 1) != 0) {
        p++;
    }
    if (lcd.getPixel(83, 1) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 1) != 0) {
        p++;
    }


    if (p == 2) {
        if (lcd.getPixel(83, 0) != 0) {
            buffer[83][0] = 1;
        } else {
            buffer[83][0] = 0;
        }

    } else if (p == 3) {
        buffer[83][0] = 1;
    } else {
        buffer[83][0] = 0;
    }


    p = 0;
    if (lcd.getPixel(83,47) != 0) {     
        p++;
    }
    if (lcd.getPixel(0, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(1, 47) != 0) {
        p++;
    }
    if (lcd.getPixel(83, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(1, 0) != 0) {
        p++;
    }
    if (lcd.getPixel(83, 1) != 0) {
        p++;
    }
    if (lcd.getPixel(0, 1) != 0) {
        p++;
    }
    if (lcd.getPixel(1, 1) != 0) {
        p++;
    }


    if (p == 2) {
        if (lcd.getPixel(0, 0) != 0) {
            buffer[0][0] = 1;
        } else {
            buffer[0][0] = 0;
        }

    } else if (p == 3) {
        buffer[0][0] = 1;
    } else {
        buffer[0][0] = 0;
    }


}



void display()
{
    for (int i=0; i<84; i++) {        // Sets LCD to display alive or dead cells
        for (int j=0; j<48; j++) {
            if (buffer[i][j] > 0) {
                lcd.setPixel(i,j);
            } else {
                lcd.clearPixel(i,j);
            }
        }
    }
}