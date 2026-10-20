// =============================================================================
// student.h — Interface Praktikum
// Pertemuan 6: Queue (Antrean) dengan Linked List
// =============================================================================
// INSTRUKSI UNTUK MAHASISWA:
//   - JANGAN mengubah file ini.
//   - Implementasikan seluruh fungsi di dalam src/student.cpp.
//   - Nama struct, nama field, nama fungsi, tipe parameter, dan tipe kembalian
//     adalah KONTRAK: checker memanggilnya langsung, jadi tidak boleh diubah.
//   - Cara Anda memenuhi kontrak sepenuhnya bebas. Penilaian hanya melihat
//     perilaku yang teramati oleh pemanggil.
//
// -----------------------------------------------------------------------------
// STUDY CASE: ANTREAN PENCETAKAN "CETAK BERSAMA"
// -----------------------------------------------------------------------------
// Pertemuan ini hanya punya SATU soal, yaitu study case antrean pencetakan
// Cetak Bersama. Ceritanya lengkap ada di src/student.cpp. File ini hanya
// ringkasan kontraknya.
//
// Ada TIGA pekerjaan yang dinilai:
//
//     Soal 1  enqueue     dokumen baru berdiri paling belakang     35 poin
//     Soal 2  dequeue      printer mengambil dokumen terdepan      40 poin
//     Soal 3  clear        tombol Batalkan Semua                   25 poin
//
// Empat fungsi lain SUDAH DISEDIAKAN lengkap di src/student.cpp dan tidak
// dinilai: `inisialisasi`, `isEmpty`, `front`, dan `display`.
//
// -----------------------------------------------------------------------------
// KONVENSI UMUM
// -----------------------------------------------------------------------------
//   - Nomor dokumen bertipe `int`. Boleh negatif, boleh nol, dan boleh muncul
//     lebih dari sekali di dalam satu antrean.
//   - Antrean yang KOSONG adalah keadaan yang sah, bukan kesalahan.
//   - Antrean ini berbasis linked list, jadi TIDAK ada batas banyaknya dokumen
//     dan tidak ada keadaan "penuh".
//   - Tidak ada fungsi yang mencetak apa pun ke layar.
// =============================================================================

#ifndef STUDENT_H
#define STUDENT_H

#include <string>

using std::string;

/**
 * Satu dokumen yang sedang mengantre.
 *
 *   `data`   nomor dokumen
 *   `next`   dokumen DI BELAKANGNYA, atau `NULL` bila dia paling belakang
 */
struct Node {
    int   data;
    Node* next;
};

/**
 * Sebuah antrean, diwakili oleh DUA penunjuk sekaligus.
 *
 *   `depan`      dokumen paling depan — inilah yang dicetak berikutnya
 *   `belakang`   dokumen paling belakang — di sinilah dokumen baru menyambung
 *
 *        depan                                    belakang
 *          |                                          |
 *       [ 101 ] -> [ 102 ] -> [ 103 ] -> [ 104 ] -> NULL
 *
 * Antrean KOSONG ditandai dengan `depan` DAN `belakang` sama-sama bernilai
 * `NULL`. Keduanya harus selalu sepakat: tidak boleh ada keadaan di mana
 * salah satunya `NULL` sedangkan yang lain tidak.
 *
 * Antrean yang berisi TEPAT SATU dokumen membuat `depan` dan `belakang`
 * menunjuk node yang sama.
 */
struct Queue {
    Node* depan;
    Node* belakang;
};

/**
 * SOAL 1 — Seseorang mengirim dokumen baru, sehingga dokumen itu berdiri
 * PALING BELAKANG dalam antrean.
 *
 * Kontrak:
 *   - Sesudah pemanggilan, dokumen baru itulah dokumen paling belakang, dan
 *     `belakang` milik pemanggil menunjuk node tersebut.
 *   - Seluruh dokumen yang sudah mengantre tetap berada di depan dokumen baru
 *     dengan urutan yang sama persis.
 *   - Antrean yang semula KOSONG berubah menjadi antrean berisi satu dokumen;
 *     dalam keadaan itu `depan` milik pemanggil IKUT berubah, dan kedua
 *     penunjuk menunjuk node yang sama. Pada antrean yang sudah berisi, `depan`
 *     TIDAK boleh berpindah.
 *   - `next` milik dokumen baru bernilai `NULL`, karena tidak ada yang
 *     berdiri di belakangnya.
 *   - Penelusuran dari `depan` terus lewat `next` harus berakhir tepat di node
 *     yang ditunjuk `belakang`.
 *   - TIDAK ADA PENOLAKAN KARENA PENUH. Kembaliannya `true` selama node barunya
 *     berhasil dibuat.
 */
