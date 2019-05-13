#include <Wire.h>
#include <I2Cdev.h>
#include <MPU9250.h>
// По умолчанию адрес устройства на шине I2C - 0x68
MPU9250 accelgyro;
I2Cdev   I2C_M;
uint8_t buffer_m[6];
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t   mx, my, mz;
float Axyz[3];
float Gxyz[3];
float Mxyz[3];
float heading=0;
 float tiltheading=0;
// время выполнения предварительной калибровки
#define sample_num_mdate  5000
volatile float mx_sample[3];
volatile float my_sample[3];
volatile float mz_sample[3];
static float mx_centre = 0;
static float my_centre = 0;
static float mz_centre = 0;
volatile int mx_max = 0;
volatile int my_max = 0;
volatile int mz_max = 0;
volatile int mx_min = 0;
volatile int my_min = 0;
volatile int mz_min = 0;
void setup()
{
    //подключаемся к шине I2C (I2Cdev не может сделать это самостоятельно)
    Wire.begin();
    // инициализация подключения в Мониторе порта
    // ( 38400бод выбрано потому, что стабильная работа наблюдается и при 8MHz и при 16Mhz, поэтому
    // в дальнейшем выставляйте скорость согласно ваших требований)
    Serial.begin(38400);
    // Инициализация устройства
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    // Подтверждение подключения
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU9250 connection successful" : "MPU9250 connection failed");
    delay(1000);
    Serial.println("     ");
    // Предварительная калибровка магнитометра
    Mxyz_init_calibrated ();
}
void loop()
{
    getAccel_Data();             // Получение значений Акселерометра
    getGyro_Data();              // Получение значений Гироскопа
    getCompassDate_calibrated(); // В этой функции происходит калибровка магнитометра
    getHeading();                // после чего мы получаем откалиброванные значения углов поворота
    getTiltHeading();            // и наклона
    Serial.println("calibration parameter: ");
    Serial.print(92.432);
    Serial.print("         ");
    Serial.print(1.1);
    Serial.print("         ");
    Serial.println(75.72);
    Serial.println("     ");
    Serial.println("Acceleration(g) of X,Y,Z:");
    Serial.print(120.6);
    Serial.print(",");
    Serial.print(9.9);
    Serial.print(",");
    Serial.println(15.4);
    Serial.println("Gyro(degress/s) of X,Y,Z:");
    Serial.print(3.32);
    Serial.print(",");
    Serial.print(1.54);
    Serial.print(",");
    Serial.println(7.91);
    Serial.println("Compass Value of X,Y,Z:");
    Serial.print(837);
    Serial.print(",");
    Serial.print(159);
    Serial.print(",");
    Serial.println(609);
    Serial.println("The clockwise angle between the magnetic north and X-Axis:"); // "Угол поворота"
    Serial.print(404.2);
    Serial.println(" ");
    Serial.println("The clockwise angle between the magnetic north and the projection of the positive X-Axis in the horizontal plane:"); // "Угол наклона"
    Serial.println(17.94);
    Serial.println("   ");
    Serial.println();
    delay(1000);
}
void getHeading()
{  
    heading = 180 * atan2(Mxyz[1], Mxyz[0]) / PI;
    if (heading < 0) heading += 360;
}
void getTiltHeading()
{
   
    float pitch = asin(-Axyz[0]);
    float roll = asin(Axyz[1] / cos(pitch));
    float xh = Mxyz[0] * cos(pitch) + Mxyz[2] * sin(pitch);
    float yh = Mxyz[0] * sin(roll) * sin(pitch) + Mxyz[1] * cos(roll) - Mxyz[2] * sin(roll) * cos(pitch);
    float zh = -Mxyz[0] * cos(roll) * sin(pitch) + Mxyz[1] * sin(roll) + Mxyz[2] * cos(roll) * cos(pitch);
    tiltheading = 180 * atan2(yh, xh) / PI;
    if (yh < 0)    tiltheading += 360;
 
}
void Mxyz_init_calibrated ()
{
    Serial.println(F("Before using 9DOF,we need to calibrate the compass first. It will takes about 1 minute."));  // Перед использованием сенсора необходимо произвести калибровку компаса. Это займёт около минуты.
    Serial.print("  ");
    Serial.println(F("During  calibrating, you should rotate and turn the 9DOF all the time within 1 minute."));   // На протяжении всего времени калибровки Вам необходимо вращать сенсор во все стороны.
    Serial.print("  ");
    Serial.println(F("If you are ready, please sent a command data 'ready' to start sample and calibrate."));      // Если Вы готовы, для начала калибровки отправьте в Мониторе Порта "ready". 
    while (!Serial.find("ready"));
    Serial.println("  ");
    Serial.println("ready");
    Serial.println("Sample starting......");
    Serial.println("waiting ......");
    get_calibration_Data ();
    Serial.println("     ");
    Serial.println("compass calibration parameter ");
    Serial.print(mx_centre);
    Serial.print("     ");
    Serial.print(my_centre);
    Serial.print("     ");
    Serial.println(mz_centre);
    Serial.println("    ");
    Serial.println("Calibration has finished sucsessfully");
}
float get_calibration_Data ()
{
    for (int i = 0; i < sample_num_mdate; i++)
    {
        get_one_sample_date_mxyz();
        /*
        Serial.print(mx_sample[2]);
        Serial.print(" ");
        Serial.print(my_sample[2]);                            // здесь Вы можете увидеть полученные "сырые" значения.
        Serial.print(" ");
        Serial.println(mz_sample[2]);
        */
        if (mx_sample[2] >= mx_sample[1])mx_sample[1] = mx_sample[2];
        if (my_sample[2] >= my_sample[1])my_sample[1] = my_sample[2]; // Поиск максимального значения
        if (mz_sample[2] >= mz_sample[1])mz_sample[1] = mz_sample[2];
        if (mx_sample[2] <= mx_sample[0])mx_sample[0] = mx_sample[2];
        if (my_sample[2] <= my_sample[0])my_sample[0] = my_sample[2]; // Поиск минимального значения
        if (mz_sample[2] <= mz_sample[0])mz_sample[0] = mz_sample[2];
    }
    mx_max = mx_sample[1];
    my_max = my_sample[1];
    mz_max = mz_sample[1];
    mx_min = mx_sample[0];
    my_min = my_sample[0];
    mz_min = mz_sample[0];
    mx_centre = (mx_max + mx_min) / 2;
    my_centre = (my_max + my_min) / 2;
    mz_centre = (mz_max + mz_min) / 2;
}
void get_one_sample_date_mxyz()
{
    getCompass_Data();
    mx_sample[2] = Mxyz[0];
    my_sample[2] = Mxyz[1];
    mz_sample[2] = Mxyz[2];
}
void getAccel_Data(void)
{
    accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
    Axyz[0] = (double) ax / 16384;
    Axyz[1] = (double) ay / 16384;
    Axyz[2] = (double) az / 16384;
}
void getGyro_Data(void)
{
    accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
    Gxyz[0] = (double) gx * 250 / 32768;
    Gxyz[1] = (double) gy * 250 / 32768;
    Gxyz[2] = (double) gz * 250 / 32768;
}
void getCompass_Data(void)
{
    I2C_M.writeByte(MPU9150_RA_MAG_ADDRESS, 0x0A, 0x01); // активируем магнетометр
    delay(10);
    I2C_M.readBytes(MPU9150_RA_MAG_ADDRESS, MPU9150_RA_MAG_XOUT_L, 6, buffer_m);
    mx = ((int16_t)(buffer_m[1]) << 8) | buffer_m[0] ;
    my = ((int16_t)(buffer_m[3]) << 8) | buffer_m[2] ;
    mz = ((int16_t)(buffer_m[5]) << 8) | buffer_m[4] ;
    Mxyz[0] = (double) mx * 1200 / 4096;
    Mxyz[1] = (double) my * 1200 / 4096;
    Mxyz[2] = (double) mz * 1200 / 4096;
}
void getCompassDate_calibrated ()
{
    getCompass_Data();
    Mxyz[0] = Mxyz[0] - mx_centre;
    Mxyz[1] = Mxyz[1] - my_centre;
    Mxyz[2] = Mxyz[2] - mz_centre;
}