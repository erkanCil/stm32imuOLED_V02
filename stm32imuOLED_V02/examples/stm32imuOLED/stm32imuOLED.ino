 /*******************************************************************************
* @dosya stm32imuOLED.ino
* @yazar Erkan CIL
* @sürüm V0.0.1
* @tarih 23-Aralık-2017
* @özet IMU senrörü üzerinden gelen değerlerin OLED ekranda gösterilmesi, görselin döndürülmesi
kodun orjinali http://forum.freetronics.com/viewtopic.php?f=37&t=5495
******************************************************************************
*
* Bu program özgür yazılımdır: Özgür Yazılım Vakfı tarafından yayımlanan GNU
* Genel Kamu Lisansı’nın sürüm 3 ya da (isteğinize bağlı olarak) daha sonraki
* sürümlerinin hükümleri altında yeniden dağıtabilir ve/veya değiştirebilirsiniz.
*
*******************************************************************************/

/* Eklentiler ------------------------------------------------------------------*/ 
#include <stm32imuOLED.h>

/* Ayar Fonksyonu---------------------------------------------------------------*/
void setup()
{
// buraya yazdığınız ayarlama kodları, sadece bir kez çalıştırılacaktır:
	ilkCizim(); //OLED ekranın ve IMU sensörünün hazırlanması
}
/***********************************************************************************************************************************/

/* Sonsuz Döngü Fonksyonu-------------------------------------------------------*/
void loop()
{
// buraya yazdığınız döngü kodları, sonsuz döngü olarak sürekli çalıştırılacaktır:
  degerOlusturma(); //Sensörden IMU değerlerinin okunması dönüştürlmesi.
  goruntuOlusturma(); // OLED ekrana görüntünün oluşturulması
}


