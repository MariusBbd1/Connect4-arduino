#include <Adafruit_NeoPixel.h>

#define grille_Pin 7
#define N_LEDS 42

#define bande_Pin 8
#define bande_Leds 7

Adafruit_NeoPixel grille = Adafruit_NeoPixel(N_LEDS, grille_Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(bande_Leds, bande_Pin, NEO_GRB + NEO_KHZ800);

//GAME MATRIX
//                5  |00|  |01|  |02|  |03|  |04|  |05|  |06|
//                4  |07|  |08|  |09|  |10|  |11|  |12|  |13|
//                3  |14|  |15|  |16|  |17|  |18|  |19|  |20|
//                2  |21|  |22|  |23|  |24|  |25|  |26|  |27|
//                1  |28|  |29|  |30|  |31|  |32|  |33|  |34|
//                0  |35|  |36|  |37|  |38|  |39|  |40|  |41|
//COLUMN              0     1     2     3     4     5     6
const int right_button=3; //Buttons pins
const int middle_button=4;
const int left_button=5;
int matrix[7][6]; //- - - - - - - - - - - matrix[column][row]
int player; //- - - - - - - - - - - - - number 1: red leds - - - number 2: yellow leds
int num_colum;
int brightness=30;
long long frame;
int animation_delay=30;
int animation_length=0;
long long animation_count=0;
int led1=0,led2=0,led3=0,led4=0;
boolean animation_done=false;
boolean stacked=true;
boolean button_pressed=false;
boolean initialized=false;
boolean win1=false,win2=false;
int r=128,g=0,b=0;
int current_pixel;
int left_press;
int right_press;
int last_right_press;
int last_left_press;
int mid_button_press;

//########################################################### SET_ UP ##################################################################################################
void setup()
{
  grille.begin();
  grille.clear();
  grille.setBrightness(brightness);
  strip.begin();
  strip.clear();
  strip.setBrightness(brightness);
  Serial.begin(9600);
  pinMode(right_button,INPUT_PULLUP);
  pinMode(left_button,INPUT_PULLUP);
  pinMode(middle_button,INPUT_PULLUP);
  pinMode(grille_Pin,OUTPUT);
  pinMode(bande_Pin,OUTPUT);
}

//######################################################## LOOP #########################################################################################################
void loop()
{
  frame=millis();
  
  if(!initialized)
  {
    initializing_leds();
  }
  pushing_button();
  piece_animation(num_colum, frame);
  stacking_pieces(num_colum, player);
  check_win();
}
//################################################### SUPERIOR_LED #########################################################################################################
//returns the number of most superior led of column
int superior_led(int column)
{
  int numero_pixel;
  numero_pixel = column;
  return numero_pixel;
}


//##################################################### PIECE_ANIMATION ##############################################################################################
void piece_animation(int col, long long frame)
{
  long long subtraction = frame - animation_count;

  if( subtraction > animation_delay && button_pressed)
  {
    animation_count=frame;                 //update the account
    grille.setPixelColor(superior_led(col)+7*animation_length,r,g,b);////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    grille.show();
    if( animation_length!=0 &&
        animation_length<6-number_pieces(col) //if itsn't the last or first positions of column
      )
    {
      grille.setPixelColor((superior_led(col)-7)+7*animation_length,0);
      grille.show();
    }
    animation_length++;
    if(animation_length==6-number_pieces(col)) //if it's the last position of column
    {
      animation_done=true;
      animation_length=0;
    }
   }else
   if(total_of_pieces()==42)
   {
    delay(2000);
    animation_done=true;
    initialized=false;
  }
   
}
//############################################################ TOTAL_OF_PIECES ######################################################################################
//it returns the total of pieces on the game
int total_of_pieces()
{
  int total=0;
  for(int c=0;c<7;c++)
  {
    total += number_pieces(c);
  }
  return total;
}
//############################################################ NEXT_SPACE #####################################################################################
//it returns the led number where the piece will be stay in the column
int next_space(int column)
{
  int row = 0;
  int number=35+column;

  while (matrix[column][row] != 0)
  {
    row++;
    number-=7;
  }

  return row;
}

//########################################################### NUMBER_PIECES ###############################################################################################
//it returns the number of pieces in a column
int number_pieces(int column)
{
  int number_pieces = 0;

  for (int i=0; i<6; i++)
  {
    if (matrix[column][i] != 0)
    {
      number_pieces++;
    }
  }
  return number_pieces;
}

//############################################################ COLOR #################################################################################################
//it changes the color depending on the player's number
void color(int &r, int &g, int &b)
{
  if (player == 2)
  {
    r = 128;
    g = 128;
    b = 0;
  }else
  {
    r = 128;
    g = 0;
    b = 0;
  }
}

//############################################################## STACKING PIECES ####################################################################################
//it stacks the piece in the matrix, putting on a number (1,2) according to the player
void stacking_pieces(int col, int player){
  if(animation_done)
  {
    animation_done=false;
    if (number_pieces(col) == 0)  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - if there aren't pieces in column
    {
      if(player==1)
      {
        matrix[col][0] = 1;   // - - - - - - - - - - - - - - - - - - - - - - - - - - - put on the piece in the first column (matrix)
      }else
      {
        matrix[col][0] = 2;
      }
      monitor_matrix();
    }
    else   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - if there are pieces in the column
    {
      if(player==1)
      {
        matrix[col][next_space(col)] = 1; // - - - - - - - - - - - - - - - - - - put on the piece in the next space (matrix)
      }
      else
      {
        matrix[col][next_space(col)] = 2; //- - - - - - - - - - - - - - - - - - - put on the piece in the next space (matrix)
      }
      monitor_matrix();
    }
    switch_player();
    color(r,g,b);
    stacked=true;
    button_pressed=false;
    strip.setPixelColor(current_pixel,r,g,b);
    strip.show();
  }
}

//############################################################### MONITOR_MATRIX ################################################################################### 
//it shows on the Serial Monitor the matrix that represents the game
void monitor_matrix()
{
  for (int r = 5; r >=0; r--)
  {
    for (int c = 0; c < 7; c++)
    {
      Serial.print(" |");
      if(matrix[c][r]==1)
      {
        Serial.print("X");
      }else
      if(matrix[c][r]==2)
      {
        Serial.print("O");
      }else
      {
        Serial.print(" ");
      }
      Serial.print("| ");

    }
    Serial.println("");
  }
  Serial.println("-_________________________________-__________________________________-");
}
//########################################################### PUSHING_BUTTON ########################################################################################
// reads the inputs on the buttons and move the current pixel accordingly, switch player and adds button_pressed=true when middle button is pressed
void pushing_button(){
  left_press = digitalRead(left_button);
  right_press = digitalRead(right_button);
  mid_button_press = digitalRead(middle_button);

  if(left_press==0 && current_pixel!=0 && last_left_press!=left_press) {
    current_pixel--;
    strip.clear();
    strip.setPixelColor(current_pixel,r,g,b);
    strip.show();
  }
  if(right_press==0 && current_pixel!=6 && last_right_press!=right_press){
    current_pixel++;
    strip.clear();
    strip.setPixelColor(current_pixel,r,g,b);
    strip.show();
  }

  if(left_press!=last_left_press) last_left_press = left_press;

  if(right_press!=last_right_press) last_right_press=right_press;

  if(stacked){
    if(mid_button_press==0 && number_pieces(current_pixel)<6){
      num_colum=current_pixel;
      color(r,g,b);
      button_pressed=true;
      stacked=false;
    }
  }
}

//################################################################ NUM_LED ############################################################################################
//it returns the number of led in a column and a row

int num_led(int colum, int row)
{
  int num_led;

  num_led = (5-row)*7 + colum;
  
  return num_led;
}

//################################################################ FINAL ANIMATION ####################################################################################
void final_animation()
{
  color(r,g,b);
  for(int i=0;i<5;i++)
  {
    grille.setPixelColor(led1,r,g,b);
    grille.setPixelColor(led2,r,g,b);
    grille.setPixelColor(led3,r,g,b);
    grille.setPixelColor(led4,r,g,b);
    Serial.println("led1: ");
    Serial.print(led1);
    Serial.println("");
    Serial.println("led2: ");
    Serial.print(led2);
    Serial.println("");
    Serial.println("led3: ");
    Serial.print(led3);
    Serial.println("");
    Serial.println("led4: ");
    Serial.print(led4);
    Serial.println("");
    grille.show();
    delay(200);

    grille.setPixelColor(led1,0);
    grille.setPixelColor(led2,0);
    grille.setPixelColor(led3,0);
    grille.setPixelColor(led4,0);
    
    grille.show();

    delay(500);
    
  }
  initialized=false;
}
//################################################################ WIN_CONDITION ###################################################################################
void check_win()
{
  if(!win1&&!win2)
  {
  check_rows();
  check_columns();
  check_right();
  check_left();
  }else
  if(win1)
  {
    player=1;
    Serial.println("WIN1");
    final_animation();
  }else
  if(win2)
  {
    player=2;
    Serial.println("WIN2");
    final_animation();
  }
}

//##################################################################### CHECK_COLUMNS #################################################################################
//it checks the columns
void check_columns()
{
  for(int c=0;c<7;c++)
  {
    for(int f=0;f<3;f++)
    {
      if(matrix[c][f]==1&&matrix[c][f+1]==1&&matrix[c][f+2]==1&&matrix[c][f+3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c,f+1);
        led3=num_led(c,f+2);
        led4=num_led(c,f+3);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c][f+1]==2&&matrix[c][f+2]==2&&matrix[c][f+3]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c,f+1);
        led3=num_led(c,f+2);
        led4=num_led(c,f+3);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### CHECK_ROWS #####################################################################################
//it checks the rows
void check_rows()
{
  for(int f=0;f<5;f++)
  {
    for(int c=0;c<4;c++)
    {
      if(matrix[c][f]==1&&matrix[c+1][f]==1&&matrix[c+2][f]==1&&matrix[c+3][f]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f);
        led3=num_led(c+2,f);
        led4=num_led(c+3,f);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c+1][f]==2&&matrix[c+2][f]==2&&matrix[c+3][f]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f);
        led3=num_led(c+2,f);
        led4=num_led(c+3,f);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### CHECK_RIGHT ####################################################################################
//it checks the diagonal to right
void check_right()
{
    for(int c=0;c<4;c++)
  {
    for(int f=0;f<3;f++)
    {
      if(matrix[c][f]==1&&matrix[c+1][f+1]==1&&matrix[c+2][f+2]==1&&matrix[c+3][f+3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f+1);
        led3=num_led(c+2,f+2);
        led4=num_led(c+3,f+3);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c+1][f+1]==2&&matrix[c+2][f+2]==2&&matrix[c+3][f+3]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f+1);
        led3=num_led(c+2,f+2);
        led4=num_led(c+3,f+3);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### CHECK_LEFT #####################################################################################
//it checks the diagonal to left
void check_left()
{
  for(int c=0;c<4;c++)
  {
    for(int f=5;f>2;f--)
    {
      if(matrix[c][f]==1&&matrix[c+1][f-1]==1&&matrix[c+2][f-2]==1&&matrix[c+3][f-3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f-1);
        led3=num_led(c+2,f-2);
        led4=num_led(c+3,f-3);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c+1][f-1]==2&&matrix[c+2][f-2]==2&&matrix[c+3][f-3]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f-1);
        led3=num_led(c+2,f-2);
        led4=num_led(c+3,f-3);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### SWITCH_PLAYER ###################################################################################
void switch_player(){
  if(player==1) {player=2;}
  else {player=1;}
}

//################################################################## INITIALIZING LEDS #################################################################################
//it initializes all the game with the defautl parameters
void initializing_leds()
{
  grille.clear();
  grille.show();
  strip.clear();
  strip.setPixelColor(current_pixel,128,0,0);
  strip.show();
  win1=false;
  win2=false;
  for(int c=0;c<7;c++)
  {
    for(int r=0;r<6;r++)
    {
      matrix[c][r]=0;
    }
  }
  player=1;
  current_pixel=0;
  initialized=true;
}
