// =============================================================================
// checker.cpp — Test Harness Instruktur
// Pertemuan 6: Queue (Antrean) dengan Linked List
// =============================================================================
// File ini adalah milik instruktur dan TIDAK boleh diubah mahasiswa.
//
// PRINSIP PENILAIAN (behavior-based):
//   Checker hanya memanggil fungsi mahasiswa lalu memeriksa akibatnya: isi dan
//   urutan antrean sesudah pemanggilan, banyaknya dokumen, keadaan KEDUA
//   penunjuk, nilai yang diterima pemanggil lewat parameter keluaran, dan nilai
//   yang dikembalikan. Nama variabel, gaya penulisan, jenis loop, urutan kode,
//   komentar, dan formatting tidak pernah diperiksa. Implementasi apa pun yang
//   memenuhi kontrak di student.h akan PASS.
//
// BOBOT:
//   20 test case, bobot rata 100 / 20 = 5 poin per test (lihat report.h).
//
//   Pertemuan ini hanya punya SATU soal, yaitu study case antrean pencetakan
//   "Cetak Bersama". Ketiga fungsi di bawah adalah pekerjaan di dalam study
//   case yang sama, bukan soal yang berdiri sendiri-sendiri. Masing-masing
//   tetap diberi suite terpisah supaya nilai parsial tetap dapat.
//
//     Soal 1  enqueue   7 test = 35   dokumen baru berdiri paling belakang
//     Soal 2  dequeue   8 test = 40   printer mengambil dokumen terdepan
//     Soal 3  clear     5 test = 25   tombol Batalkan Semua
//
//   Empat fungsi lain (inisialisasi, isEmpty, front, display) SUDAH DISEDIAKAN
//   lengkap di src/student.cpp dan TIDAK dinilai. Checker tidak memakai satu
//   pun di antaranya — ia punya alat bacanya sendiri.
//
// KENAPA HANYA TIGA SOAL:
//   Daftar operasi queue pada modul adalah inisialisasi, isEmpty, isFull,
//   enqueue, dequeue, dan front. Karena antrean pada pertemuan ini berbasis
//   linked list, `isFull` tidak punya arti lagi dan digantikan `clear` —
//   perlakuan yang sama dengan Pertemuan 5. Dari sisanya, `inisialisasi`,
//   `isEmpty`, dan `front` masing-masing hanya satu sampai dua baris, sehingga
//   disediakan jadi. Yang benar-benar layak dinilai tinggal tiga.
//
// TASK SALING BEBAS:
//   Antrean untuk pengujian dibangun langsung oleh checker (lihat buatQueue)
//   yang merangkai sendiri node-nya dengan `new`, bukan lewat fungsi mahasiswa.
//   Bentuk penyimpanan `Node` dan `Queue` sudah ditetapkan sebagai kontrak di
//   student.h, sehingga checker dapat menyiapkan keadaan apa pun — kosong,
//   berisi satu dokumen, atau berisi banyak dokumen — tanpa bergantung pada
//   satu pun fungsi mahasiswa. Dengan begitu `enqueue` yang salah tidak ikut
//   menjatuhkan nilai `dequeue` maupun `clear`.
//
//   Pengecualian yang disengaja adalah dua test rangkaian FIFO pada Soal 2
//   (uji13 dan uji14), yang memang menguji rangkaian operasi campuran dan
//   karena itu memakai `enqueue` mahasiswa sendiri.
//
// KEDUA PENUNJUK DIPERIKSA LANGSUNG:
//   Kesalahan khas pada queue adalah `belakang` yang tidak dikosongkan saat
//   dokumen terakhir keluar. Akibatnya tidak terlihat dari isi antrean — yang
//   tertinggal hanya sebuah penunjuk menggantung ke node yang sudah dilepas.
//   Karena `Queue` adalah struct biasa, checker membaca `q.depan` dan
//   `q.belakang` secara langsung dan melaporkan keduanya. Dengan begitu
//   kesalahan itu tertangkap tanpa perlu memanggil `enqueue` mahasiswa.
//
//   Diperiksa juga KESEPAKATAN kedua penunjuk: node terakhir yang dicapai dari
//   `depan` harus node yang sama dengan yang ditunjuk `belakang`, dan
//   `belakang->next` harus NULL.
//
// ISOLASI PROSES:
//   Setiap test dijalankan di dalam PROSES ANAK hasil fork() dengan batas waktu
//   dan batas memori. Anak menyiapkan antreannya sendiri, memanggil fungsi
//   mahasiswa, lalu menuliskan RINGKASAN KEADAAN AKHIR sebagai teks; induk
//   hanya membandingkan teks itu dengan teks yang diharapkan. Akibatnya:
//     - penunjuk yang tidak sah / node yang sudah dilepas -> FAIL biasa
//     - rantai node yang menunjuk balik ke dirinya sendiri -> "WAKTU HABIS"
//       atau tertangkap lebih dulu oleh BATAS_TELUSUR di gambarQueue
//     - ketiganya TIDAK menghentikan test-test berikutnya
//
//   Isolasi ini murni soal ketahanan checker. Yang dinilai tetap perilaku
//   fungsi mahasiswa, dan mekanisme grading global (report.h, scripts/,
//   workflow, skema result.json) tidak diubah sama sekali.
//
// KETAHANAN TERHADAP CRASH:
//   Checker menyimpan snapshot result.json setiap kali satu test selesai,
//   dengan test yang belum sempat berjalan dicatat sebagai FAIL. Nilai parsial
//   yang sudah diperoleh tetap tercatat dan tidak berubah menjadi 0.
//
// CATATAN KEAMANAN:
//   Mahasiswa dapat membaca file ini. Mitigasi:
//   - Setiap kontrak diuji dengan beberapa keadaan antrean (kosong, satu
//     dokumen, banyak dokumen) dan beberapa nomor (positif, nol, negatif)
//   - Repository mahasiswa bersifat privat
// =============================================================================

#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <vector>

#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include "report.h"
#include "../src/student.h"

using namespace std;

