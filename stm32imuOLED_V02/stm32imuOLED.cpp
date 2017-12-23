#include <stm32imuOLED.h>

//oled ekran tanımı
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define OLED_address  0x3c // OLED I2C adresi
//MPU 
const int MPU=0x68;  //  MPU-6050 I2C adresi

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

float xx,xy,xz;
float yx,yy,yz;
float zx,zy,zz;
float donusumDeger;

int Xan,Yan;
int Xoff;
int Yoff;
int Zoff;

int cizgiCizim; // çizimi yapılacak çizgiler  .
int eskiCizgiCizim; //  çizimi yapılmış olan çizgiler. değişiklik olan eski çizgiler silinecektir.

struct nokta3B
{
  int x;
  int y;
  int z;
};

struct nokta2B
{
  int x;
  int y;
};

struct cizgi3B
{
  nokta3B p0;
  nokta3B p1;
};

struct cizgi2B
{
  nokta2B p0;
  nokta2B p1;
};

cizgi3B cizgiler[12];  // çizimi yapılacak çizgilerin sayısı
cizgi2B cizim[12];
cizgi2B nesneCizim[12];

/***********************************************************************************************************************************/
// 3d dönüşümü için genel değişkenleri ayarlar. İşlem yoluyla gönderilen tüm puanlar bu rakamlarla dönüştürülecektir.
void degerOlusturma(void)
{

  display.display();
  display.clearDisplay();   // ekran ve tampon bellegi temizler 
  
  // küp dönüşümü
  int xOut=0;
  int yOut=0;
  
  xOut = map(AcX,-17000,17000,-50,50);
  yOut = map(AcY,-17000,17000,-50,50);
  
  Xan+=xOut;
  Yan+=yOut;
  

  Yan=Yan % 360;
  Xan=Xan % 360; // taşmanın engellenmesi
  
  float Xan2,Yan2,Zan2;
  float s1,s2,s3,c1,c2,c3;
  
  Xan2 = Xan / donusumDeger; // dereceden radyana dönüşüm
  Yan2 = Yan / donusumDeger; // dereceden radyana dönüşüm
  
  // Zan değişkeni 0 olarak ayarlanmıştır
    
  s1 = sin(Yan2);
  s2 = sin(Xan2);
  
  c1 = cos(Yan2);
  c2 = cos(Xan2);

  xx = c1;
  xy = 0; 
  xz = -s1;

  yx = (s1 * s2);
  yy = c2;
  yz = (c1 * s2);

  zx = (s1 * c2);
  zy = -s2;
  zz = (c1 * c2);
}

/***********************************************************************************************************************************/
// degerOlusturma() içerisinden gelen değişkenler ile x1,y1,z1 işlenmesi ve sonuç olarak rx1,ry1 dönüştürülmesi
void cizgiOlusturma(struct cizgi2B *ret,struct cizgi3B vec)
{
  float zvt1;
  int xv1,yv1,zv1;

  float zvt2;
  int xv2,yv2,zv2;
  
  int rx1,ry1;
  int rx2,ry2;
 
  int x1;
  int y1;
  int z1;

  int x2;
  int y2;
  int z2;
  
  int Ok;
  
  x1=vec.p0.x;
  y1=vec.p0.y;
  z1=vec.p0.z;

  x2=vec.p1.x;
  y2=vec.p1.y;
  z2=vec.p1.z;

  Ok=0; 

  xv1 = (x1 * xx) + (y1 * xy) + (z1 * xz);
  yv1 = (x1 * yx) + (y1 * yy) + (z1 * yz);
  zv1 = (x1 * zx) + (y1 * zy) + (z1 * zz);

  zvt1 = zv1 - Zoff;


  if( zvt1 < -5){
    rx1 = 256 * (xv1 / zvt1) + Xoff;
    ry1 = 256 * (yv1 / zvt1) + Yoff;
    Ok=1; // birinci noktanın tamamlanması
  }
  
  
  xv2 = (x2 * xx) + (y2 * xy) + (z2 * xz);
  yv2 = (x2 * yx) + (y2 * yy) + (z2 * yz);
  zv2 = (x2 * zx) + (y2 * zy) + (z2 * zz);

  zvt2 = zv2 - Zoff;


  if( zvt2 < -5){
    rx2 = 256 * (xv2 / zvt2) + Xoff;
    ry2 = 256 * (yv2 / zvt2) + Yoff;
  } else
  {
    Ok=0;
  }
  
  if(Ok==1){
  ret->p0.x=rx1;
  ret->p0.y=ry1;

  ret->p1.x=rx2;
  ret->p1.y=ry2;
  }
}

