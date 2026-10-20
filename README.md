# Praktikum Struktur Data C++ — Pertemuan 6

## Study Case: Antrean Pencetakan "Cetak Bersama"

Repository ini dibuat otomatis oleh aplikasi praktikum. Setiap kali Anda
melakukan **push**, GitHub Actions akan mengompilasi kode Anda, menjalankan
test, dan mengirim nilainya ke aplikasi.

---

# Study Case

Pertemuan ini hanya punya **satu soal**, yaitu study case di bawah ini. Tiga
pekerjaan yang Anda kerjakan semuanya berasal dari cerita yang sama — tidak ada
cerita baru lagi sesudahnya.

## Latar

Lab komputer kampus punya **satu** printer yang dipakai bersama oleh puluhan
orang. Anda diminta membuat bagian dalam perangkat lunak pengelola
pencetakannya, yang namanya **Cetak Bersama**.

Tampilannya sudah jadi. Orang sudah bisa mengirim dokumen, dan papan di layar
sudah bisa menampilkan daftarnya. Yang belum ada justru bagian intinya: siapa
yang dicetak lebih dulu, dan bagaimana dokumen keluar dari daftar.

## Fitur pertama — antrean cetak

Printernya hanya satu dan hanya bisa mencetak satu dokumen pada satu waktu.
Padahal dokumen bisa masuk dari banyak orang sekaligus. Jadi harus ada aturan:
siapa yang dicetak lebih dulu?

Coba bayangkan kalau printernya mengambil dokumen yang **paling baru** masuk.
Anda mengirim dokumen pertama pagi-pagi, lalu menunggu. Setiap kali printer
selesai mencetak, ternyata sudah ada dokumen baru dari orang lain yang lebih
muda — dan dokumen itulah yang diambil. Dokumen Anda terus tergeser ke belakang.
Selama masih ada orang yang mengirim, dokumen Anda tidak akan pernah tercetak
sampai lab tutup.

Karena itu aturannya harus adil: **dokumen yang paling pertama masuk adalah
dokumen yang paling pertama keluar**. Aturan itu disebut **FIFO** — *First In,
First Out*. Bentuk penyimpanan yang mengikutinya disebut **queue**, atau
antrean.

Persis seperti orang mengantre di kasir: yang baru datang berdiri paling
belakang, dan yang dilayani selalu yang paling depan. Tidak ada yang menyerobot.

> **Perhatikan bedanya dengan pertemuan lalu.** Tumpukan piring pada Pertemuan 5
> masuk dan keluar dari ujung yang **sama**, yaitu atas. Antrean memakai **dua**
> ujung yang berbeda: dokumen masuk dari belakang, dan keluar dari depan.

```
        depan                                    belakang
          |                                          |
       [ 101 ] -> [ 102 ] -> [ 103 ] -> [ 104 ] -> NULL
          ^                                 ^
     dicetak dari sini                masuk dari sini
```

| Istilah | Artinya |
|---|---|
| `depan` | Penunjuk ke dokumen **paling depan**, yaitu yang dicetak berikutnya. |
| `belakang` | Penunjuk ke dokumen **paling belakang**, yaitu tempat dokumen baru menyambung. |
| `next` | Penunjuk ke dokumen **di belakangnya**. Yang paling belakang `next`-nya `NULL`. |

Antrean **kosong** ditandai `depan` **dan** `belakang` sama-sama bernilai
`NULL`. Antrean berisi **tepat satu** dokumen membuat keduanya menunjuk node
yang sama.

### Kenapa ada dua penunjuk, padahal stack cukup satu?

Karena kedua ujungnya dipakai. Kalau yang dipegang hanya `depan`, setiap kali
ada dokumen baru masuk Anda harus menelusuri dari depan sampai ketemu ujung
belakangnya dulu — dan makin panjang antreannya, makin lama. Dengan `belakang`
yang selalu menunjuk ujung belakang, dokumen baru bisa langsung disambungkan
tanpa menelusuri apa pun.

Harga dari kemudahan itu: **setiap operasi harus menjaga kedua penunjuk tetap
sepakat.** Inilah sumber hampir semua kesalahan pada pertemuan ini.

