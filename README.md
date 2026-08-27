# 3D Visualization Tool 

C++ ve osgEarth altyapısı kullanılarak geliştirilmiş, Imgui arayüzü ile kontrol edilebilen 3D dünya ve veri görselleştirme uygulaması. 

**3D Dünya Görselleştirme:** osgEarth altyapısıyla, yüksek çözünürlüklü küresel arazi ve harita katmanı yönetimi. 
**Kontrol Paneli:** Dear Imgui entergrasyonu ile yatay/dikey kontrol panelleri, kamera modları, veri görselleştirme alanları.
**Modüler Mimari:**  Modüler C++ kod yapısı ve performans odaklı işleme (rendering) döngüsü.

## Bağımlılıklar 

Projenin derlenebilmesi için aşağıdaki kütüphanelerin yüklü olması gerekir:

 **C++ Compiler:** C++17 veya üzeri destekli (MSVC / GCC / Clang)
 **CMake:** Minimum v3.15
 **OpenSceneGraph (OSG):** v3.6.x
 **osgEarth:** v3.x
 **Dear ImGui:** Entegre edilmiş / dahili
 **OpenGL:** 3.3+

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

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:\Projects\vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release

```
<img width="1918" height="982" alt="Ekran görüntüsü 2026-08-27 170130" src="https://github.com/user-attachments/assets/6856cd7d-6212-495d-9be1-c60b72f5b094" />
<img width="1918" height="972" alt="Ekran görüntüsü 2026-08-27 170431" src="https://github.com/user-attachments/assets/3839f412-0c99-4e5b-b443-1b2b202d15ba" />
<img width="1918" height="982" alt="Ekran görüntüsü 2026-08-27 170319" src="https://github.com/user-attachments/assets/318c5108-2552-49d9-824c-2cba34c81cee" />
<img width="1918" height="977" alt="Ekran görüntüsü 2026-08-27 170659" src="https://github.com/user-attachments/assets/e2389f62-ca5a-4e9f-9837-967b4841a22f" />
<img width="1918" height="982" alt="Ekran görüntüsü 2026-08-27 170614" src="https://github.com/user-attachments/assets/58473468-6341-4bb8-9000-2ef21e37c99c" />
<img width="566" height="840" alt="Ekran görüntüsü 2026-08-27 172229" src="https://github.com/user-attachments/assets/1298ac55-a232-45ce-8fd6-de50947c4c81" />






<img width="677" height="901" alt="Ekran görüntüsü 2026-08-27 171050" src="https://github.com/user-attachments/assets/738b214f-8992-4c63-9684-3c52461d580b" />





