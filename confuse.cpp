#include <iostream>
#include <queue>
#include <string>
#include <vector>
using namespace std;

struct Film {
    string judul;
    int harga;
    int studio;
};

struct Tiket {
    int noAntrian;
    string nama;
    Film filmDipilih;
    int kursi;
};

int main() {
    queue<Tiket> antrian;

    // ===============================
    // DAFTAR FILM (BISA DITAMBAH)
    // ===============================
    vector<Film> daftarFilm = {
        {"Avengers: Endgame", 50000, 1},
        {"Spiderman: No Way Home", 45000, 2},
        {"Toy Story 4", 35000, 3},
        {"Fast & Furious 9", 48000, 1},
        {"Joker", 42000, 2},
        {"Frozen 2", 36000, 3}
    };

    int pilihan, nomorUrut = 1, nomorKursi = 1;

    do {
        cout << "\n===== SISTEM ANTRIAN TIKET BIOSKOP =====\n";
        cout << "1. Tambah Antrian Pembeli\n";
        cout << "2. Layani Antrian (Panggil)\n";
        cout << "3. Lihat Semua Antrian\n";
        cout << "4. Keluar\n";
        cout << "Pilih menu: ";
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {

        case 1: {
            Tiket t;
            t.noAntrian = nomorUrut++;

            cout << "Masukkan nama pembeli: ";
            getline(cin, t.nama);

            // ===============================
            //   TAMPILKAN DAFTAR FILM
            // ===============================
            cout << "\n--- Pilih Film ---\n";
            for (int i = 0; i < daftarFilm.size(); i++) {
                cout << i + 1 << ". " << daftarFilm[i].judul
                     << " (Rp " << daftarFilm[i].harga
                     << ") - Studio " << daftarFilm[i].studio << endl;
            }

            cout << "Pilihan film: ";
            int pilih;
            cin >> pilih;

            if (pilih < 1 || pilih > daftarFilm.size()) {
                cout << "Pilihan tidak valid!\n";
                continue;
            }

            // pilih film berdasarkan index
            t.filmDipilih = daftarFilm[pilih - 1];
            t.kursi = nomorKursi++;

            antrian.push(t);

            cout << "\nData pembeli berhasil ditambahkan!\n";
            cout << "No Antrian : " << t.noAntrian << endl;
            cout << "Nama       : " << t.nama << endl;
            cout << "Film       : " << t.filmDipilih.judul << endl;
            cout << "Studio     : " << t.filmDipilih.studio << endl;
            cout << "Kursi      : " << t.kursi << endl;
            cout << "Harga      : Rp " << t.filmDipilih.harga << endl;

            break;
        }

        case 2:
            if (antrian.empty()) {
                cout << "Antrian kosong!\n";
            } else {
                Tiket t = antrian.front();
                antrian.pop();

                cout << "\nMemanggil Pembeli:\n";
                cout << "No Antrian : " << t.noAntrian << endl;
                cout << "Nama       : " << t.nama << endl;
                cout << "Film       : " << t.filmDipilih.judul << endl;
                cout << "Studio     : " << t.filmDipilih.studio << endl;
                cout << "Kursi      : " << t.kursi << endl;
                cout << "Total Bayar: Rp " << t.filmDipilih.harga << endl;
            }
            break;

        case 3:
            if (antrian.empty()) {
                cout << "Tidak ada antrian.\n";
            } else {
                cout << "\n===== DAFTAR ANTRIAN =====\n";
                queue<Tiket> temp = antrian;
                while (!temp.empty()) {
                    Tiket t = temp.front();
                    temp.pop();

                    cout << "No Antrian : " << t.noAntrian << endl;
                    cout << "Nama       : " << t.nama << endl;
                    cout << "Film       : " << t.filmDipilih.judul << endl;
                    cout << "Studio     : " << t.filmDipilih.studio << endl;
                    cout << "Kursi      : " << t.kursi << endl;
                    cout << "Harga      : Rp " << t.filmDipilih.harga << endl;
                    cout << "--------------------------------------\n";
                }
            }
            break;

        case 4:
            cout << "Keluar dari program...\n";
            break;

        default:
            cout << "Pilihan tidak valid!\n";
        }

    } while (pilihan != 4);

    return 0;
}