> **Tidak ada kapasitas.** Antrean ini dibangun dari linked list, jadi tidak ada
> batas banyaknya dokumen dan tidak ada keadaan "penuh". Bila Anda pernah
> melihat contoh queue di internet yang memakai array dengan
> `if (belakang == MAX - 1) return false;`, itu bentuk yang lain dan **tidak**
> berlaku di sini.
>
> Yang tetap ada adalah keadaan **kosong** — dan itu sah, bukan kesalahan.
> Printer yang disuruh mencetak padahal tidak ada yang mengantre memang
> seharusnya tidak melakukan apa-apa.

## Fitur kedua — tombol "Batalkan Semua"

Lab tutup pukul lima, atau kertasnya habis. Operator menekan tombol **Batalkan
Semua**, dan seluruh dokumen yang masih mengantre dibuang sekaligus tanpa
dicetak.

Yang penting di sini bukan tampilannya. Papan memang akan langsung terlihat
bersih begitu kedua penunjuk dikosongkan — tetapi seluruh node dokumennya masih
menumpuk di memori, dan tidak ada satu pun yang bisa mencapainya lagi. Keadaan
seperti itu disebut **kebocoran memori**, dan itu ikut dinilai.

## Satu sesi di lab

Berikut satu sesi pemakaian Cetak Bersama. **Tiga langkah bertanda Soal adalah
pekerjaan yang harus Anda kerjakan.**

| # | Yang terjadi | Pekerjaan |
|---:|---|---|
| 1 | Empat orang mengirim dokumen berturut-turut. Setiap dokumen berdiri di ujung **paling belakang** antrean. | **Soal 1** `enqueue` |
| 2 | Papan di layar menampilkan dokumen yang akan dicetak berikutnya, dan di bawahnya seluruh isi antrean dari depan ke belakang. | *sudah disediakan* (`front`, `display`) |
| 3 | Printer mencetak. Dokumen **paling depan** keluar dari antrean, dan operator perlu tahu nomor dokumen mana yang barusan keluar. Menekan "Cetak" saat tidak ada yang mengantre tidak boleh membuat aplikasi berhenti tidak wajar. | **Soal 2** `dequeue` |
| 4 | Lab tutup. Tombol **Batalkan Semua** ditekan, dan seluruh dokumen yang masih mengantre dibuang sekaligus. | **Soal 3** `clear` |

### Yang sudah disediakan (tidak dinilai)

| Fungsi | Gunanya |
|---|---|
| `inisialisasi` | menyiapkan antrean baru menjadi kosong |
| `isEmpty` | apakah antrean sedang kosong |
| `front` | melihat dokumen terdepan tanpa mengeluarkannya — **pembanding untuk Soal 2** |
| `display` | membaca seluruh isi antrean menjadi satu baris teks |

Keempatnya sudah ditulis lengkap di `src/student.cpp`. Pakai `display` sesering
mungkin untuk memeriksa hasil kerja Anda sendiri.

---

## Tujuan Praktikum

Setelah pertemuan ini Anda diharapkan mampu:

- menjelaskan aturan FIFO dan menyebutkan contoh nyatanya;
- menjelaskan bedanya dengan LIFO pada Pertemuan 5;
- membangun queue dari linked list, tanpa batas kapasitas;
- menjaga **dua** penunjuk tetap sepakat di setiap operasi;
- menambah lewat satu ujung dan mengeluarkan lewat ujung yang lain;
- melaporkan dua hal sekaligus lewat kembalian dan parameter keluaran;
- menangani keadaan kosong (underflow) dengan wajar; dan
- membebaskan seluruh node antrean dengan benar.

---

## Materi

| Konsep | Yang perlu Anda kuasai |
|---|---|
| FIFO | yang pertama masuk, dialah yang pertama keluar |
| Dua ujung | masuk lewat `belakang`, keluar lewat `depan` |
| Dua penunjuk | keduanya harus selalu sepakat; keduanya `NULL` berarti kosong |
| Tanpa kapasitas | linked list tidak pernah "penuh" |
| Underflow | mengambil dari antrean kosong harus gagal dengan wajar |
| Parameter keluaran | `int&` untuk melaporkan nomor selain kembalian `bool` |
| Pembebasan | node yang keluar harus di-`delete` |

