/*
 * Algoritma ve Programlama - Grup Odevi
 * CPU Surec Zamanlayici Simulasyonu (FCFS, SJF, Round Robin)
 * Sadece stdio.h kullanilir. struct / malloc / free yoktur.
 *
 * Grup uyesi: Sara Yasser Hammed Helmmy
 */
#define _CRT_SECURE_NO_WARNINGS   /* Visual Studio scanf uyarisini kapatir */
#include <stdio.h>

#define MAXP 20          /* en fazla surec sayisi            */
#define MAXG 500         /* Gantt'ta en fazla blok sayisi    */
#define BLOK_SATIR 6     /* Gantt'ta bir satirdaki blok sayisi */

 /* ---- Global degiskenler (Gantt semasi ve sayaclar) ----
  * Bu degiskenlere birden cok fonksiyon ortak ulasir; bu yuzden global.
  * Her algoritmadan once sifirla() ile sifirlanir.                      */
int ganttId[MAXG];       /* blogun surec numarasi, 0 = BOS (bosta)  */
int ganttBas[MAXG];      /* blogun baslangic zamani                 */
int ganttBit[MAXG];      /* blogun bitis zamani                     */
int ganttSayac = 0;      /* Gantt'taki blok sayisi                  */
int ctxSayac = 0;        /* baglam degisimi sayisi                  */
int sonSurec = 0;        /* Gantt'a eklenen son SUREC blogunun no'su */

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: static yerel degisken ornegi. Programin basindan beri
 * Gantt'a eklenen TOPLAM blok sayisini tutar (3 algoritma boyunca).
 * artir=1 ise sayaci 1 arttirir; her durumda guncel degeri dondurur.
 * 'toplam' sadece bu fonksiyonun icinde gorunur (scope: yerel) ama
 * program bitene kadar yasar ve sifirlanmaz (lifetime: tum program).
 * ------------------------------------------------------------------ */
