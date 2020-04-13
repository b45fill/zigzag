
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
AccelStepper Stepper1(1,22,23); //использует пин 12 и 13 для dir и step, 1 - режим "external driver" (A4988)

const int PIN_BUTTON = 21; //сигнал с оптопары положения вала
int dir = 1; //используется для смены направления
int dir1 = 1; //используется для выхода в ноль при старте программы 
int place = 1; //используется для фиксайии серидины зигзага
int place1 = 1; //используется для фиксайии нуля прямой строчки
int zigzag_mode = 1; //используется для смены вида зигзага
int steps1 = 40; //используется для шага меду уколами 2x
int steps2 = 15; //используется для шага меду уколами 4x
int num = 1; //номер шага в 4*укольном зигзаге
int Sign = 2; //срабатывание позиционера

void setup() {



  Stepper1.setSpeed(100000); //устанавливаем максимальную скорость вращения ротора двигателя (шагов/секунду)
  Stepper1.setAcceleration(500000); //устанавливаем ускорение (шагов/секунду^2)

  dir1=100;
  Stepper1.move(dir1);  //двигаемя вправо до упора
  Stepper1.run();
  delay(200); //задержка
  dir1=-50;
  Stepper1.move(dir1);//двигаемя обратно до середины 
  Stepper1.run();
  delay(200); //задержка
  place = Stepper1.currentPosition();  //запоминаем положение середины
  place1 = Stepper1.currentPosition(); //запоминаем положение середины


  lcd.begin(16, 2);            //начальная загрузка экрана
  lcd.print("Zigzag Two");
  lcd.setCursor(0, 1);
  lcd.print("Width 40");

}


void loop(){



  button = getPressedButton(); //цикл меню
  switch (button) {
  case BUTTON_RIGHT: // при 
    dir1=-1;
    Stepper1.move(dir1);
    Stepper1.run();
    delay(200); //задержка
    if (zigzag_mode == 0) place1 = Stepper1.currentPosition();
    break;

  case BUTTON_LEFT:
    dir1=1;
    Stepper1.move(dir1);
    Stepper1.run();
    delay(200); //задержка
    if (zigzag_mode == 0) place1 = Stepper1.currentPosition(); 
    break;
  case BUTTON_UP:
    if (zigzag_mode == 1){
      steps1=steps1+1;
      lcd.setCursor(6, 1);
      lcd.print("      ");
      lcd.setCursor(6, 1);
      lcd.print(steps1);
    }
    else 
    { 
      steps2=steps2+1;
      lcd.setCursor(6, 1);
      lcd.print("      ");
      lcd.setCursor(6, 1);
      lcd.print(steps2);
    }

    delay(200); //задержка 
    break;
  case BUTTON_DOWN:
    lcd.setCursor(6, 1);
    lcd.print("      ");
    if (zigzag_mode == 1){
      steps1=steps1-1;
      lcd.setCursor(6, 1);
      lcd.print(steps1);
    }
    else 
    { 
      steps2=steps2-1;
      lcd.setCursor(6, 1);
      lcd.print(steps2);
      delay(200);
    } 
    break;


  case BUTTON_SELECT:
    if (zigzag_mode == 2){ //прямая строчка
      lcd.setCursor(0, 0);
      lcd.print("                 ");
      lcd.setCursor(0, 0);
      lcd.print("Straight Stitch");

      zigzag_mode = 0 ; 
      Stepper1.moveTo(place1);
      Stepper1.run();
      delay(200);
    }
    else {
      if (zigzag_mode == 0) //обычный зигзаг
      {
        lcd.setCursor(0, 0);
        lcd.print("                 ");
        lcd.setCursor(0, 0);
        lcd.print("Zigzag Two ");
        Stepper1.moveTo( place + 0.5 * steps1);
        Stepper1.run();
        delay(200); 
      }
      if (zigzag_mode == 1)  //4х укольный зигзаг 
      {
        lcd.setCursor(0, 0);
        lcd.print("                 ");
        lcd.setCursor(0, 0);
        lcd.print("Zigzag Four");
        Stepper1.moveTo( place + 0.75 * steps2);
        Stepper1.run();
        delay(200);
        num = 1; 
      } 
      zigzag_mode = zigzag_mode + 1;      
    }  
    break;


  }  //конец меню


    //функция перемещения игловода
  if(Stepper1.distanceToGo()==0){ //проверка, отработал ли двигатель предыдущее движение
    int buttonState = digitalRead(PIN_BUTTON); 
    if (buttonState == 0){

      if (Sign == 1){


        if (zigzag_mode  == 1)   {                 
          Stepper1.move(steps1*dir); //устанавливает  перемещение
          dir = dir*(-1); //отрицательное значение dir, благодаря чему реализуется вращение в противоположном направлении 

          delay(3); //задержка на 1 секунду
          Sign = 2;
        }


        else  
          if (zigzag_mode  == 2) { // зигзаг мода равен 2
          if (num < 4) {
            num = (num+1);

            Stepper1.move(steps2*dir); //устанавливает следующее перемещение  > противоположное направление)
            delay(3); //задержка

            Sign=2;   
          }

          else {
            dir = dir*(-1); //отрицательное значение dir, благодаря чему реализуется вращение в противоположном направлении
            num = 1; 
          }
        }
      } 
    }
    else      Sign = 1;


  }   

  Stepper1.run(); //запуск шагового двигателя.     



} 

