#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <iomanip>
#include <limits>

using namespace std;

// ======================
// Class Kursi (SeatMap)
// ======================
class Kursi {
private:
    int rows;
    int cols;
    vector<vector<bool>> mapa; // true = terisi, false = kosong

public:
    Kursi(int r = 5, int c = 5) : rows(r), cols(c) {
        mapa.assign(rows, vector<bool>(cols, false));
    }

    void tampil() const {
        cout << "    ";
        for (int j = 0; j < cols; ++j) {
            cout << setw(3) << j + 1;
        }
        cout << "\n";
        for (int i = 0; i < rows; ++i) {
            cout << setw(3) << i + 1 << " ";
            for (int j = 0; j < cols; ++j) {
                cout << (mapa[i][j] ? "[X]" : "[O]");
            }
            cout << "\n";
        }
        cout << "Keterangan: [O] = kosong, [X] = terisi\n";
    }

    bool tersedia(int r, int c) const {
        if (r < 1 || r > rows || c < 1 || c > cols) return false;
        return !mapa[r-1][c-1];
    }

    bool pesan(int r, int c) {
        if (!tersedia(r,c)) return false;
        mapa[r-1][c-1] = true;
        return true;
    }

    void batalPesan(int r, int c) {
        if (r >= 1 && r <= rows && c >= 1 && c <= cols) mapa[r-1][c-1] = false;
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }
};

// ======================
// Class Session (film session)
// ======================
class Session {
private:
    static int nextId;
    int id;
    string judul;
    string jadwal; // bisa berupa string (misal "10:00")
    int studioId;
    int harga;
    Kursi kursi;

public:
    Session(const string& judul_, const string& jadwal_, int studioId_, int harga_, int rows=5, int cols=5)
        : judul(judul_), jadwal(jadwal_), studioId(studioId_), harga(harga_), kursi(rows, cols) {
        id = ++nextId;
    }

    int getId() const { return id; }
    string getJudul() const { return judul; }
    string getJadwal() const { return jadwal; }
    int getStudioId() const { return studioId; }
    int getHarga() const { return harga; }
    Kursi& getKursi() { return kursi; }
    const Kursi& getKursiConst() const { return kursi; }

    void tampilRingkas() const {
        cout << "Session ID: " << id
             << " | Film: " << judul
             << " | Studio: " << studioId
             << " | Jadwal: " << jadwal
             << " | Harga: Rp " << harga << "\n";
    }
};

int Session::nextId = 0;

// ======================
// Class Studio
// ======================
class Studio {
private:
    int id;
    string nama;

public:
    Studio(int id_, const string& nama_): id(id_), nama(nama_) {}
    int getId() const { return id; }
    string getNama() const { return nama; }
    void tampil() const {
        cout << "Studio " << id << " - " << nama << "\n";
    }
};

// ======================
// Class Tiket
// ======================
struct Tiket {
    int noAntrian;
    string namaPembeli;
    int sessionId;
    int baris, kolom;
    int harga;
};

// ======================
// Class Bioskop (Cinema)
// ======================
class Bioskop {
private:
    vector<Studio> studios;
    vector<Session> sessions;
    queue<Tiket> antrian;
    int nextQueueNo;

    // utilities: find session index by id
    int findSessionIndex(int sessionId) {
        for (int i = 0; i < (int)sessions.size(); ++i) {
            if (sessions[i].getId() == sessionId) return i;
        }
        return -1;
    }

public:
    Bioskop(): nextQueueNo(1) {
        // default: buat 3 studio
        studios.emplace_back(1, "Studio Besar");
        studios.emplace_back(2, "Studio Kecil");
        studios.emplace_back(3, "Studio VIP");
    }

    void tampilStudios() const {
        cout << "\nDaftar Studio:\n";
        for (const auto& s : studios) s.tampil();
    }

    void tambahStudio(const string& nama) {
        int id = studios.empty() ? 1 : studios.back().getId() + 1;
        studios.emplace_back(id, nama);
        cout << "Studio ditambahkan: ID " << id << " - " << nama << "\n";
    }

    void tambahSession(const string& judul, const string& jadwal, int studioId, int harga, int rows=5, int cols=5) {
        // cek studio exists
        bool found = false;
        for (const auto& s : studios) if (s.getId() == studioId) { found = true; break; }
        if (!found) {
            cout << "Studio dengan ID " << studioId << " tidak ada!\n";
            return;
        }

        sessions.emplace_back(judul, jadwal, studioId, harga, rows, cols);
        cout << "Session ditambahkan (ID " << sessions.back().getId() << ") untuk film '" << judul << "' di studio " << studioId << ".\n";
    }

