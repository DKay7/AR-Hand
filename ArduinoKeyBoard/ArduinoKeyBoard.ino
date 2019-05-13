int PinOut[4] {5, 4, 3, 2}; // пины выходы
 
int PinIn[4] {9, 8, 7, 6}; // пины входа
int val = 0;
const int value[4][4]
 
 
{ {1,  2,   3,  4},
  {5,  6,   7,  8},
  {9,  10, 11, 12},
  {13, 14, 15, 16}
};
// двойной массив, обозначающий кнопку
 
int b = 0; // переменная, куда кладется число из массива(номер кнопки)
 
void setup()
{
  pinMode (2, OUTPUT); // инициализируем порты на выход (подают нули на столбцы)
  pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (5, OUTPUT);
 
  pinMode (6, INPUT); // инициализируем порты на вход с подтяжкой к плюсу (принимают нули на строках)
  digitalWrite(6, HIGH);
  
  pinMode (7, INPUT);
  digitalWrite(7, HIGH);
  
  pinMode (8, INPUT);
  digitalWrite(8, HIGH);
  
  pinMode (9, INPUT);
  digitalWrite(9, HIGH);
 
  Serial.begin(9600); // открываем Serial порт
}
 
void matrix () // создаем функцию для чтения кнопок
{
  for (int i = 0; i < 4; i++){ // цикл, передающий 0 по всем столбцам
    digitalWrite(PinOut[i], LOW); // если i меньше 4 , то отправляем 0 на ножку
    
    for (int j = 0; j < 4; j++) // цикл, принимающих 0 по строкам
      if (digitalRead(PinIn[j]) == LOW){ // если один из указанных портов входа равен 0, то..
        Serial.write( value[i][j]); // то b равно значению из двойного массива
        delay(175);
      }

    digitalWrite(PinOut[i], HIGH); // подаём обратно высокий уровень
  }
}
 
void loop()
{
  matrix(); // используем функцию опроса матричной клавиатуры
 
}