int toplamBlok(int artir)
{
    static int toplam = 0;
    if (artir)
        toplam++;
    return toplam;
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Her algoritmadan once global degiskenleri ve Gantt
 * sayaclarini sifirlar.
 * ------------------------------------------------------------------ */
void sifirla(void)
{
    int i;
    for (i = 0; i < MAXG; i++) {
        ganttId[i] = 0;
        ganttBas[i] = 0;
        ganttBit[i] = 0;
    }
    ganttSayac = 0;
    ctxSayac = 0;
    sonSurec = 0;
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Gantt'a bir blok ekler (id=0 ise BOS blogu). Surec blogu,
 * kendinden onceki son surec blogundan farkliysa baglam degisimini
 * 1 arttirir. Ilk surec ve BOS bloklari sayilmaz.
 * ------------------------------------------------------------------ */
void ganttEkle(int id, int bas, int bit)
{
    if (ganttSayac >= MAXG)
        return;
    ganttId[ganttSayac] = id;
    ganttBas[ganttSayac] = bas;
    ganttBit[ganttSayac] = bit;
    ganttSayac++;
    toplamBlok(1);
    if (id != 0) {
        if (sonSurec != 0 && sonSurec != id)
            ctxSayac++;
        sonSurec = id;
    }
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Gantt semasini yazar. Her satirda en fazla BLOK_SATIR blok.
 * ------------------------------------------------------------------ */
void ganttYaz(void)
{
    int i;
    for (i = 0; i < ganttSayac; i++) {
        if (ganttId[i] == 0)
            printf("[BOS %d-%d] ", ganttBas[i], ganttBit[i]);
        else
            printf("[P%d %d-%d] ", ganttId[i], ganttBas[i], ganttBit[i]);
        if ((i + 1) % BLOK_SATIR == 0 || i == ganttSayac - 1)
            printf("\n");
    }
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Her surec icin donus = bitis - gelis ve
 * bekleme = donus - cpu hesaplar. Diziler adresle gecer, bu yuzden
 * donus[] ve bekleme[] dizileri cagiran fonksiyonda dolmus olur.
 * ------------------------------------------------------------------ */
void sureleriHesapla(int gelis[], int cpu[], int bitis[],
    int donus[], int bekleme[], int n)
{
    int i;
    for (i = 0; i < n; i++) {
        donus[i] = bitis[i] - gelis[i];
        bekleme[i] = donus[i] - cpu[i];
    }
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Ortalama bekleme ve ortalama donus suresini hesaplar.
 * Iki sonuc dondurmek gerektigi icin (return tek deger dondurur)
 * sonuclar pointer parametrelerle (*ortBek, *ortDon) cagirana verilir.
 * ------------------------------------------------------------------ */
void ortalamalar(int bekleme[], int donus[], int n,
    double* ortBek, double* ortDon)
{
    int i;
    int topBek = 0, topDon = 0;
    for (i = 0; i < n; i++) {
        topBek += bekleme[i];
        topDon += donus[i];
    }
    *ortBek = (double)topBek / n;
    *ortDon = (double)topDon / n;
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Sonuc tablosunu surec numarasi sirasiyla yazar.
 * ------------------------------------------------------------------ */
void tabloYaz(int gelis[], int cpu[], int bitis[],
    int donus[], int bekleme[], int n)
{
    int i;
    printf("No\tGelis\tCPU\tBitis\tDonus\tBekleme\n");
    for (i = 0; i < n; i++)
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
            i + 1, gelis[i], cpu[i], bitis[i], donus[i], bekleme[i]);
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Bitmemis (bitti[i]==0) sureclerin en erken geliş zamanini
 * dondurur. Boşta kuralinda saati ilerletmek icin kullanilir.
 * ------------------------------------------------------------------ */
int enErkenGelis(int gelis[], int bitti[], int n)
{
    int i, en = -1;
    for (i = 0; i < n; i++)
        if (!bitti[i] && (en == -1 || gelis[i] < en))
            en = gelis[i];
    return en;
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy  (FCFS)
 * Ne yapar: FCFS. Surecleri geliş zamanina gore (esitse numarasi
 * kucuk olan once) siralar ve kesintisiz calistirir. CPU bos kalirsa
 * BOS blogu ekler. Bitis zamanlarini bitis[] dizisine yazar.
 * ------------------------------------------------------------------ */
void fcfs(int gelis[], int cpu[], int bitis[], int n)
{
    int sira[MAXP];
    int i, j, tmp, t = 0;

    for (i = 0; i < n; i++)
        sira[i] = i;
    /* secmeli siralama: gelis, esitse indeks (numara) */
    for (i = 0; i < n - 1; i++)
        for (j = i + 1; j < n; j++)
            if (gelis[sira[j]] < gelis[sira[i]] ||
                (gelis[sira[j]] == gelis[sira[i]] && sira[j] < sira[i])) {
                tmp = sira[i];
                sira[i] = sira[j];
                sira[j] = tmp;
            }

    for (i = 0; i < n; i++) {
        int p = sira[i];
        if (t < gelis[p]) {
            ganttEkle(0, t, gelis[p]);
            t = gelis[p];
        }
        ganttEkle(p + 1, t, t + cpu[p]);
        t += cpu[p];
        bitis[p] = t;
    }
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy  (SJF)
 * Ne yapar: Kesintisiz SJF. CPU bosaldiginda gelmis ve bitmemis
 * sureclerden CPU suresi en kisa olani secer (esitlikte gelis erken,
 * o da esitse numara kucuk). Hic surec yoksa saati ilerletip BOS ekler.
 * ------------------------------------------------------------------ */
void sjf(int gelis[], int cpu[], int bitis[], int n)
{
    int bitti[MAXP];
    int i, secilen, biten = 0, t = 0;

    for (i = 0; i < n; i++)
        bitti[i] = 0;

    while (biten < n) {
        secilen = -1;
        for (i = 0; i < n; i++) {
            if (bitti[i] || gelis[i] > t)
                continue;
            if (secilen == -1 || cpu[i] < cpu[secilen] ||
                (cpu[i] == cpu[secilen] && gelis[i] < gelis[secilen]))
                secilen = i;   /* tam esitlikte i buyuk oldugu icin degismez */
        }
        if (secilen == -1) {
            int yeni = enErkenGelis(gelis, bitti, n);
            ganttEkle(0, t, yeni);
            t = yeni;
        }
        else {
            ganttEkle(secilen + 1, t, t + cpu[secilen]);
            t += cpu[secilen];
            bitis[secilen] = t;
            bitti[secilen] = 1;
            biten++;
        }
    }
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy  (Round Robin)
 * Ne yapar: Round Robin. P1..Pn dairesel dolasilir; sirasi gelen surec
 * gelmis ve bitmemisse en fazla 'q' kadar calisir. Bir turda hic surec
 * calismadiysa BOS blogu eklenir ve yeni tur P1'den baslar.
 * kalan[] yerel dizisi orijinal cpu[] dizisini bozmamak icindir.
 * ------------------------------------------------------------------ */
void roundRobin(int gelis[], int cpu[], int bitis[], int n, int q)
{
    int kalan[MAXP], bitti[MAXP];
    int i, sure, biten = 0, t = 0, calisti;

    for (i = 0; i < n; i++) {
        kalan[i] = cpu[i];
        bitti[i] = 0;
    }

    while (biten < n) {
        calisti = 0;
        for (i = 0; i < n; i++) {
            if (bitti[i] || gelis[i] > t)
                continue;
            sure = (kalan[i] < q) ? kalan[i] : q;
            ganttEkle(i + 1, t, t + sure);
            t += sure;
            kalan[i] -= sure;
            calisti = 1;
            if (kalan[i] == 0) {
                bitis[i] = t;
                bitti[i] = 1;
                biten++;
            }
        }
        if (!calisti) {
            int yeni = enErkenGelis(gelis, bitti, n);
            ganttEkle(0, t, yeni);
            t = yeni;
        }
    }
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Bir algoritmanin sonucunu yazar: Gantt, tablo, ozet.
 * Ortalama bekleme suresini pointer ile (*ortBekleme) main'e verir.
 * ------------------------------------------------------------------ */
void raporYaz(const char ad[], int gelis[], int cpu[], int bitis[], int n,
    double* ortBekleme)
{
    int donus[MAXP], bekleme[MAXP];
    double ortDon;

    sureleriHesapla(gelis, cpu, bitis, donus, bekleme, n);
    ortalamalar(bekleme, donus, n, ortBekleme, &ortDon);

    printf("\n===== %s =====\n", ad);
    printf("Gantt:\n");
    ganttYaz();
    printf("\n");
    tabloYaz(gelis, cpu, bitis, donus, bekleme, n);
    printf("\nOrtalama bekleme: %.2f\n", *ortBekleme);
    printf("Ortalama donus  : %.2f\n", ortDon);
    printf("Baglam degisimi : %d\n", ctxSayac);
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Uc algoritmanin ortalama bekleme surelerini yan yana
 * yazar ve en dusugunu belirtir. Esitlikte FCFS, SJF, RR sirasi.
 * ------------------------------------------------------------------ */
void karsilastir(double f, double s, double r)
{
    const char* en = "FCFS";
    double min = f;

    if (s < min) { min = s; en = "SJF"; }
    if (r < min) { min = r; en = "Round Robin"; }

    printf("\n===== KARSILASTIRMA =====\n");
    printf("FCFS: %.2f   SJF: %.2f   Round Robin: %.2f\n", f, s, r);
    printf("En dusuk ortalama bekleme: %s (%.2f)\n", en, min);
}

/* ------------------------------------------------------------------
 * Yazan: Sara Yasser Hammed Helmmy
 * Ne yapar: Sureclerin verisini ve zaman dilimini okur, uc algoritmayi
 * ayni veriyle calistirir ve sonunda karsilastirir.
 * ------------------------------------------------------------------ */
int main(void)
{
    /* Paralel diziler: ayni indeks ayni surece aittir (i -> P(i+1)) */
    int gelis[MAXP], cpu[MAXP], bitis[MAXP];
    int n, q, i;
    double bekF, bekS, bekR;

    printf("Surec sayisi (1-%d): ", MAXP);
    if (scanf("%d", &n) != 1)
        return 1;
    for (i = 0; i < n; i++) {
        printf("P%d gelis zamani ve CPU suresi: ", i + 1);
        if (scanf("%d %d", &gelis[i], &cpu[i]) != 2)
            return 1;
    }
    printf("Zaman dilimi (quantum): ");
    if (scanf("%d", &q) != 1)
        return 1;

    sifirla();
    fcfs(gelis, cpu, bitis, n);
    raporYaz("FCFS", gelis, cpu, bitis, n, &bekF);

    sifirla();
    sjf(gelis, cpu, bitis, n);
    raporYaz("SJF", gelis, cpu, bitis, n, &bekS);

    sifirla();
    roundRobin(gelis, cpu, bitis, n, q);
    raporYaz("Round Robin", gelis, cpu, bitis, n, &bekR);

    karsilastir(bekF, bekS, bekR);
    printf("\n(Programin basindan beri eklenen toplam blok: %d)\n",
        toplamBlok(0));
    return 0;
}