---

## Untuk Mahasiswa

### File yang Harus Dikerjakan

**Satu-satunya file yang dinilai adalah:**

```
src/student.cpp
```

Di bagian paling bawah `src/student.cpp` ada `main()`. Bagian itu memeragakan
seluruh sesi di lab, sehingga Anda bisa langsung melihat hasil kerja Anda
berjalan sebagai satu cerita utuh. `main()` tersebut **tidak ikut dinilai** dan
**bebas Anda ubah** sesukanya.

Jangan mengubah file lain. Perubahan pada `src/student.h`, `tests/checker.cpp`,
`tests/report.h`, atau `.github/workflows/` tidak akan membuat nilai Anda naik
dan dapat menyebabkan penilaian gagal.

### Contract / API

```cpp
struct Node {
    int   data;
    Node* next;
};

struct Queue {
    Node* depan;
    Node* belakang;
};

bool enqueue(Queue& q, int nomor);    // Soal 1
bool dequeue(Queue& q, int& nomor);   // Soal 2
void clear(Queue& q);                 // Soal 3

// Sudah disediakan, TIDAK dinilai
void   inisialisasi(Queue& q);
bool   isEmpty(const Queue& q);
bool   front(const Queue& q, int& nomor);
string display(const Queue& q);
```

Yang **wajib sama**: nama kedua struct beserta field-nya, nama fungsi, tipe
parameter, dan tipe kembalian.

### Aturan yang berlaku untuk seluruh pekerjaan

- Nomor dokumen bertipe `int`. Boleh negatif, boleh nol, boleh berulang.
- Antrean yang **kosong** adalah keadaan yang sah, bukan kesalahan.
- Tidak ada kapasitas dan tidak ada keadaan "penuh".
- Node dibuat dengan `new` dan yang keluar dilepas dengan `delete`.
- `depan` dan `belakang` harus selalu sepakat. Tidak boleh ada keadaan di mana
  yang satu `NULL` sedangkan yang lain tidak.
- Tidak ada satu pun fungsi yang mencetak ke layar.
- Ketiga soal dinilai **sendiri-sendiri**: checker menyiapkan antrean ujinya
  dengan merangkai node sendiri, tanpa memakai fungsi Anda. Kecualinya hanya
  dua test rangkaian FIFO pada Soal 2, yang memang menguji operasi campuran
  sehingga harus memakai `enqueue` Anda.

---

# Soal 1 — `enqueue` (35 poin)

> Langkah 1 pada cerita · dokumen baru berdiri paling belakang

Letakkan sebuah nomor dokumen baru di ujung **paling belakang** antrean. Seluruh
dokumen yang sudah mengantre tetap tersimpan dengan urutan yang sama persis,
hanya saja sekarang mereka semua berada di depan dokumen baru itu.

| Parameter | Artinya |
|---|---|
| `q` | Antrean milik pemanggil. Bertanda `&` supaya kedua penunjuknya bisa diubah |
| `nomor` | Nomor dokumen yang mau dimasukkan |
| *kembalian* | `true` bila dokumen baru berhasil masuk |

| Sebelum | Operasi | Sesudah |
|---|---|---|
| `depan -> 101 -> 102` | `enqueue(q, 103)` | `depan -> 101 -> 102 -> 103`, `belakang` menunjuk 103 |
| `(kosong)` | `enqueue(q, 101)` | `depan -> 101`, dan **kedua** penunjuk menunjuk node 101 |

**Yang perlu diingat.**
- `next` milik dokumen baru bernilai `NULL`, karena tidak ada yang berdiri di
  belakangnya.
- Dokumen baru harus disambungkan dari belakang: `next` milik dokumen yang
  tadinya paling belakang sekarang menunjuk dokumen baru. Lupa baris ini membuat
  dokumen baru **tidak pernah terlihat** dari `depan`, walaupun `belakang` sudah
  menunjuknya.
- Sesudah pemanggilan, `belakang` menunjuk node yang baru.
- **Ada dua keadaan awal yang berbeda, dan keduanya diuji:**

  | Keadaan awal | Yang harus terjadi |
  |---|---|
  | antrean **kosong** | `depan` dan `belakang` dua-duanya menunjuk node baru |
  | antrean **berisi** | `depan` **tidak boleh** berpindah sedikit pun |