// =============================================================================
// Penghitung blok memori dinamis yang masih hidup
// =============================================================================
// Mengganti operator new/delete global adalah cara yang sah dalam C++ dan
// berlaku untuk seluruh program, termasuk `new Node` di dalam student.cpp.
// Angka ini hanya dibaca sebagai SELISIH pada potongan kode yang sangat pendek,
// sehingga alokasi milik checker sendiri tidak ikut terhitung.

static long long g_blokHidup = 0;

void* operator new(size_t ukuran) {
    if (ukuran == 0) ukuran = 1;
    void* blok = malloc(ukuran);
    if (blok == NULL) throw bad_alloc();
    ++g_blokHidup;
    return blok;
}

void* operator new[](size_t ukuran) {
    return ::operator new(ukuran);
}

void operator delete(void* blok) noexcept {
    if (blok != NULL) {
        --g_blokHidup;
        free(blok);
    }
}

void operator delete[](void* blok) noexcept {
    ::operator delete(blok);
}

void operator delete(void* blok, size_t) noexcept {
    ::operator delete(blok);
}

void operator delete[](void* blok, size_t) noexcept {
    ::operator delete(blok);
}

static long long blokHidup() {
    return g_blokHidup;
}

// =============================================================================
// Test Framework (sederhana, tanpa dependency eksternal)
// =============================================================================

// ANSI color codes untuk output terminal yang jelas
#define COLOR_GREEN "\033[32m"
#define COLOR_RED   "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_CYAN  "\033[36m"
#define COLOR_RESET "\033[0m"
#define COLOR_BOLD  "\033[1m"

static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

// Banyaknya test yang direncanakan pada pertemuan ini. Dipakai untuk menghitung
// skor snapshot supaya test yang belum berjalan tetap terhitung sebagai gagal.
static const int TOTAL_TEST_DIRENCANAKAN = 20;

// Menulis result.json versi sementara setelah setiap test selesai.
// Test yang belum dijalankan ditambahkan sebagai FAIL, lalu dilepas kembali,
// sehingga isi rekaman sebenarnya tidak terpengaruh.
static void simpanHasilSementara() {
    vector<TestRecord>& rekaman = test_records();
    const size_t jumlahAsli = rekaman.size();

    for (size_t i = jumlahAsli;
         i < static_cast<size_t>(TOTAL_TEST_DIRENCANAKAN); ++i) {
        rekaman.push_back(TestRecord{
            "(test #" + to_string(i + 1) + " belum dijalankan)",
            "FAIL",
            "Checker berhenti sebelum test ini sempat dijalankan."});
    }

    const int skor = passed_tests * 100 / TOTAL_TEST_DIRENCANAKAN;
    write_result_json("result.json", skor);

    rekaman.resize(jumlahAsli);
}

// Mencatat satu hasil test. Dipakai oleh makro UJI di bawah.
static void catatHasil(const string& nama, bool lulus,
                       const string& keterangan) {
    total_tests++;
    if (lulus) {
        passed_tests++;
        cout << COLOR_GREEN << "  [PASS]" << COLOR_RESET << " " << nama
                  << endl;
    } else {
        failed_tests++;
        cout << COLOR_RED << "  [FAIL]" << COLOR_RESET << " " << nama
                  << endl;
        cout << "         Keterangan: " << keterangan << endl;
    }
    record_test(nama, lulus, lulus ? "" : keterangan);
    simpanHasilSementara();
}

// =============================================================================
// Menjalankan satu test di dalam proses anak dengan batas waktu
// =============================================================================

// Batas waktu satu test. Implementasi yang benar selesai dalam hitungan
// milidetik; batas ini semata-mata menangkap perulangan yang tidak berhenti.
static const int BATAS_DETIK = 5;

// Batas pemakaian memori proses anak (512 MB), supaya perulangan yang terus
// menumpuk teks tidak menghabiskan memori runner sebelum batas waktu tercapai.
static const rlim_t BATAS_MEMORI = static_cast<rlim_t>(512) * 1024 * 1024;

// Batas panjang teks hasil, jauh di bawah kapasitas pipe (64 KB) sehingga anak
// tidak pernah terhalang saat menulis.
static const size_t BATAS_KELUARAN = 8000;

enum KeadaanAnak { ANAK_SELESAI, ANAK_WAKTU_HABIS, ANAK_BERHENTI };

struct HasilAnak {
    KeadaanAnak keadaan;
    string teks;
    int penyebab;   // nomor sinyal atau kode keluar, sesuai keadaan
};

typedef string (*FungsiUji)();

