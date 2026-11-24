CPU Süreç Zamanlayıcı Simülasyonu

Algoritma ve Programlama dersi grup ödevi. C dilinde yazılmış bir CPU zamanlayıcı (scheduler) simülasyonudur. Aynı süreç listesini üç algoritmayla çalıştırır ve sonuçları karşılaştırır:

FCFS (First Come First Served)
SJF (Shortest Job First, kesintisiz)
Round Robin

Program gerçek süreç çalıştırmaz; zamanı bir sayaçla ilerleterek her şeyi hesaplar.

Özellikler
Her algoritma için Gantt şeması ([P1 0-5], boşta kalma için [BOS 22-25]), her satırda en fazla 6 blok
Süreç numarasına göre sıralı sonuç tablosu: geliş, CPU, bitiş, dönüş ve bekleme süreleri
Ortalama bekleme süresi, ortalama dönüş süresi ve bağlam değişimi (context switch) sayısı
Sonda üç algoritmanın karşılaştırması (eşitlikte sıra: FCFS, SJF, RR)
Aynı girdi için her zaman aynı çıktı (eşitlik kuralları sabittir)
Ödev Kurallarına Uyum
Yalnızca stdio.h kullanılmıştır
struct, malloc ve free kullanılmamıştır
Veriler paralel dizilerde tutulur (gelis[], cpu[], bitis[]; aynı indeks aynı süreç)
Global değişkenler yalnızca Gantt şeması ve sayaçlar içindir; her algoritmadan önce sifirla() ile sıfırlanır
Her fonksiyonun üstünde, yazanın adı ve fonksiyonun görevi yazılıdır
Derleme ve Çalıştırma

Visual Studio: dosyayı .c uzantısıyla projeye ekleyin ve Ctrl + F5 ile çalıştırın.

gcc:

gcc -Wall -o scheduler scheduler.c
./scheduler
Kullanım

Program girdileri sırayla ister: süreç sayısı (en fazla 20), her sürecin geliş zamanı ve CPU süresi, en sonda zaman dilimi (quantum).

Örnek girdi
Surec sayisi (1-20): 5
P1 gelis zamani ve CPU suresi: 0 5
P2 gelis zamani ve CPU suresi: 1 3
P3 gelis zamani ve CPU suresi: 2 8
P4 gelis zamani ve CPU suresi: 3 6
P5 gelis zamani ve CPU suresi: 25 2
Zaman dilimi (quantum): 4
Örnek çıktı (özet)
FCFS:        [P1 0-5] [P2 5-8] [P3 8-16] [P4 16-22] [BOS 22-25] [P5 25-27]
SJF:         [P1 0-5] [P2 5-8] [P4 8-14] [P3 14-22] [BOS 22-25] [P5 25-27]
Round Robin: [P1 0-4] [P2 4-7] [P3 7-11] [P4 11-15] [P1 15-16] [P3 16-20]
             [P4 20-22] [BOS 22-25] [P5 25-27]

FCFS: 4.60   SJF: 4.20   Round Robin: 7.40
En dusuk ortalama bekleme: SJF (4.20)
Algoritma	Ort. bekleme	Ort. dönüş	Bağlam değişimi
FCFS	4.60	9.40	4
SJF	4.20	9.00	4
Round Robin	7.40	12.20	7
Algoritma Kuralları
FCFS: geliş zamanına göre sıralanır, eşitse küçük numara önce. Kesintisiz çalışır.
SJF: CPU boşaldığında gelmiş sürecler arasından en kısa CPU süreli seçilir. Eşitlikte erken geliş, o da eşitse küçük numara.
Round Robin: P1…Pn dairesel dolaşılır, her süreç bir seferde en fazla quantum kadar çalışır. Bir turda hiç süreç çalışmazsa boşta kuralı uygulanır ve yeni tur P1'den başlar.
Boşta kuralı: CPU boş kalırsa saat, bitmemiş süreçlerin en erken geliş zamanına ilerler ve [BOS a-b] bloğu eklenir.
Bağlam değişimi: eklenen süreç bloğu, önceki son süreç bloğundan farklı bir sürece aitse sayaç artar. İlk süreç ve boşta blokları sayılmaz.
Fonksiyonlar
Fonksiyon	Görevi
sifirla	Global değişkenleri ve Gantt sayaçlarını sıfırlar
ganttEkle	Gantt'a blok ekler, bağlam değişimini sayar
ganttYaz	Gantt şemasını yazar
sureleriHesapla	Dönüş ve bekleme sürelerini hesaplar
ortalamalar	Ortalamaları pointer parametrelerle döndürür
tabloYaz	Sonuç tablosunu yazar
enErkenGelis	Bitmemiş süreçlerin en erken geliş zamanını bulur
fcfs, sjf, roundRobin	Zamanlama algoritmaları
raporYaz, karsilastir, main	Sonuçları yazar, karşılaştırır, programı yönetir
toplamBlok	static yerel değişken örneği (scope: fonksiyon, lifetime: tüm program)

