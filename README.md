# 3D Visualization Tool 

C++ ve osgEarth altyapısı kullanılarak geliştirilmiş, Imgui arayüzü ile kontrol edilebilen 3D dünya ve veri görselleştirme uygulaması. 

**3D Dünya Görselleştirme:** osgEarth altyapısıyla, yüksek çözünürlüklü küresel arazi ve harita katmanı yönetimi.<br>
**Kontrol Paneli:** Dear Imgui entergrasyonu ile yatay/dikey kontrol panelleri, kamera modları, veri görselleştirme alanları.<br>
**Modüler Mimari:**  Modüler C++ kod yapısı ve performans odaklı işleme (rendering) döngüsü.<br>

## Bağımlılıklar 

Projenin derlenebilmesi için aşağıdaki kütüphanelerin yüklü olması gerekir:

 **C++ Compiler:** C++17 veya üzeri destekli (MSVC / GCC / Clang)<br>
 **CMake:** Minimum v3.15<br>
 **OpenSceneGraph (OSG):** v3.6.<br>
 **osgEarth:** v3.x<br>
 **Dear ImGui:** Entegre edilmiş / dahili<br>
 **OpenGL:** 3.3+<br>

**Not:** Bağımlılıkları kolay yönetmek için "vcpkg" kullanılması tavsiye edilir.

## Derleme ve Kurulum 

### 1. Depoyu Klonlayın

```bash

git clone [https://github.com/samsam274/osgEarth-Simulation-.git](https://github.com/samsam274/osgEarth-Simulation-.git)
cd osgEarth-Simulation-

```
### 2. CMake ile Derleyin 

```bash

mkdir build
cd build
cmake ..
cmake --build . --config Release

```

(vcpkg kullanıyorsanız):

```bash

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/Projects/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
#Not: Kendi sisteminizdeki vcpkg.cmake dosyasının konumuna göre -DCMAKE_TOOLCHAIN_FILE yolunu güncellemeyi unutmayın.

```
<img width="1918" height="982" alt="Ekran görüntüsü 2026-08-27 170130" src="https://github.com/user-attachments/assets/6856cd7d-6212-495d-9be1-c60b72f5b094" /><br>

### 1. İHA Rota Simülasyonu ve Uçuş Hattı<br>
3D Rota Çizgisi: Belirlenen Waypoint noktaları arasında kırmızı uçuş hattı (polyline) çizilerek İHA'nın izleyeceği rota 3 boyutlu uzayda gösterilir.<br>
İHA Vektör Simgesi: Kırmızı ok/koni nesnesi, İHA'nın anlık konumunu ve yönelim açısını (heading) temsil eder.<br>


<img width="1918" height="972" alt="Ekran görüntüsü 2026-08-27 170431" src="https://github.com/user-attachments/assets/3839f412-0c99-4e5b-b443-1b2b202d15ba" /><br>

### 2. Harita Altlıkları ve Katman Çeşitliliği<br>
Vektörel Sokak Haritası (OpenStreetMap): Ülke sınırları, şehir isimleri ve sokak detaylarını içeren 2D vektörel katman görünümüdür.<br>
Sokak Haritası Yakın Çekim: Yüksek zoom seviyelerinde arazi çizgileri ve yerleşim yeri metinlerinin net olarak işlendiğini gösterir.<br>
Uydu Kaplaması (ArcGIS World Imagery): Gerçek zamanlı uydu fotoğrafları kullanılarak arazinin doğal dokusu küre üzerine giydirilir.<br>


<img width="1918" height="982" alt="Ekran görüntüsü 2026-08-27 170319" src="https://github.com/user-attachments/assets/318c5108-2552-49d9-824c-2cba34c81cee" /><br>

### 3. Temel Dünya Görünümü ve Gece/Gündüz Aydınlatması<br>
Küresel (Geocentric) Görünüm: Uygulama açılışında dünya, WGS84 koordinat sistemine uygun olarak uzay ortamında 3 boyutlu bir küre biçiminde oluşturulur.<br>
Uzay ve Atmosfer Arka Planı: Harita etrafında dinamik yıldız arka planı ve güneş açısına bağlı atmosferik ışıklandırma yer alır.<br>


<img width="1918" height="977" alt="Ekran görüntüsü 2026-08-27 170659" src="https://github.com/user-attachments/assets/e2389f62-ca5a-4e9f-9837-967b4841a22f" /><br>

### 4. 3D Yükseklik Verisi ve Arazi Modeli (Elevation/DEM)<br>
Topografik Yükseklik (SRTM): Yükseklik verileri (Elevation Layers) aktif edildiğinde düz harita görüntüsü gerçek topografik dağ ve vadi yapılarına dönüşür.<br>
Yüzey Detayı: Araziye yakınlaşıldığında yüksek çözünürlüklü engebe, çukur ve tepe yükseklik sınırları render edilir. Elevation Layer ayarlarını iki harita tipinde de görebilmekteyiz.<br>


<img width="566" height="840" alt="Ekran görüntüsü 2026-08-27 172229" src="https://github.com/user-attachments/assets/6058ccc0-3a85-4732-abca-4f8f8248016f" /><br>