- **Tidak pernah ada penolakan karena penuh.** Kembaliannya selalu `true` selama
  node barunya berhasil dibuat.
- Setiap pemanggilan menyediakan **tepat satu** node baru.
- Penelusuran dari `depan` terus lewat `next` harus berakhir tepat di node yang
  ditunjuk `belakang`. Bila keduanya tidak sepakat, antreannya rusak walaupun
  `display` kebetulan masih terlihat benar.

---

# Soal 2 — `dequeue` (40 poin)

> Langkah 3 pada cerita · printer mengambil dokumen terdepan

Keluarkan dokumen **paling depan** dari antrean, beri tahu pemanggil nomornya,
lalu buang node-nya dari memori.

Fungsi ini harus melaporkan **dua** hal sekaligus: berhasil atau tidak, dan
nomor dokumen mana yang keluar. Karena satu fungsi hanya bisa mengembalikan satu
nilai, yang kedua disampaikan lewat parameter `nomor` yang bertanda `&`.

> **Petunjuk.** Bandingkan dengan `front` yang sudah disediakan. Keduanya
> sama-sama mengisi `nomor` dan sama-sama mengembalikan `bool`, dan cara
> menangani antrean kosongnya pun sama. Bedanya cuma satu: `front` hanya
> melihat, sedangkan `dequeue` benar-benar mengeluarkan dan membuang node-nya.
> Perhatikan juga tipe parameternya — `front` menerima `const Queue&` justru
> karena dia tidak mengubah apa pun.

| Parameter | Artinya |
|---|---|
| `q` | Antrean milik pemanggil. Bertanda `&` |
| `nomor` | Tempat pemanggil menerima nomor dokumen yang keluar. Bertanda `&` |
| *kembalian* | `true` bila ada dokumen yang keluar, `false` bila antrean kosong |

| Sebelum | Operasi | Sesudah |
|---|---|---|
| `depan -> 101 -> 102 -> 103` | `int n; dequeue(q, n);` | `depan -> 102 -> 103`, `n` = 101 |
| `depan -> 101` | `int n; dequeue(q, n);` | `(kosong)`, `n` = 101, **kedua** penunjuk `NULL` |
| `(kosong)` | `int n = -999; dequeue(q, n);` | `(kosong)`, `n` **tetap** -999 |

**Yang perlu diingat.**
- Yang keluar selalu dokumen **paling depan**, bukan yang paling belakang.
  Inilah yang membedakan antrean dari tumpukan Pertemuan 5.
- `nomor` harus sudah diisi **sebelum** node-nya di-`delete`. Node yang sudah
  dilepas tidak boleh dibaca lagi.
- Node yang keluar harus dibuang dengan `delete`, **tepat satu** per pemanggilan
  yang berhasil.
- **Ini kesalahan paling sering pada pertemuan ini:** bila yang keluar adalah
  dokumen **terakhir**, `belakang` juga harus dijadikan `NULL`. Kalau tidak,
  `belakang` masih menunjuk node yang sudah dilepas — sebuah penunjuk
  menggantung. Papan mungkin terlihat benar, tetapi dokumen yang masuk sesudahnya
  akan disambungkan ke memori yang sudah mati.
- **Underflow:** bila antrean sedang kosong, kembaliannya `false` dan `nomor`
  **tidak boleh disentuh sama sekali**. Ini ikut diuji — checker mengisi variabel
  penerima dengan penanda lebih dulu, lalu memastikan penanda itu masih utuh.
- Sesudah antrean menjadi kosong, antrean itu harus tetap bisa dipakai lagi
  seperti biasa.

---

# Soal 3 — `clear` (25 poin)

> Langkah 4 pada cerita · tombol "Batalkan Semua"

Buang seluruh dokumen yang masih mengantre sekaligus, sampai antreannya
benar-benar kosong.

Perhatikan baik-baik kata "dibuang". Memutus sambungannya saja tidak cukup. Anda
memang bisa langsung membuat kedua penunjuk bernilai `NULL`, dan sekilas
antreannya akan terlihat kosong — tetapi seluruh node-nya masih menumpuk di
memori, dan sekarang tidak ada satu pun yang bisa mencapainya lagi. Keadaan
seperti itu disebut **kebocoran memori**, dan itu ikut dinilai.

