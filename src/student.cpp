// =============================================================================
// student.cpp — KERJAKAN DI SINI
// Pertemuan 6: Queue (Antrean) dengan Linked List
// =============================================================================

#include "student.h"

#include <iostream>
#include <sstream>

using namespace std;

// -----------------------------------------------------------------------------
// DAFTAR PEKERJAAN DAN BOBOTNYA
// -----------------------------------------------------------------------------
//     Soal 1  enqueue    dokumen baru berdiri paling belakang     35 poin
//     Soal 2  dequeue     printer mengambil dokumen terdepan      40 poin
//     Soal 3  clear       tombol Batalkan Semua                   25 poin
//                                                                ---------
//                                                                100 poin
//
// -----------------------------------------------------------------------------
// SUDAH DISEDIAKAN — TIDAK DINILAI
// -----------------------------------------------------------------------------
//   inisialisasi   menyiapkan antrean baru menjadi kosong
//   isEmpty        apakah antrean sedang kosong
//   front          melihat dokumen terdepan tanpa mengeluarkannya
//   display        membaca seluruh isi antrean menjadi satu baris teks
//
// Keempatnya sudah ditulis lengkap di bagian bawah file ini. Pakai `display`
// sesering mungkin untuk memeriksa hasil kerja Anda sendiri.
//
// =============================================================================

// =============================================================================
// SUDAH DISEDIAKAN — TIDAK PERLU DIUBAH, TIDAK DINILAI
// =============================================================================
// Keempat fungsi di bawah sudah lengkap. Silakan dibaca — khususnya `front`,
// yang menjadi pembanding untuk Soal 2.

void inisialisasi(Queue& q) {
    q.depan = NULL;
    q.belakang = NULL;
}

bool isEmpty(const Queue& q) {
    return q.depan == NULL;
}

bool front(const Queue& q, int& nomor) {
    if (q.depan == NULL) return false;

    nomor = q.depan->data;
    return true;
}

string display(const Queue& q) {
    ostringstream keluaran;
    bool pertama = true;

    for (Node* p = q.depan; p != NULL; p = p->next) {
        if (!pertama) keluaran << " ";
        keluaran << p->data;
        pertama = false;
    }

    return keluaran.str();
}
// =============================================================================

// =============================================================================
// SOAL 1 — enqueue()                                                  35 poin
// =============================================================================
bool enqueue(Queue& q, int nomor) {
    // TODO: kerjakan di sini
    return false;
}

// =============================================================================
// SOAL 2 — dequeue()                                                  40 poin
// =============================================================================
bool dequeue(Queue& q, int& nomor) {
    // TODO: kerjakan di sini
    return false;
}

// =============================================================================
// SOAL 3 — clear()                                                    25 poin
// =============================================================================
void clear(Queue& q) {
    // TODO: kerjakan di sini
}

// =============================================================================
// MAIN() — program utama. TIDAK DINILAI, BEBAS DIUBAH.
// =============================================================================
// Bagian di bawah ini membuat file ini menjadi program C++ utuh. Tekan Run di
// VS Code, atau jalankan lewat terminal:
//
//     g++ -std=c++17 src/student.cpp -o latihan
//     ./latihan
//
//
// SATU ATURAN YANG TIDAK BOLEH DILANGGAR
// --------------------------------------
// cin hanya boleh dipakai DI DALAM main() ini. JANGAN menaruh cin di dalam
// ketiga fungsi yang dinilai. Saat menilai, checker memanggil fungsi-fungsi itu
// tanpa memberi masukan apa pun, jadi cin di sana akan membaca sampah — dan
// nilai Anda berubah-ubah setiap kali dinilai, dari kode yang sama persis.
//
// (Baris #ifndef di bawah hanya urusan teknis: saat menilai, checker memakai
//  main() miliknya sendiri, jadi main() ini dilewati supaya tidak bentrok.
//  Anda tidak perlu menyentuhnya.)
// =============================================================================

#ifndef ADA_MAIN_LAIN

// Mencetak keadaan antrean apa adanya, memakai fungsi yang sudah disediakan.
static void tampilkanKeadaan(const Queue& q) {
    int terdepan = 0;
    bool ada = front(q, terdepan);

    cout << "    isi antrean         : \"" << display(q) << "\"" << endl;
    cout << "    dicetak berikutnya  : ";
    if (ada) cout << terdepan << endl;
    else     cout << "(tidak ada)" << endl;
    cout << "    isEmpty             : "
         << (isEmpty(q) ? "true" : "false") << endl;
}

