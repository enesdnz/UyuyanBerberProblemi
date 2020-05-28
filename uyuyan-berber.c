//ilgili kutuphanelerin import edilmesi
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h> 

#define MAX_MUSTERİ 15 // maksimum müsteri sayisini sabitliyoruz

//semaforlarin tanimlanmasi
sem_t beklemeKoltugu; 
sem_t berberKoltugu; 
sem_t uyuyanBerber; 
sem_t musteriBekleme;

//Musterilerin saci kesildiginde thread durdurulur.
int tamamlandi = 0; 


//fonksiyonlarin prototipleri
void *musteri(void *num); 
void *berber(void *); 
void beklemeSuresi(int saniye); 


int main() 
{
    pthread_t berberThread; 
    pthread_t musteriThread[MAX_MUSTERİ]; 
    int i, x, musteriSayisi, koltukSayisi; int Sayi[MAX_MUSTERİ]; 
    printf("Dükkana en fazla 15 kişi alabiliriz. Lütfen müşteri sayısını giriniz : \n");
    scanf("%d",&x);
    musteriSayisi = x; 
    printf("Lütfen dükkandaki koltuk sayısını giriniz : \n");
    scanf("%d",&x);
    koltukSayisi = x;
    // Girilen müsteri sayisi kontrolu yapiliyor eger fazlaysa hata donduruyor
    if (musteriSayisi > MAX_MUSTERİ) { 
       printf("Maksimum müşteri sayısı :  %d.\n", MAX_MUSTERİ); 
       return 0;
    } 
    for (i = 0; i < MAX_MUSTERİ; i++) { 
        Sayi[i] = i; 
    } 
    // Semaforlari baslangic degerleri ile baslatiyoruz
    sem_init(&beklemeKoltugu, 0, koltukSayisi); 
    sem_init(&berberKoltugu, 0, 1);
    sem_init(&uyuyanBerber, 0, 0); 
    sem_init(&musteriBekleme, 0, 0); 
    
    // Berberi olusturma 
    pthread_create(&berberThread, NULL, berber, NULL); 
    
    // Musteri olusturma 
    for (i = 0; i < musteriSayisi; i++) {
        pthread_create(&musteriThread[i], NULL, musteri, (void *)&Sayi[i]); 
    } 
    // threadleri birlestirme islemi
    for (i = 0; i < musteriSayisi; i++) { 
        pthread_join(musteriThread[i],NULL); 
    } 
    // Müsterilerin sac kesme islemi bitince berber threadini olduruyoruz
    tamamlandi = 1; 
    sem_post(&uyuyanBerber); // Berberi dukkani kapatmasi icin uyandiriyoruz
    pthread_join(berberThread,NULL); 
    return 0;
} 

void *musteri(void *sayi) 
{ 
  int numara = *(int *)sayi; 
  
  printf("Müşteri %d dükkana gitmek için yola koyuluyor..\n", numara); 
  beklemeSuresi(5); 
  
  printf("Müşteri %d dükkana geldi..\n", numara); 
  sem_wait(&beklemeKoltugu); // Bekleme koltuklarinin bosalmasini bekliyor
  
  printf("Müşteri %d bekleme koltuğuna geçiyor..\n", numara); 
  sem_wait(&berberKoltugu);  // Siranin musteriye gelmesi icin bekliyor
  sem_post(&beklemeKoltugu); // Berberi uyandirma semaforu
  
  printf("Müşteri %d berberi uyandırıyor..\n", numara); 
  sem_post(&uyuyanBerber); // Berberin saci kesmesi icin bekleniyor 
  sem_wait(&musteriBekleme); 
  sem_post(&berberKoltugu); 
  printf("Müşteri %d dükkandan ayrılıyor..\n", numara); 
}

void *berber(void *sacKesimi){  
  while (!tamamlandi)
  { 
    // Musteri gelene kadar berber uyuyor.
    printf("Berber uyuyor..\n"); 
    sem_wait(&uyuyanBerber); 
    if (!tamamlandi) 
    { 
     // Musteri saci kesilmesi icin sure ayriliyor.
     printf("Berber şu anda saç kesiyor..\n"); 
     beklemeSuresi(3);
     printf("Saç kesim işlemi tamamlandı..\n");
     // Sac kesildiginde ilgili musteri ile bag kesilir.
     sem_post(&musteriBekleme); 
    } 
    else
    { 
         printf("Dükkan kapatıldı, berber eve gidiyor.\n"); 
    } 
   }
}

void beklemeSuresi(int saniye){ 
     int sure = 1; 
     sleep(sure); 
}