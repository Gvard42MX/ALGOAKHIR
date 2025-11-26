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

void tampilFilm(const vector<Film>& daftarFilm) {
    cout << "\n===== DAFTAR FILM =====\n";
    if (daftarFilm.empty()) {
        cout << "Belum ada film.\n";
        return;
    }

    for (int i = 0; i < daftarFilm.size(); i++) {
        cout << i + 1 << ". " << daftarFilm[i].judul
             << " (Rp " << daftarFilm[i].harga
             << ") - Studio " << daftarFilm[i].studio << endl;
    }
}

int main() {
    queue<Tiket> antrian;

    vector<Film> daftarFilm = {
        {"Avengers: Endgame", 50000, 1},
        {"Spiderman: No Way Home", 45000, 2},
        {"Toy Story 4", 35000, 3}
    };

    int nomorUrut = 1;
    int nomorKursi = 1;
    int menuUtama;

    do {
        cout << "\n===== MENU UTAMA =====\n";
        cout << "1. Login Admin\n";
        cout << "2. Masuk User\n";
        cout << "3. Keluar\n";
        cout << "Pilih: ";
        cin >> menuUtama;
        cin.ignore();

        // ==============================
        //          MENU ADMIN
        // ==============================
        if (menuUtama == 1) {
            string pass;
            cout << "Masukkan password admin: ";
            getline(cin, pass);

            if (pass != "admin123") {
                cout << "Password salah!\n";
                continue;
            }

            int menuAdmin;
            do {
                cout << "\n===== MENU ADMIN =====\n";
                cout << "1. Lihat Daftar Film\n";
                cout << "2. Tambah Film\n";
                cout << "3. Hapus Film\n";
                cout << "4. Kembali\n";
                cout << "Pilih: ";
                cin >> menuAdmin;
                cin.ignore();

                switch (menuAdmin) {
                case 1:
                    tampilFilm(daftarFilm);
                    break;

                case 2: {
                    Film f;
                    cout << "Judul film: ";
                    getline(cin, f.judul);
                    cout << "Harga tiket: ";
                    cin >> f.harga;
                    cout << "Studio: ";
                    cin >> f.studio;
                    cin.ignore();

                    daftarFilm.push_back(f);
                    cout << "Film berhasil ditambahkan!\n";
                    break;
                }

                case 3: {
                    tampilFilm(daftarFilm);
                    cout << "Pilih nomor film yang ingin dihapus: ";
                    int hapus;
                    cin >> hapus;

                    if (hapus < 1 || hapus > daftarFilm.size()) {
                        cout << "Pilihan tidak valid!\n";
                    } else {
                        daftarFilm.erase(daftarFilm.begin() + (hapus - 1));
                        cout << "Film dihapus!\n";
                    }
                    break;
                }

                case 4:
                    break;

                default:
                    cout << "Pilihan tidak valid!\n";
                }

            } while (menuAdmin != 4);
        }

        // ==============================
        //          MENU USER
        // ==============================
        else if (menuUtama == 2) {
            int menuUser;
            do {
                cout << "\n===== MENU USER =====\n";
                cout << "1. Lihat Film\n";
                cout << "2. Pesan Tiket (Masuk Antrian)\n";
                cout << "3. Dipanggil (Pop Antrian)\n";
                cout << "4. Lihat Semua Antrian\n";
                cout << "5. Kembali\n";
                cout << "Pilih: ";
                cin >> menuUser;
                cin.ignore();

                switch (menuUser) {

                case 1:
                    tampilFilm(daftarFilm);
                    break;

                case 2: {
                    if (daftarFilm.empty()) {
                        cout << "Tidak ada film untuk dipesan!\n";
                        break;
                    }

                    Tiket t;
                    t.noAntrian = nomorUrut++;

                    cout << "Masukkan nama: ";
                    getline(cin, t.nama);

                    tampilFilm(daftarFilm);
                    cout << "Pilih film: ";
                    int pilih;
                    cin >> pilih;

                    if (pilih < 1 || pilih > daftarFilm.size()) {
                        cout << "Pilihan tidak valid!\n";
                        break;
                    }

                    t.filmDipilih = daftarFilm[pilih - 1];
                    t.kursi = nomorKursi++;

                    antrian.push(t);

                    cout << "Tiket ditambahkan ke antrian!\n";
                    break;
                }

                case 3:
                    if (antrian.empty()) {
                        cout << "Antrian kosong!\n";
                    } else {
                        Tiket t = antrian.front();
                        antrian.pop();
                        cout << "\n===== DIPANGGIL =====\n";
                        cout << "Nama       : " << t.nama << endl;
                        cout << "Film       : " << t.filmDipilih.judul << endl;
                        cout << "Studio     : " << t.filmDipilih.studio << endl;
                        cout << "Kursi      : " << t.kursi << endl;
                        cout << "Harga      : Rp " << t.filmDipilih.harga << endl;
                    }
                    break;

                case 4:
                    if (antrian.empty()) {
                        cout << "Antrian kosong!\n";
                    } else {
                        cout << "\n===== SEMUA ANTRIAN =====\n";
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
                            cout << "--------------------------\n";
                        }
                    }
                    break;

                case 5:
                    break;

                default:
                    cout << "Pilihan tidak valid!\n";
                }

            } while (menuUser != 5);
        }

    } while (menuUtama != 3);

    cout << "Program selesai.\n";
    return 0;
}