static HasilAnak jalankanTerisolasi(FungsiUji uji) {
    HasilAnak hasil;
    hasil.keadaan = ANAK_BERHENTI;
    hasil.penyebab = 0;

    int pipa[2];
    if (pipe(pipa) != 0) {
        hasil.teks = "(checker gagal menyiapkan pipe)";
        return hasil;
    }

    cout.flush();
    cerr.flush();

    pid_t anak = fork();
    if (anak < 0) {
        close(pipa[0]);
        close(pipa[1]);
        hasil.teks = "(checker gagal membuat proses anak)";
        return hasil;
    }

    if (anak == 0) {
        // ---------------------------------------------------------------
        // Proses anak: di sinilah fungsi mahasiswa benar-benar dipanggil.
        // ---------------------------------------------------------------
        close(pipa[0]);

        struct rlimit batas;
        batas.rlim_cur = BATAS_MEMORI;
        batas.rlim_max = BATAS_MEMORI;
        setrlimit(RLIMIT_AS, &batas);

        string keluaran;
        try {
            keluaran = uji();
        } catch (const exception& e) {
            keluaran = string("(program melempar exception: ") + e.what() + ")";
        } catch (...) {
            keluaran = "(program melempar exception)";
        }
        if (keluaran.size() > BATAS_KELUARAN) {
            keluaran.resize(BATAS_KELUARAN);
            keluaran += "...(dipotong)";
        }

        const char* data = keluaran.c_str();
        size_t sisa = keluaran.size();
        while (sisa > 0) {
            ssize_t ditulis = write(pipa[1], data, sisa);
            if (ditulis <= 0) break;
            data += ditulis;
            sisa -= static_cast<size_t>(ditulis);
        }
        close(pipa[1]);
        _exit(0);
    }

    // -------------------------------------------------------------------
    // Proses induk: menunggu anak, dengan batas waktu.
    // -------------------------------------------------------------------
    close(pipa[1]);

    int status = 0;
    bool berakhir = false;
    for (int i = 0; i < BATAS_DETIK * 100; ++i) {
        pid_t hasilTunggu = waitpid(anak, &status, WNOHANG);
        if (hasilTunggu == anak) { berakhir = true; break; }
        if (hasilTunggu < 0) { berakhir = true; break; }
        struct timespec jeda;
        jeda.tv_sec = 0;
        jeda.tv_nsec = 10L * 1000L * 1000L;   // 10 ms
        nanosleep(&jeda, NULL);
    }

    if (!berakhir) {
        kill(anak, SIGKILL);
        waitpid(anak, &status, 0);
    }

    string teks;
    char penyangga[4096];
    ssize_t dibaca;
    while ((dibaca = read(pipa[0], penyangga, sizeof(penyangga))) > 0) {
        teks.append(penyangga, static_cast<size_t>(dibaca));
    }
    close(pipa[0]);

    if (!berakhir) {
        hasil.keadaan = ANAK_WAKTU_HABIS;
        hasil.penyebab = BATAS_DETIK;
    } else if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        hasil.keadaan = ANAK_SELESAI;
    } else if (WIFSIGNALED(status)) {
        hasil.keadaan = ANAK_BERHENTI;
        hasil.penyebab = WTERMSIG(status);
    } else {
        hasil.keadaan = ANAK_BERHENTI;
        hasil.penyebab = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
    hasil.teks = teks;
    return hasil;
}

// Menerjemahkan hasil proses anak menjadi teks "Got" yang dibaca manusia.
static string bacaHasil(const HasilAnak& hasil) {
    switch (hasil.keadaan) {
        case ANAK_SELESAI:
            return hasil.teks;
        case ANAK_WAKTU_HABIS: {
            ostringstream out;
            out << "(operasi tidak pernah berhenti — dihentikan setelah "
                << hasil.penyebab << " detik; kemungkinan perulangan yang "
                   "tidak menemukan titik berhenti)";
            return out.str();
        }
        default: {
            ostringstream out;
            out << "(program berhenti tidak wajar";
            if (hasil.penyebab == SIGSEGV) {
                out << ": mengakses memori yang tidak sah (SIGSEGV), mis. "
                       "menulis di luar jangkauan array";
            } else if (hasil.penyebab == SIGABRT) {
                out << ": program dihentikan paksa (SIGABRT)";
            } else if (hasil.penyebab != 0) {
                out << ": sinyal/kode " << hasil.penyebab;
            }
            out << ")";
            if (!hasil.teks.empty()) out << " keluaran sebagian: " << hasil.teks;
            return out.str();
        }
    }
}

// Makro utama: jalankan `fungsi` secara terisolasi, bandingkan teks hasilnya
// dengan `harapan`.
#define UJI(nama, fungsi, harapan) do { \
    HasilAnak _h = jalankanTerisolasi(fungsi); \
    string _dapat = bacaHasil(_h); \
    string _harap = (harapan); \
    bool _ok = (_h.keadaan == ANAK_SELESAI) && (_dapat == _harap); \
    ostringstream _pesan; \
    _pesan << "Expected: " << _harap << " | Got: " << _dapat; \
    catatHasil((nama), _ok, _pesan.str()); \
} while (0)

// =============================================================================
// Utilitas antrean milik checker (dipakai di dalam proses anak)
// =============================================================================

static const char* bo(bool nilai) { return nilai ? "true" : "false"; }

// Batas penelusuran rantai node. Implementasi yang salah dapat membentuk rantai
// yang menunjuk balik ke dirinya sendiri; batas ini membuat keadaan itu
// dilaporkan sebagai FAIL yang terbaca, bukan sebagai penelusuran tanpa henti.
static const int BATAS_TELUSUR = 300;

// Menyiapkan antrean uji secara langsung dengan merangkai node sendiri, tanpa
// memakai satu pun fungsi mahasiswa. `nomor` diurutkan dari dokumen paling
// DEPAN ke dokumen paling BELAKANG, sehingga nomor[0] menjadi yang terdepan.
static void buatQueue(Queue& q, const int* nomor, int n) {
    q.depan = NULL;
    q.belakang = NULL;

    for (int i = 0; i < n; ++i) {
        Node* baru = new Node;
        baru->data = nomor[i];
        baru->next = NULL;
        if (q.belakang == NULL) {
            q.depan = baru;
            q.belakang = baru;
        } else {
            q.belakang->next = baru;
            q.belakang = baru;
        }
    }
}

// Melepas seluruh node antrean tanpa memakai clear() mahasiswa. Dipakai untuk
// membereskan antrean bantu supaya hitungan memori test lain tidak terganggu.
static void bereskan(Queue& q) {
    Node* p = q.depan;
    int langkah = 0;
    while (p != NULL && langkah < BATAS_TELUSUR) {
        Node* berikut = p->next;
        delete p;
        p = berikut;
        ++langkah;
    }
    q.depan = NULL;
    q.belakang = NULL;
}

// Apakah KEDUA penunjuk masih sepakat satu sama lain.
//   - antrean kosong  : keduanya harus NULL
//   - antrean berisi  : node terakhir yang dicapai dari `depan` harus node yang
//                       sama dengan yang ditunjuk `belakang`, dan
//                       `belakang->next` harus NULL
// Inilah yang menangkap `belakang` yang menggantung atau tidak dipindahkan,
// yaitu kesalahan yang tidak terlihat dari isi antrean.
static bool ujungSepakat(const Queue& q) {
    if (q.depan == NULL) return q.belakang == NULL;
    if (q.belakang == NULL) return false;

    Node* p = q.depan;
    int langkah = 0;
    while (p->next != NULL) {
        p = p->next;
        if (++langkah > BATAS_TELUSUR) return false;
    }
    return p == q.belakang && q.belakang->next == NULL;
}

