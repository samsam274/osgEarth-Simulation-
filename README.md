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

<img width="1918" height="972" alt="Ekran görüntüsü 2026-08-27 170431" src="https://github.com/user-attachments/assets/3839f412-0c99-4e5b-b443-1b2b202d15ba" /><br>

<img width="1918" height="982" alt="Ekran görüntüsü 2026-08-27 170319" src="https://github.com/user-attachments/assets/318c5108-2552-49d9-824c-2cba34c81cee" /><br>

<img width="1918" height="977" alt="Ekran görüntüsü 2026-08-27 170659" src="https://github.com/user-attachments/assets/e2389f62-ca5a-4e9f-9837-967b4841a22f" /><br>

<img width="566" height="840" alt="Ekran görüntüsü 2026-08-27 172229" src="https://github.com/user-attachments/assets/6058ccc0-3a85-4732-abca-4f8f8248016f" /><br>

<img width="562" height="842" alt="Ekran görüntüsü 2026-08-27 172255" src="https://github.com/user-attachments/assets/10f5bac8-ca4d-4793-b0b5-c836e40aaa13" /><br>