bool enqueue(Queue& q, int nomor);

/**
 * SOAL 2 — Printer mengambil dokumen yang berada PALING DEPAN untuk dicetak,
 * dan operator menerima nomornya.
 *
 * Kontrak:
 *   - Yang keluar selalu dokumen paling depan, yaitu dokumen yang paling
 *     PERTAMA masuk di antara yang masih mengantre.
 *   - `nomor` diisi dengan nomor dokumen yang keluar, dan pengisian itu harus
 *     terjadi SEBELUM node-nya dilepas.
 *   - `depan` berpindah ke dokumen berikutnya, dan seluruh dokumen sisanya
 *     tetap mengantre dengan urutan yang sama persis.
 *   - BILA YANG KELUAR ADALAH DOKUMEN TERAKHIR, antrean kembali KOSONG:
 *     `depan` DAN `belakang` sama-sama harus menjadi `NULL`. Lupa
 *     mengosongkan `belakang` meninggalkan penunjuk yang menggantung ke node
 *     yang sudah dilepas, dan itu ikut diuji.
 *   - Node dokumen yang keluar harus dibuang dengan `delete`.
 *   - Kembaliannya `true` bila ada dokumen yang benar-benar keluar.
 *   - Bila antrean sedang KOSONG, kembaliannya `false`, `nomor` TIDAK boleh
 *     disentuh sama sekali, dan antrean tetap kosong.
 */
bool dequeue(Queue& q, int& nomor);

/**
 * SOAL 3 — Tombol "Batalkan Semua" ditekan: seluruh dokumen yang masih
 * mengantre dibuang sekaligus.
 *
 * Kontrak:
 *   - Sesudah selesai, antrean KOSONG: `depan` dan `belakang` sama-sama
 *     `NULL`.
 *   - SELURUH node harus dibuang dengan `delete`, bukan hanya yang terdepan.
 *     Memutus sambungan tanpa melepas node-nya adalah kebocoran memori, dan
 *     banyaknya node yang dibuang dihitung secara TEPAT.
 *   - Alamat dokumen berikutnya harus sudah disimpan SEBELUM node sekarang
 *     dilepas.
 *   - Memanggilnya pada antrean yang sudah kosong, atau dua kali
 *     berturut-turut, harus aman.
 *   - Sesudah dikosongkan, antrean harus tetap dapat DIPAKAI LAGI seperti
 *     biasa.
 */
void clear(Queue& q);

// =============================================================================
// SUDAH DISEDIAKAN — TIDAK DINILAI
// =============================================================================
// Keempat fungsi di bawah sudah ditulis lengkap di src/student.cpp. Anda tidak
// perlu mengerjakannya. `front` sengaja disediakan sebagai PEMBANDING untuk
// Soal 2: bentuknya nyaris sama dengan `dequeue`, dan bedanya justru itulah
// inti pertemuan ini.
// =============================================================================

/** Menyiapkan antrean baru menjadi kosong. */
void inisialisasi(Queue& q);

/** Apakah antrean sedang kosong. */
bool isEmpty(const Queue& q);

/** Melihat nomor dokumen terdepan TANPA mengeluarkannya. Mengembalikan `false`
 *  bila antrean kosong, dan dalam keadaan itu `nomor` tidak disentuh.
 *  Pasangan pembanding untuk Soal 2. */
bool front(const Queue& q, int& nomor);

/** Membaca seluruh isi antrean menjadi satu baris teks, dari depan ke
 *  belakang, dipisahkan satu spasi. Antrean kosong menghasilkan teks kosong. */
string display(const Queue& q);

#endif // STUDENT_H