## İHA Kontrol & Katman Paneli (ImGui Arayüz Detayları)<br>
### 1. Telemetri Bölümü (Canlı Veri Ekranı)<br>
Enlem (Lat) & Boylam (Lon): İHA'nın dünya üzerindeki anlık WGS84 coğrafi koordinatlarını derece cinsinden canlı gösterir.<br>
Yukseklik: İHA'nın deniz seviyesinden anlık yüksekliğini ($m$) belirtir.<br>
Yon (Heading): İHA'nın burnunun baktığı yön açısını ($0^\circ - 360^\circ$) ifade eder.<br>
Hedef: İHA'nın şu an rotadaki kaçıncı hedef noktasına (Waypoint) doğru ilerlediğini gösterir (Örn: 1 / 4).<br>
Durum: Simülasyonun anlık modunu gösterir (BEKLEMEDE veya UÇUŞTA).<br>

### 2. Ucus Kontrolu Bölümü<br>
Baslat (Buton): Rota oluşturulmuşsa İHA simülasyonunu başlatır; hava aracı ilk waypoint'ten itibaren hareket etmeye başlar.<br>
Sifirla (Buton): Simülasyonu durdurur ve İHA'yı rotanın en başındaki başlangıç konumuna geri getirir.<br>
Hiz (km/h) (Slider): İHA'nın hat üzerindeki ilerleme hızını canlı olarak ayarlar.<br>
Hedef irtifa (m) (Slider): İHA'nın uçuş yüksekliğini dinamik olarak değiştirir.<br>
Koni boyutu (m) (Slider): 3D haritada İHA'yı temsil eden kırmızı ok/koni nesnesinin ekrandaki fiziksel boyutunu büyütüp küçültür.<br>
### 3. Rota / Waypoint Bölümü<br>
Enlem / Boylam / Irtifa (Girdi Kutuları & +/- Butonları): Eklenecek yeni hedef noktasının koordinatlarını manuel olarak girmenize veya butonlarla hassas ayarlamanıza olanak tanır.<br>
Waypoint Ekle (Buton): Girdi kutularına yazılan koordinatı aşağıdaki mevcut waypoint listesinin sonuna ekler.<br>
Konumu Al (Buton): Kameranın 3D haritada tam olarak baktığı orta noktanın koordinatlarını girdi kutularına otomatik doldurur.<br>
Rotayi Olustur (Buton): Listede biriken tüm waypoint noktalarını birleştirerek 3D harita üzerinde kırmızı renkli uçuş çizgisini (polyline) çizer.<br>
Rotayi Temizle (Buton): Çizilen tüm uçuş hattını ve listedeki bütün hedef noktalarını sıfırlar.<br>
Sil (Liste Butonları): Her waypoint maddesinin yanında bulunur; sadece o belirli hedef noktasını listeden çıkarmaya yarar.<br>

<img width="562" height="842" alt="Ekran görüntüsü 2026-08-27 172255" src="https://github.com/user-attachments/assets/10f5bac8-ca4d-4793-b0b5-c836e40aaa13" /><br>

### 4. Harita Katmanlari Bölümü<br>
OpenStreetMap (Onay Kutusu / Checkbox): Vektörel sokak ve şehir haritası katmanını açıp kapatır.<br>
World_Imagery (Onay Kutusu / Checkbox): ArcGIS renkli uydu fotoğrafı katmanını açıp kapatır.<br>
Baskinlik (Slider'lar): Katmanların şeffaflık ($0.00 - 1.00$) oranını ayarlar. İki katman da açıkken slider'lar kaydırılarak uydu görüntüsü ile sokak haritası birbiri üzerine transparan şekilde giydirilebilir.<br>
### 5. Yukseklik (Elevation) Bölümü<br>
SRTM N39E032 vb. (Onay Kutuları): Yükseklik verisini (DEM/SRTM karolarını) aktif veya pasif eder. Seçildiğinde düz harita görüntüsü gerçekğinki gibi dağ, vadi ve tepe engebelerine kavuşur.<br>
### 6. Gunes: Mevsim ve Saat Bölümü<br>
Mevsim (Açılır Menü / Dropdown): Yaz, Kış, İlkbahar veya Sonbahar tarihlerini seçerek güneşin mevsimsel açısını ayarlar.<br>
Ay / Gun (Sayısal Kutular): Özel bir simülasyon tarihi belirlemenizi sağlar.<br>
15.00 saat (Slider): Günün saatini ($00.00 - 24.00$) kaydırarak güneşin gökyüzündeki konumunu ve gölge açılarını canlı olarak değiştirir.<br>
Safak / Ogle / Gun batimi / Gece (Hızlı Butonlar): Saati doğrudan ilgili zaman dilimine (Örn: Gün batımı için saat 19:00'a) tek tıkla sabitler.<br>
Saati otomatik ilerlet (Onay Kutusu): Zamanın simülasyon boyunca gerçek zamanlı olarak akmasını ve gölgelerin hareket etmesini sağlar.<br>
Gece aydınligi (Slider): Gece moduna geçildiğinde zifiri karanlık olmaması için ortamın minimum ortam ışığını (Ambient light) ayarlar.<br>