int main() {
    cout << "==================================================" << endl;
    cout << " Study Case — Antrean Pencetakan \"Cetak Bersama\"" << endl;
    cout << " Memeragakan satu sesi di lab" << endl;
    cout << " (bagian ini tidak ikut dinilai)" << endl;
    cout << "==================================================" << endl;

    Queue q;
    inisialisasi(q);

    // -----------------------------------------------------------------------
    cout << "\n[0] Lab baru dibuka, belum ada yang mengantre" << endl;
    tampilkanKeadaan(q);

    // -----------------------------------------------------------------------
    cout << "\n[1] SOAL 1 — enqueue: empat orang mengirim dokumen" << endl;
    cout << "    enqueue 101         : "
         << (enqueue(q, 101) ? "true" : "false") << endl;
    cout << "    enqueue 102         : "
         << (enqueue(q, 102) ? "true" : "false") << endl;
    cout << "    enqueue 103         : "
         << (enqueue(q, 103) ? "true" : "false") << endl;
    cout << "    enqueue 104         : "
         << (enqueue(q, 104) ? "true" : "false") << endl;
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: isi \"101 102 103 104\", berikutnya 101" << endl;

    // -----------------------------------------------------------------------
    cout << "\n[2] SOAL 1 — tidak ada batas kapasitas: 8 dokumen lagi" << endl;
    bool semuaMasuk = true;
    for (int i = 1; i <= 8; ++i) {
        if (!enqueue(q, 200 + i)) semuaMasuk = false;
    }
    cout << "    semua masuk         : "
         << (semuaMasuk ? "true" : "false") << endl;
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: semua masuk true — linked list tidak pernah penuh"
         << endl;

    // -----------------------------------------------------------------------
    cout << "\n[3] SOAL 3 — clear: dibersihkan dulu supaya sesinya mudah "
            "diikuti" << endl;
    clear(q);
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: isi \"\", berikutnya (tidak ada), isEmpty true"
         << endl;

    // -----------------------------------------------------------------------
    cout << "\n[4] SOAL 1 — tiga dokumen dikirim ulang" << endl;
    enqueue(q, 101);
    enqueue(q, 102);
    enqueue(q, 103);
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: isi \"101 102 103\"" << endl;

    // -----------------------------------------------------------------------
    cout << "\n[5] SOAL 2 — dequeue: printer mencetak dua dokumen" << endl;
    int dicetak = 0;
    for (int i = 1; i <= 2; ++i) {
        if (dequeue(q, dicetak)) {
            cout << "    cetak ke-" << i << "          : berhasil, dokumen "
                 << dicetak << endl;
        } else {
            cout << "    cetak ke-" << i << "          : gagal (antrean kosong)"
                 << endl;
        }
    }
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: keluar 101 lalu 102 — sesuai urutan masuk (FIFO)"
         << endl;

    // -----------------------------------------------------------------------
    cout << "\n[6] SOAL 2 — dokumen terakhir keluar, antrean menjadi kosong"
         << endl;
    if (dequeue(q, dicetak)) {
        cout << "    cetak terakhir      : berhasil, dokumen " << dicetak
             << endl;
    }
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: keluar 103, lalu antrean kosong" << endl;

    // -----------------------------------------------------------------------
    cout << "\n[7] SOAL 2 — tombol Cetak ditekan saat tidak ada yang mengantre"
         << endl;
    int penerima = -999;
    if (dequeue(q, penerima)) {
        cout << "    cetak               : berhasil, dokumen " << penerima
             << endl;
    } else {
        cout << "    cetak               : gagal (antrean kosong), nilai tidak "
                "diubah (" << penerima << ")" << endl;
    }
    cout << "\n    Yang benar: gagal, dan nilainya tetap -999 (tidak disentuh)"
         << endl;

    // -----------------------------------------------------------------------
    cout << "\n[8] Antrean tetap bisa dipakai lagi sesudah kosong" << endl;
    enqueue(q, 301);
    enqueue(q, 302);
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: isi \"301 302\", berikutnya 301" << endl;
    cout << "    (kalau isinya aneh atau programnya berhenti di sini,"
         << endl;
    cout << "     kemungkinan `belakang` belum dikosongkan saat dokumen"
         << endl;
    cout << "     terakhir keluar pada langkah [6])" << endl;

    // -----------------------------------------------------------------------
    cout << "\n[9] SOAL 3 — lab tutup, Batalkan Semua" << endl;
    clear(q);
    tampilkanKeadaan(q);
    cout << "\n    Yang benar: isi \"\", isEmpty true" << endl;

    cout << "\n==================================================" << endl;
    cout << " Sesi selesai. Silakan ubah bagian ini untuk" << endl;
    cout << " mencoba percobaan Anda sendiri." << endl;
    cout << "==================================================" << endl;

    return 0;
}

#endif // ADA_MAIN_LAIN
