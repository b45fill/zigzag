#include <LiquidCrystal.h>
#include <AccelStepper.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7 );

// Нажатые кнопки
int button;
const int BUTTON_NONE   = 0;
const int BUTTON_RIGHT  = 1;
const int BUTTON_UP     = 2;
const int BUTTON_DOWN   = 3;
const int BUTTON_LEFT   = 4;
const int BUTTON_SELECT = 5;

int getPressedButton() {
  int buttonValue = analogRead(0); // считываем значения с аналогового входа(A0)
  if (buttonValue < 50) {
    return BUTTON_RIGHT;
  }
  else if (buttonValue < 200) {
    return BUTTON_UP;
  }
  else if (buttonValue < 400) {
    return BUTTON_DOWN;
  }
  else if (buttonValue < 600) {
    return BUTTON_LEFT;
  }
  else if (buttonValue < 800) {
    return BUTTON_SELECT;
  }
  return BUTTON_NONE;
}
AccelStepper Stepper1(1,6,5); //использует пин 12 и 13 для dir и step, 1 - режим "external driver" (A4988)

const int PIN_BUTTON = 7; //сигнал с оптопары положения вала
int dir = 1; //используется для смены направления
int zigzag_mode = 1; //используется для смены вида зигзага
int steps1 = 40; //используется для ширины 2x
int steps2 = 15; //используется для ширины 4x
int num = 1; //количество шагов в 4*укольном зигзаге
int Sign = 2; //срабатывание позиционера

void setup() {
 


Stepper1.setSpeed(10000); //устанавливаем максимальную скорость вращения ротора двигателя (шагов/секунду)
Stepper1.setAcceleration(50000); //устанавливаем ускорение (шагов/секунду^2) 

    lcd.begin(16, 2);
  lcd.print("Zigzag ");
  lcd.setCursor(0, 1);
      lcd.print("Width");
      lcd.setCursor(6, 1);
        lcd.print(steps1);}
 



void loop(){
  
 
  
 button = getPressedButton();
  switch (button) {
    case BUTTON_RIGHT: // при 
      
     Stepper1.move(-1);
       Stepper1.run();
      delay(200); //задержка
      break;
      
    case BUTTON_LEFT:
     
       Stepper1.move(-1);
       Stepper1.run();
       delay(200); //задержка 
      break;
    case BUTTON_UP:
       if (zigzag_mode == 1){
        steps1=steps1+1;
        lcd.setCursor(6, 1);
         lcd.print("      ");
        lcd.setCursor(6, 1);
        lcd.print(steps1);}
        else 
        { steps2=steps2+1;
        lcd.setCursor(6, 1);
         lcd.print("      ");
        lcd.setCursor(6, 1);
        lcd.print(steps2);}
        
      delay(200); //задержка 
      break;
    case BUTTON_DOWN:
      lcd.setCursor(6, 1);
         lcd.print("      ");
      if (zigzag_mode == 1){
        steps1=steps1-1;
        lcd.setCursor(6, 1);
        lcd.print(steps1);}
        else 
        { steps2=steps2-1;
        lcd.setCursor(6, 1);
        lcd.print(steps2);}
       delay(200);
      break;
    case BUTTON_SELECT:
    
     if (zigzag_mode == 2){
                lcd.setCursor(0, 0);
                lcd.print("                 ");
                lcd.setCursor(0, 0);
                 lcd.print("Line Stitch");
                 lcd.setCursor(6, 1);
                  lcd.print("      ");
                  delay(100);
              zigzag_mode = 0 ;} 
             else
             if (zigzag_mode == 0){
                lcd.setCursor(0, 0);
                lcd.print("                 ");
                lcd.setCursor(0, 0);
                 lcd.print("Zigzag  ");
                 zigzag_mode = zigzag_mode + 1;
                  lcd.setCursor(6, 1);
                  lcd.print("      ");
                   lcd.setCursor(6, 1);
                   lcd.print(steps1);
                delay(100); }
                 else           {
                lcd.setCursor(0, 0);
                lcd.print("                 ");
                lcd.setCursor(0, 0);
                 lcd.print("Zigzag 4");
                 
                lcd.setCursor(6, 1);
                lcd.print("      ");
                lcd.setCursor(6, 1);
                lcd.print(steps2);
                  delay(100);
              zigzag_mode = zigzag_mode + 1; } 
    
           
       
     //  lcd.setCursor(12, 1);
      //          lcd.print(zigzag_mode);       
      
       delay(200);
       
 
      break;
  }
 
 
 
 if(Stepper1.distanceToGo()==0){ //проверка, отработал ли двигатель предыдущее движение
  int buttonState = digitalRead(PIN_BUTTON); 
      if (buttonState == 0){
 
                      if (Sign == 1){
                                    
                                    
                                    if (zigzag_mode  == 1)   {                 
                                          Stepper1.move(steps1*dir); //устанавливает  перемещение
                                          dir = dir*(-1); //отрицательное значение dir, благодаря чему реализуется вращение в противоположном направлении 
                                         
                                          delay(1); //задержка на 1 секунду
                                          Sign = 2;
                                                  }
              
                                   
                                    else  // зигзаг мода равен 2
                                          if (num < 4) {
                                              num = (num+1);
                          
                                               Stepper1.move(steps2*dir); //устанавливает следующее перемещение  будет перемещаться -1600 -> противоположное направление)
                                               delay(5); //задержка
                                               
                                               Sign=2;   }
                                               
                                          else {
                                             dir = dir*(-1); //отрицательное значение dir, благодаря чему реализуется вращение в противоположном направлении
                                             num = 1; }
                        }  
                        }
       else      Sign = 1;
        
      
                        }   
                         
    Stepper1.run(); //запуск шагового двигателя.     
 
     
                 
} 