| Parameter | Artinya |
|---|---|
| `q` | Antrean milik pemanggil. Bertanda `&` |
| *kembalian* | Tidak ada (`void`) |

**Yang perlu diingat.**
- Sesudah selesai, `depan` dan `belakang` sama-sama bernilai `NULL`.
- **Seluruh** node harus dibuang dengan `delete`, bukan hanya yang terdepan.
  Banyaknya node yang dibuang ikut dihitung checker secara **tepat**.
- Alamat dokumen berikutnya harus sudah disimpan **sebelum** node sekarang
  di-`delete`. Menulis `delete q.depan;` lalu `q.depan = q.depan->next;` berarti
  membaca memori yang sudah dilepas.
- Memanggilnya pada antrean yang sudah kosong, atau dua kali berturut-turut,
  harus aman.
- Sesudah dikosongkan, antrean harus tetap bisa **dipakai lagi** seperti biasa.

---

## Batasan

- Hanya **tiga** fungsi di `src/student.cpp` yang dinilai: `enqueue`, `dequeue`,
  dan `clear`. Empat fungsi lain sudah disediakan dan tidak dinilai. `main()` di
  bagian paling bawah file itu bebas Anda ubah.
- `cin` hanya boleh dipakai di dalam `main()` tersebut.
- Jangan mengubah `struct Node`, `struct Queue`, maupun signature fungsi di
  `src/student.h`.
- Materi pertemuan ini terbatas pada **queue dengan linked list**. Tidak
  diperlukan array sebagai penyimpanan, stack, pohon, maupun graf.
- Tidak perlu memakai container pustaka standar (`queue`, `deque`, `vector`,
  `list`, dan sejenisnya) — isi antrean harus benar-benar tersimpan sebagai
  rantai `Node` yang Anda kelola sendiri.

---

## Penilaian Otomatis

Total **20 test case**, masing-masing bernilai **5 poin**:

| Soal | Fungsi | Test | Bobot |
|---|---|---:|---:|
| 1 | `enqueue` | 7 | 35 |
| 2 | `dequeue` | 8 | 40 |
| 3 | `clear` | 5 | 25 |
| | **Total** | **20** | **100** |

| Kondisi | Score |
|---|---|
| Gagal compile | 0 |
| Sebagian test lolos | jumlah test lolos × 5 |
| Semua test lolos | 100 |

Antrean untuk pengujian disiapkan sendiri oleh checker, bukan lewat fungsi Anda.
Karena itu Soal 1 yang belum benar **tidak** ikut menjatuhkan nilai Soal 2 dan
Soal 3. Pengecualiannya hanya dua test rangkaian FIFO pada Soal 2.

### Membaca Hasil

| Status | Artinya |
|---|---|
| ✅ hijau | Semua test berhasil |
| ❌ merah | Ada test yang gagal, compile error, atau program berhenti tidak wajar |

Isi antrean ditulis dari **depan ke belakang**, jadi elemen paling kiri adalah
yang dicetak berikutnya. Checker juga melaporkan kedua penunjuk secara terpisah.
Contoh:

```
Expected: keluar=[101 102] dibuang=2/2 isi(depan->belakang)=[] banyak=0 depan=(kosong) belakang=(kosong) sepakat=true
Got     : keluar=[101 102] dibuang=2/2 isi(depan->belakang)=[] banyak=0 depan=(kosong) belakang=17 sepakat=false
```

Contoh ini berarti dokumennya sudah keluar dengan urutan yang benar dan node-nya
sudah dibuang, tetapi `belakang` tidak dikosongkan saat dokumen terakhir keluar —
ia masih menunjuk node yang sudah dilepas, sehingga angkanya pun sudah sampah.

| Keterangan | Artinya |
|---|---|
| `sepakat=false` | `depan` dan `belakang` tidak lagi cocok satu sama lain |
| `dibuang=0/1` | Node keluar dari antrean tetapi tidak di-`delete` (bocor) |
| `dibebaskan=1/10` | `clear` hanya membuang sebagian node |