    void hapusSession(int sessionId) {
        int idx = findSessionIndex(sessionId);
        if (idx == -1) {
            cout << "Session ID tidak ditemukan.\n";
            return;
        }
        sessions.erase(sessions.begin() + idx);
        cout << "Session dihapus.\n";
    }

    void tampilAllSessions() const {
        if (sessions.empty()) {
            cout << "Belum ada session.\n";
            return;
        }
        cout << "\n===== Semua Session =====\n";
        for (const auto& sess : sessions) sess.tampilRingkas();
    }

    void tampilSessionDetail(int sessionId) {
        int idx = findSessionIndex(sessionId);
        if (idx == -1) {
            cout << "Session ID tidak ditemukan.\n";
            return;
        }
        const Session& s = sessions[idx];
        s.tampilRingkas();
        s.getKursiConst().tampil();
    }

    // User memesan kursi untuk session -> masuk antrian
    void pesanTiket(const string& namaPembeli, int sessionId, int baris, int kolom) {
        int idx = findSessionIndex(sessionId);
        if (idx == -1) {
            cout << "Session ID tidak ditemukan.\n";
            return;
        }

        Session& s = sessions[idx];
        if (!s.getKursi().tersedia(baris, kolom)) {
            cout << "Kursi tidak tersedia. Pesanan gagal.\n";
            return;
        }

        // reserve seat immediately so orang lain tidak mengambil
        bool ok = s.getKursi().pesan(baris, kolom);
        if (!ok) {
            cout << "Gagal memesan kursi (sudah terisi).\n";
            return;
        }

        Tiket t;
        t.noAntrian = nextQueueNo++;
        t.namaPembeli = namaPembeli;
        t.sessionId = sessionId;
        t.baris = baris;
        t.kolom = kolom;
        t.harga = s.getHarga();

        antrian.push(t);
        cout << "Berhasil: Tiket masuk antrian. No Antrian: " << t.noAntrian << "\n";
    }

    void lihatAntrian() const {
        if (antrian.empty()) {
            cout << "Antrian kosong.\n";
            return;
        }
        cout << "\n===== DAFTAR ANTRIAN =====\n";
        queue<Tiket> temp = antrian;
        while (!temp.empty()) {
            const Tiket& t = temp.front();
            cout << "No " << t.noAntrian << " | " << t.namaPembeli
                 << " | SessionID: " << t.sessionId
                 << " | Kursi: (" << t.baris << "," << t.kolom << ")"
                 << " | Harga: Rp " << t.harga << "\n";
            temp.pop();
        }
    }

    // admin/pegawai memanggil (layani) antrian terdepan
    void layaniAntrian() {
        if (antrian.empty()) {
            cout << "Antrian kosong.\n";
            return;
        }
        Tiket t = antrian.front();
        antrian.pop();
        cout << "\n===== MELAYANI PESANAN =====\n";
        cout << "No Antrian: " << t.noAntrian << "\n";
        cout << "Nama: " << t.namaPembeli << "\n";
        cout << "Session ID: " << t.sessionId << "\n";
        cout << "Kursi: Baris " << t.baris << ", Kolom " << t.kolom << "\n";
        cout << "Total Bayar: Rp " << t.harga << "\n";
        cout << "Silakan melakukan pembayaran & cetak tiket.\n";
    }

    // jika admin ingin batalkan antrian teratas dan kembalikan kursinya
    void batalkanAntrianTeratas() {
        if (antrian.empty()) {
            cout << "Antrian kosong.\n";
            return;
        }
        Tiket t = antrian.front();
        antrian.pop();
        int idx = findSessionIndex(t.sessionId);
        if (idx != -1) {
            sessions[idx].getKursi().batalPesan(t.baris, t.kolom);
        }
        cout << "Antrian No " << t.noAntrian << " dibatalkan dan kursi dikembalikan.\n";
    }
};

// ======================
// Helper input safe read
// ======================
void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string& prompt) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x) {
            clearCin();
            return x;
        } else {
            cout << "Input tidak valid. Coba lagi.\n";
            clearCin();
        }
    }
}