// Ringkasan isi sebuah antrean: dokumen dari depan ke belakang beserta
// banyaknya. Inilah teks yang dibandingkan dengan harapan.
static string gambarQueue(const Queue& q) {
    int nomor[BATAS_TELUSUR];
    int banyak = 0;

    for (Node* p = q.depan; p != NULL; p = p->next) {
        if (banyak >= BATAS_TELUSUR) {
            ostringstream out;
            out << "isi(depan->belakang)=[?] banyak=lebih dari "
                << BATAS_TELUSUR
                << " (rantai node tidak berujung — kemungkinan menunjuk balik "
                   "ke node sebelumnya)";
            return out.str();
        }
        nomor[banyak] = p->data;
        banyak++;
    }

    ostringstream out;
    out << "isi(depan->belakang)=[";
    for (int i = 0; i < banyak; ++i) {
        if (i > 0) out << " ";
        out << nomor[i];
    }
    out << "] banyak=" << banyak;
    return out.str();
}

// Ringkasan kedua UJUNG antrean. Dibaca langsung dari struct, sehingga
// `belakang` yang salah tertangkap walaupun isi antrean kebetulan benar.
static string gambarUjung(const Queue& q) {
    ostringstream out;
    out << "depan=";
    if (q.depan == NULL) out << "(kosong)"; else out << q.depan->data;
    out << " belakang=";
    if (q.belakang == NULL) out << "(kosong)"; else out << q.belakang->data;
    out << " sepakat=" << bo(ujungSepakat(q));
    return out.str();
}

// Nilai yang ditaruh di variabel penerima sebelum dequeue dipanggil. Bila
// operasinya gagal, nilai ini harus masih utuh sesudahnya.
static const int NILAI_AWAL = -999;

// =============================================================================
// SOAL 1 — enqueue  (7 test = 35 poin)
// =============================================================================