| Step yang gagal | Penyebab |
|---|---|
| `Periksa penggunaan cin` | Ada `cin`/`scanf` di dalam fungsi yang dinilai |
| `Compile student.cpp` | Ada syntax/compile error di `student.cpp` |
| `Compile checker` | Nama atau signature fungsi tidak sesuai `student.h`, atau `main()` Anda keluar dari blok `#ifndef ADA_MAIN_LAIN` |
| `Jalankan test & hitung score` | Kode berhasil dikompilasi tetapi perilakunya belum sesuai |

---

## Cara Menjalankan Program Anda

`src/student.cpp` adalah program C++ utuh. Tekan **F5** di VS Code, atau:

```bash
g++ -std=c++17 src/student.cpp -o latihan
./latihan
```

`main()` bawaan menjalankan **sesi di lab secara berurutan**, dan menampilkan
hasil tiap langkah berdampingan dengan jawaban yang benar.

> **Saran urutan pengerjaan.** Kerjakan Soal 1 lebih dulu, karena seluruh
> percobaan di `main()` memerlukan antrean yang sudah terisi.

Perhatikan langkah **[8]** pada keluaran program — langkah itu memakai antrean
lagi sesudah dikosongkan. Kalau isinya aneh atau programnya berhenti di situ,
hampir pasti `belakang` belum dikosongkan saat dokumen terakhir keluar.

> **Penting: `cin` hanya di dalam `main()`.**
> Jangan pernah menaruh `cin` di dalam ketiga fungsi yang dinilai. Saat menilai,
> checker memanggil fungsi-fungsi itu tanpa memberi masukan apa pun, sehingga
> `cin` di sana membaca sampah — dan nilai Anda berubah-ubah setiap kali dinilai.

### Memeriksa Kebocoran Memori (opsional, sangat disarankan)

Karena pertemuan ini memakai `new` dan `delete`, ada satu kesalahan yang tidak
terlihat dari keluaran program: **node yang tidak pernah dilepas**.

```bash
g++ -std=c++17 -fsanitize=address,leak -g src/student.cpp -o latihan_periksa
./latihan_periksa
```

Bila ada node yang bocor, program melaporkannya di akhir dengan keterangan
`LeakSanitizer: detected memory leaks`. Pemeriksa yang sama juga akan menangkap
pembacaan node yang sudah dilepas — yaitu akibat `belakang` yang menggantung.

---

## Cara Menjalankan Test di Komputer Sendiri

Butuh `g++` yang mendukung C++17, dan sistem berbasis Linux/macOS (atau WSL di
Windows) karena checker menjalankan setiap test sebagai proses terpisah:

```bash
chmod +x scripts/run_tests.sh
./scripts/run_tests.sh
```

Pada starter code yang belum diisi, compiler memunculkan peringatan
*unused parameter*. Itu wajar dan **tidak** mengurangi nilai.

---

## Cara Mengumpulkan

Tidak ada tombol "submit". **Push adalah pengumpulan.**

```bash
git clone https://github.com/<ORG>/praktikum-06-<username>.git
cd praktikum-06-<username>
# edit src/student.cpp
git add src/student.cpp
git commit -m "Kerjakan pertemuan 6"
git push
```

Anda boleh push berkali-kali. **Setiap percobaan tersimpan**, misalnya
35 → 75 → 100.

---

## Struktur Repository

```
.
├── .github/workflows/test.yml     ← workflow penilaian (jangan diubah)
├── .vscode/                       ← setelan tombol Run (jangan diubah)
├── src/
│   ├── student.h                  ← kontrak/interface (jangan diubah)
│   └── student.cpp                ← KERJAKAN DI SINI ← (main() ada di bawahnya)
├── tests/
│   ├── checker.cpp                ← test instruktur (jangan diubah)
│   └── report.h                   ← penulis result.json (jangan diubah)
├── scripts/
│   ├── run_tests.sh               ← uji lokal
│   ├── periksa_masukan.py         ← menolak cin di dalam fungsi yang dinilai
│   ├── anotasi_gcc.py             ← terjemahan error compiler ke bahasa Indonesia
│   ├── job_summary.py             ← Job Summary dari result.json
│   └── write_error_result.sh      ← result.json saat compile error
└── README.md
```