string readLine(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

// ======================
// MAIN — UI sederhana
// ======================
int main() {
    Bioskop bioskop;

    // contoh session awal (boleh dihapus oleh admin nanti)
    bioskop.tambahSession("Avengers: Endgame", "10:00", 1, 50000, 5, 5);
    bioskop.tambahSession("Spiderman: No Way Home", "13:00", 2, 45000, 5, 5);
    bioskop.tambahSession("Toy Story 4", "15:30", 3, 35000, 5, 5);

    const string adminPass = "admin123";
    int pilihanMain;

    do {
        cout << "\n===== SISTEM BIOSKOP OOP =====\n";
        cout << "1. Login Admin\n";
        cout << "2. Menu User\n";
        cout << "3. Keluar\n";
        cout << "Pilih: ";
        if (!(cin >> pilihanMain)) {
            clearCin();
            cout << "Input tidak valid.\n";
            continue;
        }
        clearCin();

        if (pilihanMain == 1) {
            string pass = readLine("Masukkan password admin: ");
            if (pass != adminPass) {
                cout << "Password salah.\n";
                continue;
            }

            int pilihanAdmin;
            do {
                cout << "\n--- MENU ADMIN ---\n";
                cout << "1. Lihat Studio\n";
                cout << "2. Tambah Studio\n";
                cout << "3. Lihat Semua Session\n";
                cout << "4. Tambah Session\n";
                cout << "5. Hapus Session\n";
                cout << "6. Tampilkan Detail Session (termasuk kursi)\n";
                cout << "7. Lihat Antrian\n";
                cout << "8. Layani Antrian (panggil)\n";
                cout << "9. Batalkan Antrian Teratas (kembalikan kursi)\n";
                cout << "10. Kembali\n";
                cout << "Pilih: ";
                if (!(cin >> pilihanAdmin)) { clearCin(); cout << "Input tidak valid.\n"; continue; }
                clearCin();

                switch (pilihanAdmin) {
                    case 1:
                        bioskop.tampilStudios();
                        break;
                    case 2: {
                        string nama = readLine("Nama studio baru: ");
                        bioskop.tambahStudio(nama);
                        break;
                    }
                    case 3:
                        bioskop.tampilAllSessions();
                        break;
                    case 4: {
                        string judul = readLine("Judul film: ");
                        string jadwal = readLine("Jadwal (misal 18:30): ");
                        int studioId = readInt("Studio ID: ");
                        int harga = readInt("Harga tiket (Rp): ");
                        int rows = readInt("Jumlah baris kursi: ");
                        int cols = readInt("Jumlah kolom kursi: ");
                        bioskop.tambahSession(judul, jadwal, studioId, harga, rows, cols);
                        break;
                    }
                    case 5: {
                        int sid = readInt("Masukkan Session ID yang ingin dihapus: ");
                        bioskop.hapusSession(sid);
                        break;
                    }
                    case 6: {
                        int sid = readInt("Masukkan Session ID untuk lihat detail: ");
                        bioskop.tampilSessionDetail(sid);
                        break;
                    }
                    case 7:
                        bioskop.lihatAntrian();
                        break;
                    case 8:
                        bioskop.layaniAntrian();
                        break;
                    case 9:
                        bioskop.batalkanAntrianTeratas();
                        break;
                    case 10:
                        break;
                    default:
                        cout << "Pilihan tidak valid.\n";
                }
            } while (pilihanAdmin != 10);

        } else if (pilihanMain == 2) {
            int pilihanUser;
            do {
                cout << "\n--- MENU USER ---\n";
                cout << "1. Lihat Semua Session\n";
                cout << "2. Pilih Session & Lihat Peta Kursi\n";
                cout << "3. Pesan Tiket (pilih kursi manual)\n";
                cout << "4. Lihat Antrian\n";
                cout << "5. Kembali\n";
                cout << "Pilih: ";
                if (!(cin >> pilihanUser)) { clearCin(); cout << "Input tidak valid.\n"; continue; }
                clearCin();

                switch (pilihanUser) {
                    case 1:
                        bioskop.tampilAllSessions();
                        break;
                    case 2: {
                        int sid = readInt("Masukkan Session ID: ");
                        bioskop.tampilSessionDetail(sid);
                        break;
                    }
                    case 3: {
                        string nama = readLine("Masukkan nama Anda: ");
                        int sid = readInt("Masukkan Session ID yang ingin dipesan: ");
                        // tampil kursi untuk sesi tersebut
                        cout << "Menampilkan peta kursi untuk session " << sid << ":\n";
                        bioskop.tampilSessionDetail(sid);

                        int baris = readInt("Pilih baris (1..n): ");
                        int kolom = readInt("Pilih kolom (1..n): ");

                        bioskop.pesanTiket(nama, sid, baris, kolom);
                        break;
                    }
                    case 4:
                        bioskop.lihatAntrian();
                        break;
                    case 5:
                        break;
                    default:
                        cout << "Pilihan tidak valid.\n";
                }

            } while (pilihanUser != 5);
        } else if (pilihanMain == 3) {
            cout << "Terima kasih. Keluar program.\n";
            break;
        } else {
            cout << "Pilihan tidak valid.\n";
        }

    } while (true);

    return 0;
}