// Antrean kosong: kedua penunjuk harus ikut terisi. Lalu satu dokumen lagi:
// `depan` tidak boleh berpindah, `belakang` harus berpindah.
static string uji01() {
    Queue q;
    buatQueue(q, NULL, 0);

    ostringstream out;
    bool r1 = enqueue(q, 101);
    out << "eq101: ret=" << bo(r1) << " " << gambarQueue(q) << " "
        << gambarUjung(q);
    bool r2 = enqueue(q, 102);
    out << " | eq102: ret=" << bo(r2) << " " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Berturut-turut dari kosong, termasuk nomor negatif dan nol.
static string uji02() {
    Queue q;
    buatQueue(q, NULL, 0);

    const int urutan[] = {101, 102, 103, -5, 0};
    bool semua = true;
    for (int i = 0; i < 5; ++i) {
        if (!enqueue(q, urutan[i])) semua = false;
    }

    ostringstream out;
    out << "semuaBerhasil=" << (semua ? "ya" : "tidak") << " " << gambarQueue(q)
        << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// TIDAK ADA KAPASITAS: penambahan sebanyak apa pun harus tetap diterima dan
// tersimpan seluruhnya. Test ini menangkap implementasi yang masih membawa
// batas kapasitas dari queue berbasis array.
static string uji03() {
    Queue q;
    buatQueue(q, NULL, 0);

    bool semua = true;
    for (int i = 1; i <= 12; ++i) {
        if (!enqueue(q, i)) semua = false;
    }

    ostringstream out;
    out << "semuaBerhasil=" << (semua ? "ya" : "tidak") << " " << gambarQueue(q)
        << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Pada antrean yang SUDAH BERISI, `depan` tidak boleh berpindah sedikit pun,
// dan dokumen lama harus tetap berurutan di depan dokumen baru.
static string uji04() {
    const int awal[] = {7, 3};
    Queue q;
    buatQueue(q, awal, 2);

    ostringstream out;
    bool r1 = enqueue(q, 9);
    out << "eq9: ret=" << bo(r1) << " " << gambarQueue(q) << " "
        << gambarUjung(q);
    bool r2 = enqueue(q, -4);
    out << " | eq-4: ret=" << bo(r2) << " " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Setiap pemanggilan menyediakan TEPAT SATU node baru, dan antrean yang sangat
// panjang tetap tertampung seluruhnya.
static string uji05() {
    ostringstream out;

    {
        Queue q;
        buatQueue(q, NULL, 0);

        const int urutan[] = {11, 22, 33, 44, 55};
        long long sebelum = blokHidup();
        bool semua = true;
        for (int i = 0; i < 5; ++i) {
            if (!enqueue(q, urutan[i])) semua = false;
        }
        long long sesudah = blokHidup();

        out << "lima: semuaTrue=" << bo(semua) << " nodeBaru="
            << (sesudah - sebelum) << "/5 " << gambarQueue(q);
        bereskan(q);
    }

    {
        Queue q;
        buatQueue(q, NULL, 0);

        bool semua = true;
        for (int i = 0; i < 150; ++i) {
            if (!enqueue(q, i)) semua = false;
        }

        int banyak = 0;
        for (Node* p = q.depan; p != NULL && banyak <= BATAS_TELUSUR;
             p = p->next) {
            banyak++;
        }

        out << " | dalam: semuaTrue=" << bo(semua) << " banyak=" << banyak
            << " " << gambarUjung(q);
        bereskan(q);
    }

    return out.str();
}

// Kasus batas: antrean yang berisi TEPAT SATU dokumen, yaitu keadaan di mana
// `depan` dan `belakang` menunjuk node yang sama.
static string uji06() {
    const int awal[] = {201};
    Queue q;
    buatQueue(q, awal, 1);

    ostringstream out;
    bool r1 = enqueue(q, 202);
    out << "eq202: ret=" << bo(r1) << " " << gambarQueue(q) << " "
        << gambarUjung(q);
    bool r2 = enqueue(q, 203);
    out << " | eq203: ret=" << bo(r2) << " " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Pada antrean yang sudah panjang, penambahan tetap menaruh dokumen di ujung
// belakang dan tetap hanya membuat satu node.
static string uji07() {
    const int awal[] = {10, 20, 30, 40, 50, 60, 70, 80};
    Queue q;
    buatQueue(q, awal, 8);

    long long sebelum = blokHidup();
    bool r = enqueue(q, 90);
    long long sesudah = blokHidup();

    ostringstream out;
    out << "eq90: ret=" << bo(r) << " nodeBaru=" << (sesudah - sebelum)
        << "/1 " << gambarQueue(q) << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// =============================================================================
// SOAL 2 — dequeue  (8 test = 40 poin)
// =============================================================================

// Yang keluar adalah dokumen TERDEPAN, penerima terisi, dan tepat satu node
// dibuang. Diperiksa dua kali berturut-turut.
static string uji08() {
    const int awal[] = {101, 102, 103};
    Queue q;
    buatQueue(q, awal, 3);

    ostringstream out;

    // Keadaan dicatat SEGERA sesudah pengambilan pertama, sebelum pengambilan
    // kedua dijalankan.
    int n1 = NILAI_AWAL;
    long long sebelum1 = blokHidup();
    bool r1 = dequeue(q, n1);
    long long sesudah1 = blokHidup();

    out << "ret=" << bo(r1) << " nomor=" << n1 << " dibuang="
        << (sebelum1 - sesudah1) << "/1 " << gambarQueue(q);

    int n2 = NILAI_AWAL;
    long long sebelum2 = blokHidup();
    bool r2 = dequeue(q, n2);
    long long sesudah2 = blokHidup();

    out << " | lagi: ret=" << bo(r2) << " nomor=" << n2 << " dibuangLagi="
        << (sebelum2 - sesudah2) << "/1 " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// FIFO: beberapa pengambilan berturut-turut keluar dalam URUTAN MASUK.
// Bandingkan dengan Pertemuan 5, yang mengeluarkannya secara terbalik.
static string uji09() {
    const int awal[] = {101, 102, 103, 104};
    Queue q;
    buatQueue(q, awal, 4);

    int keluar[3];
    bool semua = true;
    long long sebelum = blokHidup();
    for (int i = 0; i < 3; ++i) {
        keluar[i] = NILAI_AWAL;
        if (!dequeue(q, keluar[i])) semua = false;
    }
    long long sesudah = blokHidup();

    ostringstream out;
    out << "keluar=[";
    for (int i = 0; i < 3; ++i) {
        if (i > 0) out << " ";
        out << keluar[i];
    }
    out << "] semuaBerhasil=" << (semua ? "ya" : "tidak") << " dibuang="
        << (sebelum - sesudah) << "/3 " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Dokumen TERAKHIR keluar: `depan` DAN `belakang` harus sama-sama menjadi
// NULL. Inilah kesalahan paling sering pada queue — `belakang` yang
// tertinggal menunjuk node yang sudah dilepas.
static string uji10() {
    const int awal[] = {101, 102};
    Queue q;
    buatQueue(q, awal, 2);

    int keluar[2];
    bool semua = true;
    long long sebelum = blokHidup();
    for (int i = 0; i < 2; ++i) {
        keluar[i] = NILAI_AWAL;
        if (!dequeue(q, keluar[i])) semua = false;
    }
    long long sesudah = blokHidup();

    ostringstream out;
    out << "keluar=[" << keluar[0] << " " << keluar[1] << "] semuaBerhasil="
        << (semua ? "ya" : "tidak") << " dibuang=" << (sebelum - sesudah)
        << "/2 " << gambarQueue(q) << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// UNDERFLOW: pada antrean kosong, kembaliannya false dan variabel penerima
// tidak boleh disentuh sama sekali. Disertai kontrol positif supaya fungsi yang
// selalu mengembalikan false tidak ikut lolos.
static string uji11() {
    ostringstream out;

    {
        Queue q;
        buatQueue(q, NULL, 0);

        int n = NILAI_AWAL;
        bool r = dequeue(q, n);

        out << "kosong: ret=" << bo(r) << " nomor=" << n << " "
            << gambarQueue(q) << " " << gambarUjung(q);
        bereskan(q);
    }

    {
        const int awal[] = {101, 102, 103};
        Queue q;
        buatQueue(q, awal, 3);

        int n = NILAI_AWAL;
        bool r = dequeue(q, n);

        out << " | kontrol: ret=" << bo(r) << " nomor=" << n << " "
            << gambarQueue(q);
        bereskan(q);
    }

    return out.str();
}

// Kasus batas: antrean yang berisi TEPAT SATU dokumen, yaitu keadaan di mana
// `depan` dan `belakang` menunjuk node yang sama sebelum pengambilan.
static string uji12() {
    const int awal[] = {777};
    Queue q;
    buatQueue(q, awal, 1);

    int n = NILAI_AWAL;
    long long sebelum = blokHidup();
    bool r = dequeue(q, n);
    long long sesudah = blokHidup();

    ostringstream out;
    out << "ret=" << bo(r) << " nomor=" << n << " dibuang="
        << (sebelum - sesudah) << "/1 " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// RANGKAIAN CAMPURAN (memakai enqueue mahasiswa — pengecualian yang disengaja).
// Masuk 101, 102, 103; ambil satu; masuk 104; ambil sampai habis. Urutan keluar
// harus 101, 102, 103, 104 — sesuai urutan masuk.
static string uji13() {
    Queue q;
    buatQueue(q, NULL, 0);

    int keluar[4];
    int banyakKeluar = 0;
    bool semua = true;

    enqueue(q, 101);
    enqueue(q, 102);
    enqueue(q, 103);

    keluar[banyakKeluar] = NILAI_AWAL;
    if (!dequeue(q, keluar[banyakKeluar])) semua = false;
    banyakKeluar++;

    enqueue(q, 104);

    for (int i = 0; i < 3; ++i) {
        keluar[banyakKeluar] = NILAI_AWAL;
        if (!dequeue(q, keluar[banyakKeluar])) semua = false;
        banyakKeluar++;
    }

    ostringstream out;
    out << "keluar=[";
    for (int i = 0; i < 4; ++i) {
        if (i > 0) out << " ";
        out << keluar[i];
    }
    out << "] semuaBerhasil=" << (semua ? "ya" : "tidak") << " "
        << gambarQueue(q) << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// RANGKAIAN CAMPURAN (memakai enqueue mahasiswa — pengecualian yang disengaja).
// Antrean dikosongkan sampai habis, lalu tombol Cetak masih ditekan sekali
// lagi, lalu antrean DIPAKAI LAGI. Ini yang memperlihatkan akibat `belakang`
// yang tidak dikosongkan pada langkah sebelumnya.
static string uji14() {
    Queue q;
    buatQueue(q, NULL, 0);

    for (int i = 1; i <= 5; ++i) enqueue(q, i);

    int keluar[5];
    for (int i = 0; i < 5; ++i) {
        keluar[i] = NILAI_AWAL;
        dequeue(q, keluar[i]);
    }

    int nKosong = NILAI_AWAL;
    bool rKosong = dequeue(q, nKosong);

    enqueue(q, 8);
    enqueue(q, 9);

    ostringstream out;
    out << "keluar=[";
    for (int i = 0; i < 5; ++i) {
        if (i > 0) out << " ";
        out << keluar[i];
    }
    out << "] deqSaatKosong=" << bo(rKosong) << " nomor=" << nKosong;
    out << " | setelahDipakaiLagi: " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Nomor nol, negatif, dan yang muncul berulang harus keluar utuh dan tetap
// berurutan.
static string uji15() {
    const int awal[] = {0, -7, -7, 42};
    Queue q;
    buatQueue(q, awal, 4);

    int keluar[4];
    bool semua = true;
    long long sebelum = blokHidup();
    for (int i = 0; i < 4; ++i) {
        keluar[i] = NILAI_AWAL;
        if (!dequeue(q, keluar[i])) semua = false;
    }
    long long sesudah = blokHidup();

    ostringstream out;
    out << "keluar=[";
    for (int i = 0; i < 4; ++i) {
        if (i > 0) out << " ";
        out << keluar[i];
    }
    out << "] semuaBerhasil=" << (semua ? "ya" : "tidak") << " dibuang="
        << (sebelum - sesudah) << "/4 " << gambarQueue(q) << " "
        << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// =============================================================================
// SOAL 3 — clear  (5 test = 25 poin)
// =============================================================================

// Mengosongkan antrean, kedua penunjuk kembali NULL, dan aman dipanggil
// dua kali berturut-turut.
static string uji16() {
    const int awal[] = {101, 102, 103};
    Queue q;
    buatQueue(q, awal, 3);

    clear(q);

    ostringstream out;
    out << "setelahClear: " << gambarQueue(q) << " " << gambarUjung(q);

    clear(q);
    out << " | clearLagi: " << gambarQueue(q) << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Banyaknya node yang dibebaskan dihitung TEPAT. Memutus sambungan tanpa
// delete akan terlihat di sini walaupun antreannya tampak sudah kosong.
static string uji17() {
    ostringstream out;

    {
        const int awal[] = {55};
        Queue q;
        buatQueue(q, awal, 1);

        long long sebelum = blokHidup();
        clear(q);
        long long sesudah = blokHidup();

        out << "satu: dibebaskan=" << (sebelum - sesudah) << "/1 "
            << gambarUjung(q);
        bereskan(q);
    }

    {
        int awal[10];
        for (int i = 0; i < 10; ++i) awal[i] = 100 + i;
        Queue q;
        buatQueue(q, awal, 10);

        long long sebelum = blokHidup();
        clear(q);
        long long sesudah = blokHidup();

        out << " | sepuluh: dibebaskan=" << (sebelum - sesudah) << "/10 "
            << gambarUjung(q);
        bereskan(q);
    }

    return out.str();
}

// Skala besar: seluruh 150 node harus dibebaskan, bukan hanya sebagian.
static string uji18() {
    int awal[150];
    for (int i = 0; i < 150; ++i) awal[i] = i;
    Queue q;
    buatQueue(q, awal, 150);

    long long sebelum = blokHidup();
    clear(q);
    long long sesudah = blokHidup();

    ostringstream out;
    out << "dibebaskan=" << (sebelum - sesudah) << "/150 " << gambarQueue(q)
        << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Pada antrean yang SUDAH kosong, tidak ada node yang dibebaskan dan
// pemanggilannya tetap aman — termasuk bila dipanggil sekali lagi.
//
// Diawali KONTROL POSITIF pada antrean berisi. Tanpa kontrol itu, `clear` yang
// badannya masih kosong akan ikut lolos, karena pada antrean kosong tidak
// melakukan apa-apa memang jawaban yang benar.
static string uji19() {
    ostringstream out;

    {
        const int awal[] = {11, 22, 33};
        Queue q;
        buatQueue(q, awal, 3);

        long long sebelum = blokHidup();
        clear(q);
        long long sesudah = blokHidup();

        out << "kontrol: dibebaskan=" << (sebelum - sesudah) << "/3 "
            << gambarQueue(q) << " " << gambarUjung(q) << " | ";
        bereskan(q);
    }

    Queue q;
    buatQueue(q, NULL, 0);

    long long sebelum1 = blokHidup();
    clear(q);
    long long sesudah1 = blokHidup();

    out << "kosong: dibebaskan=" << (sebelum1 - sesudah1) << "/0 "
        << gambarUjung(q);

    long long sebelum2 = blokHidup();
    clear(q);
    long long sesudah2 = blokHidup();

    out << " | clearKedua: dibebaskan=" << (sebelum2 - sesudah2) << "/0 "
        << gambarQueue(q) << " " << gambarUjung(q);

    bereskan(q);
    return out.str();
}

// Menyapu seluruh ukuran batas dalam satu test: nol, satu, dua, dan lima
// dokumen. Ukuran satu adalah keadaan di mana kedua penunjuk menunjuk node
// yang sama.
static string uji20() {
    const int ukuran[] = {0, 1, 2, 5};
    const char* label[] = {"n0", "n1", "n2", "n5"};

    ostringstream out;
    for (int k = 0; k < 4; ++k) {
        int awal[5];
        for (int i = 0; i < ukuran[k]; ++i) awal[i] = 300 + i;

        Queue q;
        buatQueue(q, awal, ukuran[k]);

        long long sebelum = blokHidup();
        clear(q);
        long long sesudah = blokHidup();

        if (k > 0) out << " | ";
        out << label[k] << ": dibebaskan=" << (sebelum - sesudah) << "/"
            << ukuran[k] << " " << gambarUjung(q);

        bereskan(q);
    }

    return out.str();
}
static void suiteSoal1() {
    cout << COLOR_CYAN << COLOR_BOLD
              << "\n[TEST SUITE] Soal 1 — enqueue()   (dokumen baru di belakang)"
              << COLOR_RESET << endl;

    UJI("enqueue pada antrean kosong mengisi kedua penunjuk, lalu penambahan "
        "berikutnya tidak memindahkan depan",
        uji01, "eq101: ret=true isi(depan->belakang)=[101] banyak=1 depan=101 "
        "belakang=101 sepakat=true | eq102: ret=true "
        "isi(depan->belakang)=[101 102] banyak=2 depan=101 belakang=102 "
        "sepakat=true");

    UJI("enqueue berturut-turut dari kosong, termasuk nomor negatif dan nol",
        uji02, "semuaBerhasil=ya isi(depan->belakang)=[101 102 103 -5 0] banyak=5 "
        "depan=101 belakang=0 sepakat=true");

    UJI("enqueue tidak pernah ditolak — linked list tidak punya kapasitas tetap",
        uji03, "semuaBerhasil=ya isi(depan->belakang)=[1 2 3 4 5 6 7 8 9 10 11 12] "
        "banyak=12 depan=1 belakang=12 sepakat=true");

    UJI("enqueue pada antrean yang sudah berisi menjaga urutan dokumen lama dan "
        "tidak memindahkan depan",
        uji04, "eq9: ret=true isi(depan->belakang)=[7 3 9] banyak=3 depan=7 "
        "belakang=9 sepakat=true | eq-4: ret=true isi(depan->belakang)=[7 3 9 "
        "-4] banyak=4 depan=7 belakang=-4 sepakat=true");

    UJI("enqueue menyediakan tepat satu node per pemanggilan, dan antrean yang "
        "sangat panjang tetap tertampung",
        uji05, "lima: semuaTrue=true nodeBaru=5/5 isi(depan->belakang)=[11 22 33 44 "
        "55] banyak=5 | dalam: semuaTrue=true banyak=150 depan=0 belakang=149 "
        "sepakat=true");

    UJI("enqueue pada antrean berisi satu dokumen, yaitu saat kedua penunjuk "
        "menunjuk node yang sama",
        uji06, "eq202: ret=true isi(depan->belakang)=[201 202] banyak=2 depan=201 "
        "belakang=202 sepakat=true | eq203: ret=true "
        "isi(depan->belakang)=[201 202 203] banyak=3 depan=201 belakang=203 "
        "sepakat=true");

    UJI("enqueue pada antrean panjang tetap menaruh dokumen di ujung belakang "
        "dan tetap hanya membuat satu node",
        uji07, "eq90: ret=true nodeBaru=1/1 isi(depan->belakang)=[10 20 30 40 50 60 "
        "70 80 90] banyak=9 depan=10 belakang=90 sepakat=true");
}

static void suiteSoal2() {
    cout << COLOR_CYAN << COLOR_BOLD
              << "\n[TEST SUITE] Soal 2 — dequeue()   (printer mengambil terdepan)"
              << COLOR_RESET << endl;

    UJI("dequeue mengeluarkan dokumen terdepan, mengisi variabel penerima, dan "
        "membuang tepat satu node",
        uji08, "ret=true nomor=101 dibuang=1/1 isi(depan->belakang)=[102 103] "
        "banyak=2 | lagi: ret=true nomor=102 dibuangLagi=1/1 "
        "isi(depan->belakang)=[103] banyak=1 depan=103 belakang=103 "
        "sepakat=true");

    UJI("beberapa dequeue berturut-turut keluar dalam urutan masuknya (FIFO)",
        uji09, "keluar=[101 102 103] semuaBerhasil=ya dibuang=3/3 "
        "isi(depan->belakang)=[104] banyak=1 depan=104 belakang=104 "
        "sepakat=true");

    UJI("dokumen terakhir keluar: depan DAN belakang sama-sama menjadi kosong",
        uji10, "keluar=[101 102] semuaBerhasil=ya dibuang=2/2 "
        "isi(depan->belakang)=[] banyak=0 depan=(kosong) belakang=(kosong) "
        "sepakat=true");

    UJI("dequeue pada antrean kosong gagal tanpa mengubah apa pun (underflow)",
        uji11, "kosong: ret=false nomor=-999 isi(depan->belakang)=[] banyak=0 "
        "depan=(kosong) belakang=(kosong) sepakat=true | kontrol: ret=true "
        "nomor=101 isi(depan->belakang)=[102 103] banyak=2");

    UJI("dequeue pada antrean berisi satu dokumen, yaitu saat kedua penunjuk "
        "menunjuk node yang sama",
        uji12, "ret=true nomor=777 dibuang=1/1 isi(depan->belakang)=[] banyak=0 "
        "depan=(kosong) belakang=(kosong) sepakat=true");

    UJI("rangkaian campuran: masuk tiga, ambil satu, masuk satu lagi, lalu "
        "ambil sampai habis",
        uji13, "keluar=[101 102 103 104] semuaBerhasil=ya isi(depan->belakang)=[] "
        "banyak=0 depan=(kosong) belakang=(kosong) sepakat=true");

    UJI("antrean dikosongkan sampai habis, tombol Cetak ditekan sekali lagi, "
        "lalu antrean dipakai lagi",
        uji14, "keluar=[1 2 3 4 5] deqSaatKosong=false nomor=-999 | "
        "setelahDipakaiLagi: isi(depan->belakang)=[8 9] banyak=2 depan=8 "
        "belakang=9 sepakat=true");

    UJI("nomor nol, negatif, dan yang berulang keluar utuh dan tetap berurutan",
        uji15, "keluar=[0 -7 -7 42] semuaBerhasil=ya dibuang=4/4 "
        "isi(depan->belakang)=[] banyak=0 depan=(kosong) belakang=(kosong) "
        "sepakat=true");
}

static void suiteSoal3() {
    cout << COLOR_CYAN << COLOR_BOLD
              << "\n[TEST SUITE] Soal 3 — clear()   (tombol Batalkan Semua)"
              << COLOR_RESET << endl;

    UJI("clear mengosongkan antrean beserta kedua penunjuknya, dan aman "
        "dipanggil dua kali",
        uji16, "setelahClear: isi(depan->belakang)=[] banyak=0 depan=(kosong) "
        "belakang=(kosong) sepakat=true | clearLagi: isi(depan->belakang)=[] "
        "banyak=0 depan=(kosong) belakang=(kosong) sepakat=true");

    UJI("clear membebaskan node dalam jumlah yang tepat, baik pada satu dokumen "
        "maupun pada sepuluh dokumen",
        uji17, "satu: dibebaskan=1/1 depan=(kosong) belakang=(kosong) sepakat=true | "
        "sepuluh: dibebaskan=10/10 depan=(kosong) belakang=(kosong) "
        "sepakat=true");

    UJI("clear pada antrean 150 dokumen membebaskan seluruhnya",
        uji18, "dibebaskan=150/150 isi(depan->belakang)=[] banyak=0 depan=(kosong) "
        "belakang=(kosong) sepakat=true");

    UJI("clear pada antrean yang sudah kosong tidak membebaskan node apa pun "
        "dan tetap aman",
        uji19,
        "kontrol: dibebaskan=3/3 isi(depan->belakang)=[] banyak=0 "
        "depan=(kosong) belakang=(kosong) sepakat=true "
        "| kosong: dibebaskan=0/0 depan=(kosong) belakang=(kosong) sepakat=true "
        "| clearKedua: dibebaskan=0/0 isi(depan->belakang)=[] banyak=0 "
        "depan=(kosong) belakang=(kosong) sepakat=true");

    UJI("clear benar pada seluruh ukuran batas: nol, satu, dua, dan lima "
        "dokumen",
        uji20, "n0: dibebaskan=0/0 depan=(kosong) belakang=(kosong) sepakat=true | "
        "n1: dibebaskan=1/1 depan=(kosong) belakang=(kosong) sepakat=true | "
        "n2: dibebaskan=2/2 depan=(kosong) belakang=(kosong) sepakat=true | "
        "n5: dibebaskan=5/5 depan=(kosong) belakang=(kosong) sepakat=true");
}

// =============================================================================
// Main
// =============================================================================

int main() {
    cout << COLOR_BOLD
              << "============================================" << endl;
    cout << " Praktikum Struktur Data C++ — Auto Checker" << endl;
    cout << " Pertemuan 6: Study Case Antrean Cetak Bersama" << endl;
    cout << "============================================"
              << COLOR_RESET << endl;

    suiteSoal1();     // enqueue   7 test = 35 poin
    suiteSoal2();     // dequeue   8 test = 40 poin
    suiteSoal3();     // clear     5 test = 25 poin

    // Pengaman untuk instruktur: bobot per test dihitung dari angka rencana,
    // jadi jumlah test yang benar-benar berjalan harus sama dengan rencana.
    if (total_tests != TOTAL_TEST_DIRENCANAKAN) {
        cerr << "PERINGATAN (instruktur): jumlah test berjalan ("
                  << total_tests << ") tidak sama dengan rencana ("
                  << TOTAL_TEST_DIRENCANAKAN << ")." << endl;
    }

    // -----------------------------------------------------------------------
    // Scoring Summary
    // -----------------------------------------------------------------------
    int score = (total_tests > 0) ? (passed_tests * 100 / total_tests) : 0;

    cout << "\n" << COLOR_BOLD
              << "============================================\n"
              << " SCORING SUMMARY\n"
              << "============================================\n"
              << COLOR_RESET;

    cout << " Tests Berhasil : " << COLOR_GREEN << COLOR_BOLD
              << passed_tests << COLOR_RESET << " / " << total_tests << "\n";
    cout << " Tests Gagal    : " << COLOR_RED << COLOR_BOLD
              << failed_tests << COLOR_RESET << " / " << total_tests << "\n";

    // Score line — warna hijau jika sempurna, kuning jika sebagian, merah jika 0
    string score_color = (score == 100) ? COLOR_GREEN
                            : (score > 0)    ? COLOR_YELLOW
                                             : COLOR_RED;
    cout << " Score          : " << score_color << COLOR_BOLD
              << score << " / 100" << COLOR_RESET << "\n";

    cout << COLOR_BOLD
              << "============================================\n"
              << COLOR_RESET;

    // -----------------------------------------------------------------------
    // Hasil yang dapat dibaca mesin.
    // Berkas inilah yang diunggah sebagai artifact dan dibaca aplikasi web.
    // -----------------------------------------------------------------------
    if (!write_result_json("result.json", score)) {
        cerr << "PERINGATAN: gagal menulis result.json" << endl;
    }

    if (failed_tests == 0) {
        cout << COLOR_GREEN << COLOR_BOLD
                  << " STATUS: SEMUA TEST BERHASIL ✓\n"
                  << COLOR_RESET;
        return 0; // exit code 0 = GitHub Actions SUCCESS
    } else {
        cout << COLOR_RED << COLOR_BOLD
                  << " STATUS: " << failed_tests << " TEST GAGAL ✗\n"
                  << COLOR_RESET;
        return 1; // exit code non-zero = GitHub Actions FAIL
    }
}