/***********************************************************************************************************************************/
void goruntuOlusturma( void)
{
  for(int i=0; i<cizgiCizim ; i++)
  {
    nesneCizim[i]=cizim[i]; // eski satır segmentini saklar bu nedenle daha sonra silebiliriz.
    cizgiOlusturma(&cizim[i],cizgiler[i]); // 3B çizgi dilimlerini 2B'ye çevirir.
  } 

  // burada aslında OLED ile arabirim oluşturan tek kod var. Dolayısıyla farklı bir lib kullanırsanız, burada bunu değiştirebilirsiniz.

 for (int i=0; i<eskiCizgiCizim; i++ )
  {
	display.drawLine(nesneCizim[i].p0.x,nesneCizim[i].p0.y,nesneCizim[i].p1.x,nesneCizim[i].p1.y, BLACK); // erase the old cizgiler.
  }

    
  for (int i=0; i<cizgiCizim; i++ )
  {
	display.drawLine(cizim[i].p0.x,cizim[i].p0.y,cizim[i].p1.x,cizim[i].p1.y, WHITE);
  }
  eskiCizgiCizim=cizgiCizim;
  
  
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // register başlangıç adresi 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(true);
  Wire.requestFrom(MPU,14);  // toplamda 14 registerin okunması

  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

//OLED ekrana bilgi gönderilmesi  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(51,2);
  display.println("SENSOR DEGER");
  display.setCursor(2,40);
  display.println("Y");
  display.drawLine(5, 50, 5, 60, WHITE);
  display.setCursor(18,56);
  display.println("X");
  display.drawLine(5, 60, 15, 60, WHITE);
  display.setCursor(60,15);   
  display.print("AcX: ");
  display.println(AcX);	//ACC değerinin ekrana gönderilmesi
  display.setCursor(60,25);
  display.print("AcY: ");
  display.println(AcY);
  display.setCursor(60,35);
  display.print("GyX: ");
  display.println(GyX);  //gyro değerinin ekrana gönderilmesi
  display.setCursor(60,45);
  display.print("GyY: ");
  display.println(GyY);
    
  if(350>GyX && GyX> -350 && 350>GyY && GyY> -350){
	display.fillCircle(40, 60, 3, WHITE);
	display.setCursor(51,55);
	display.println("SENSOR SABIT");
  }
    delay(10);
}

/***********************************************************************************************************************************/
// ekranda oluşturulacak görsel için ekrana ön yükleme yapılır
void ilkCizim(){
 Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // OLED ekranın I2C adresini 0x3D olarak yükler (for the 128x64)
  display.clearDisplay();   // ekran ve tampon bellegi temizler

  Wire.begin();
  donusumDeger = 180 / 3.14159265358979323846264338327950; // dereceden radyana dönüşüm
  
  Xoff = 30; // küpün ekrandaki DİKEY konumu
  Yoff = 28; // küpün ekrandaki YATAY konumu
  Zoff = 750;   //küp büyüklüğü, değer büyüdükçe küp küçülür.

// Bir küp çizmek için çizgi parçaları. temelde p0 ila p1 arasındadır. p1 ila p2. p2'den p3'e kadar.

// ön yüz
  cizgiler[0].p0.x=-50;
  cizgiler[0].p0.y=-50;
  cizgiler[0].p0.z=50;
  cizgiler[0].p1.x=50;
  cizgiler[0].p1.y=-50;
  cizgiler[0].p1.z=50;

  cizgiler[1].p0.x=50;
  cizgiler[1].p0.y=-50;
  cizgiler[1].p0.z=50;
  cizgiler[1].p1.x=50;
  cizgiler[1].p1.y=50;
  cizgiler[1].p1.z=50;

  cizgiler[2].p0.x=50;
  cizgiler[2].p0.y=50;
  cizgiler[2].p0.z=50;
  cizgiler[2].p1.x=-50;
  cizgiler[2].p1.y=50;
  cizgiler[2].p1.z=50;

  cizgiler[3].p0.x=-50;
  cizgiler[3].p0.y=50;
  cizgiler[3].p0.z=50;
  cizgiler[3].p1.x=-50;
  cizgiler[3].p1.y=-50;
  cizgiler[3].p1.z=50;
// arka yüz
  cizgiler[4].p0.x=-50;
  cizgiler[4].p0.y=-50;
  cizgiler[4].p0.z=-50;
  cizgiler[4].p1.x=50;
  cizgiler[4].p1.y=-50;
  cizgiler[4].p1.z=-50;

  cizgiler[5].p0.x=50;
  cizgiler[5].p0.y=-50;
  cizgiler[5].p0.z=-50;
  cizgiler[5].p1.x=50;
  cizgiler[5].p1.y=50;
  cizgiler[5].p1.z=-50;

  cizgiler[6].p0.x=50;
  cizgiler[6].p0.y=50;
  cizgiler[6].p0.z=-50;
  cizgiler[6].p1.x=-50;
  cizgiler[6].p1.y=50;
  cizgiler[6].p1.z=-50;

  cizgiler[7].p0.x=-50;
  cizgiler[7].p0.y=50;
  cizgiler[7].p0.z=-50;
  cizgiler[7].p1.x=-50;
  cizgiler[7].p1.y=-50;
  cizgiler[7].p1.z=-50;
// kenardaki cizgiler
  cizgiler[8].p0.x=-50;
  cizgiler[8].p0.y=-50;
  cizgiler[8].p0.z=50;
  cizgiler[8].p1.x=-50;
  cizgiler[8].p1.y=-50;
  cizgiler[8].p1.z=-50;

  cizgiler[9].p0.x=50;
  cizgiler[9].p0.y=-50;
  cizgiler[9].p0.z=50;
  cizgiler[9].p1.x=50;
  cizgiler[9].p1.y=-50;
  cizgiler[9].p1.z=-50;

  cizgiler[10].p0.x=-50;
  cizgiler[10].p0.y=50;
  cizgiler[10].p0.z=50;
  cizgiler[10].p1.x=-50;
  cizgiler[10].p1.y=50;
  cizgiler[10].p1.z=-50;

  cizgiler[11].p0.x=50;
  cizgiler[11].p0.y=50;
  cizgiler[11].p0.z=50;
  cizgiler[11].p1.x=50;
  cizgiler[11].p1.y=50;
  cizgiler[11].p1.z=-50;

  cizgiCizim=12;
  eskiCizgiCizim=cizgiCizim;
  
  //  MPU yüklenmesi
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 registeri
  Wire.write(0);     // 0 değeri ata ve MPU-6050 uyandır
  Wire.endTransmission(true);

}