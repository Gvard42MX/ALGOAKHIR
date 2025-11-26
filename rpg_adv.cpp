// rpg_adv.cpp
// RPG Petualangan (Console) - fitur: inventory, weapon, skills, map, boss, NPC dialog, save/load
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <limits>

using namespace std;

/* ---------- Struktur & kelas dasar ---------- */

struct Weapon {
    string name;
    int atkBonus;
    Weapon(string n = "Tangan Kosong", int a = 0) : name(n), atkBonus(a) {}
};

struct Item {
    string name;
    string type; // "potion", "weapon", "key", dll
    int value;   // misal: heal amount, atk untuk weapon stored separately
    Item(string n = "", string t = "", int v = 0) : name(n), type(t), value(v) {}
};

struct Skill {
    string name;
    int power;
    int cooldown;      // cooldown max
    int cdRemaining;   // turn remaining
    Skill(string n = "", int p = 0, int cd = 0) : name(n), power(p), cooldown(cd), cdRemaining(0) {}
};

struct Player {
    string nama;
    int hp;
    int maxHp;
    int baseAtk;
    int level;
    int exp;
    int x, y; // posisi di map
    Weapon equip;
    vector<Item> inventory;
    vector<Skill> skills;

    Player() {
        nama = "Hero";
        hp = maxHp = 120;
        baseAtk = 12;
        level = 1;
        exp = 0;
        x = y = 0;
        equip = Weapon();
    }

    int getAttack() const { return baseAtk + equip.atkBonus; }

    void addItem(const Item &it) {
        inventory.push_back(it);
        cout << ">> Mendapatkan item: " << it.name << " (" << it.type << ")\n";
    }

    bool useItem(const string &itemName) {
        for (size_t i=0;i<inventory.size();++i) {
            if (inventory[i].name == itemName) {
                if (inventory[i].type == "potion") {
                    hp += inventory[i].value;
                    if (hp > maxHp) hp = maxHp;
                    cout << ">> Menggunakan " << inventory[i].name << ", HP sekarang: " << hp << "/" << maxHp << "\n";
                    inventory.erase(inventory.begin()+i);
                    return true;
                } else if (inventory[i].type == "weapon") {
                    equip = Weapon(inventory[i].name, inventory[i].value);
                    cout << ">> Mengequip " << equip.name << " (ATK +" << equip.atkBonus << ")\n";
                    inventory.erase(inventory.begin()+i);
                    return true;
                } else if (inventory[i].type == "key") {
                    cout << ">> Key digunakan: " << inventory[i].name << "\n";
                    inventory.erase(inventory.begin()+i);
                    return true;
                } else {
                    cout << ">> Item tidak dapat digunakan sekarang.\n";
                    return false;
                }
            }
        }
        cout << ">> Item tidak ditemukan di inventory.\n";
        return false;
    }

    void showInventory() const {
        cout << "=== Inventory ===\n";
        if (inventory.empty()) cout << "Kosong\n";
        else {
            for (size_t i=0;i<inventory.size();++i) {
                cout << i+1 << ". " << inventory[i].name << " (" << inventory[i].type;
                if (inventory[i].type=="potion") cout << ", heal " << inventory[i].value;
                if (inventory[i].type=="weapon") cout << ", atk " << inventory[i].value;
                cout << ")\n";
            }
        }
        cout << "Equip: " << equip.name << " (ATK +" << equip.atkBonus << ")\n";
    }

    void showSkills() const {
        cout << "=== Skills ===\n";
        for (size_t i=0;i<skills.size();++i) {
            cout << i+1 << ". " << skills[i].name << " (power " << skills[i].power
                 << ", cooldown " << skills[i].cooldown << ", cdRem " << skills[i].cdRemaining << ")\n";
        }
    }

    void tickCooldowns() {
        for (auto &s : skills) {
            if (s.cdRemaining > 0) s.cdRemaining--;
        }
    }
};

struct Enemy {
    string name;
    int hp;
    int atk;
    int expReward;
    Enemy(string n="", int h=0, int a=0, int exp=0) : name(n), hp(h), atk(a), expReward(exp) {}
};

/* ---------- Map & NPC ---------- */

const int MAP_W = 5;
const int MAP_H = 5;

struct NPC {
    string name;
    int x,y;
    vector<string> dialog;
    NPC(string n="", int xx=0, int yy=0, vector<string> d = {}) : name(n), x(xx), y(yy), dialog(d) {}
    void talk() const {
        cout << name << ": \"" << dialog[rand() % dialog.size()] << "\"\n";
    }
};

struct GameMap {
    char grid[MAP_H][MAP_W];
    GameMap() {
        for (int r=0;r<MAP_H;r++){
            for (int c=0;c<MAP_W;c++){
                grid[r][c] = '.';
            }
        }
    }
    void show(const Player &p, const vector<NPC> &npcs, bool bossAlive) {
        cout << "\n=== Map (Kamu = @) ===\n";
        for (int r=0;r<MAP_H;r++) {
            for (int c=0;c<MAP_W;c++) {
                if (p.y==r && p.x==c) { cout << "@ "; continue; }
                bool printed=false;
                for (auto &n : npcs) if (n.x==c && n.y==r) { cout << "N "; printed=true; break; }
                if (printed) continue;
                if (r==MAP_H-1 && c==MAP_W-1) {
                    if (bossAlive) cout << "B "; else cout << "b ";
                } else cout << ". ";
            }
            cout << "\n";
        }
        cout << "Posisi: (" << p.x << "," << p.y << ")\n";
    }
};


/* ---------- Utility & flow ---------- */

void pauseConsole() {
    cout << "Tekan enter untuk lanjut...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

Enemy generateRandomEnemy(int playerLevel) {
    int r = rand()%3;
    if (r==0) return Enemy("Goblin", 30 + playerLevel*5, 8 + playerLevel*2, 30 + playerLevel*5);
    if (r==1) return Enemy("Serigala", 35 + playerLevel*6, 10 + playerLevel*2, 35 + playerLevel*6);
    return Enemy("Bandit", 40 + playerLevel*7, 12 + playerLevel*3, 40 + playerLevel*7);
}

void levelUpIfNeeded(Player &p) {
    while (p.exp >= 100) {
        p.exp -= 100;
        p.level++;
        p.maxHp += 30;
        p.baseAtk += 4;
        p.hp = p.maxHp;
        cout << "\n>>> LEVEL UP! Sekarang level " << p.level << ". HP dan ATK meningkat.\n";
    }
}

bool combat(Player &p, Enemy e) {
    cout << "\n--- Pertarungan dimulai: " << e.name << " ---\n";
    while (p.hp > 0 && e.hp > 0) {
        cout << "\nHP Kamu: " << p.hp << "/" << p.maxHp << "   |   HP " << e.name << ": " << e.hp << "\n";
        cout << "1) Serang  2) Gunakan Skill  3) Gunakan Item  4) Melarikan diri\nPilih: ";
        int cmd; cin >> cmd;

        if (cmd==1) {
            int dmg = p.getAttack() + (rand()%6 - 2); // sedikit variasi
            if (dmg < 0) dmg = 0;
            e.hp -= dmg;
            cout << "Kamu menyerang " << e.name << " dan memberikan " << dmg << " kerusakan.\n";
        } else if (cmd==2) {
            p.showSkills();
            cout << "Pilih skill (nomor): ";
            int si; cin >> si;
            if (si < 1 || si > (int)p.skills.size()) {
                cout << "Skill tidak valid.\n";
            } else {
                Skill &s = p.skills[si-1];
                if (s.cdRemaining > 0) {
                    cout << "Skill masih cooldown (" << s.cdRemaining << " turn tersisa).\n";
                } else {
                    int dmg = s.power + (p.getAttack()/2);
                    e.hp -= dmg;
                    s.cdRemaining = s.cooldown;
                    cout << "Menggunakan " << s.name << " dan memberikan " << dmg << " kerusakan.\n";
                }
            }
        } else if (cmd==3) {
            p.showInventory();
            cout << "Ketik nama item untuk digunakan (tepat penulisan), atau 'batal': ";
            string in;
            cin >> ws;
            getline(cin, in);
            if (in != "batal") p.useItem(in);
        } else if (cmd==4) {
            int chance = rand()%100;
            if (chance < 50) {
                cout << "Berhasil melarikan diri!\n";
                return false; // tidak mendapatkan exp
            } else {
                cout << "Gagal melarikan diri!\n";
            }
        } else {
            cout << "Pilihan tidak valid.\n";
        }

        if (e.hp > 0) {
            int edmg = e.atk + (rand()%5 - 2);
            if (edmg < 0) edmg = 0;
            p.hp -= edmg;
            cout << e.name << " menyerang dan memberikan " << edmg << " kerusakan!\n";
        }
        p.tickCooldowns();
    }

    if (p.hp <= 0) {
        cout << "\n>>> Kamu dikalahkan oleh " << e.name << ". Game Over.\n";
        return true; // player dead
    } else {
        cout << "\n>>> Musuh dikalahkan! Kamu mendapatkan " << e.expReward << " EXP.\n";
        p.exp += e.expReward;
        levelUpIfNeeded(p);
        // Drop chance
        int drop = rand()%100;
        if (drop < 50) {
            p.addItem(Item("Potion kecil", "potion", 30));
        } else if (drop < 75) {
            p.addItem(Item("Pedang kayu", "weapon", 6)); // langsung equip possible
        } else {
            p.addItem(Item("Kunci kuno", "key", 0));
        }
        return false;
    }
}

/* ---------- Save & Load ---------- */

void saveGame(const Player &p, const GameMap &map, bool bossAlive) {
    ofstream out("savegame.txt");
    if (!out) { cout << "Gagal membuat file save.\n"; return; }
    out << p.nama << '\n';
    out << p.hp << ' ' << p.maxHp << ' ' << p.baseAtk << ' ' << p.level << ' ' << p.exp << '\n';
    out << p.x << ' ' << p.y << '\n';
    out << p.equip.name << '|' << p.equip.atkBonus << '\n';
    // inventory
    out << p.inventory.size() << '\n';
    for (auto &it : p.inventory) out << it.name << '|' << it.type << '|' << it.value << '\n';
    // skills
    out << p.skills.size() << '\n';
    for (auto &s: p.skills) out << s.name << '|' << s.power << '|' << s.cooldown << '|' << s.cdRemaining << '\n';
    out << bossAlive << '\n';
    out.close();
    cout << ">> Game disimpan ke savegame.txt\n";
}

bool loadGame(Player &p, GameMap &map, bool &bossAlive, vector<NPC> &npcs) {
    ifstream in("savegame.txt");
    if (!in) { cout << "Tidak ada savegame ditemukan.\n"; return false; }
    string line;
    getline(in, p.nama);
    in >> p.hp >> p.maxHp >> p.baseAtk >> p.level >> p.exp;
    in >> p.x >> p.y;
    string tmp;
    getline(in, tmp); // consume endline
    getline(in, tmp);
    {
        stringstream ss(tmp);
        string wname; int watk;
        if (getline(ss, wname, '|')) {
            ss >> watk;
            p.equip = Weapon(wname, watk);
        }
    }
    size_t invSize; in >> invSize;
    getline(in, tmp); // newline
    p.inventory.clear();
    for (size_t i=0;i<invSize;i++) {
        getline(in, tmp);
        stringstream ss(tmp);
        string n,t; int v;
        getline(ss, n, '|');
        getline(ss, t, '|');
        ss >> v;
        p.inventory.push_back(Item(n,t,v));
    }
    size_t skillCount; in >> skillCount;
    getline(in, tmp);
    p.skills.clear();
    for (size_t i=0;i<skillCount;i++) {
        getline(in, tmp);
        stringstream ss(tmp);
        string name; int power, cd, cdRem;
        getline(ss, name, '|');
        ss >> power; ss.ignore(1); ss >> cd; ss.ignore(1); ss >> cdRem;
        p.skills.push_back(Skill(name, power, cd));
        p.skills.back().cdRemaining = cdRem;
    }
    in >> bossAlive;
    in.close();

    // reload NPCs (we keep same npc set, not saved)
    cout << ">> Savegame dimuat. Selamat datang kembali, " << p.nama << "!\n";
    return true;
}

/* ---------- Game Loop ---------- */

int main() {
    srand((unsigned)time(NULL));
    Player player;
    GameMap gameMap;

    // Inisialisasi skill dasar
    player.skills.push_back(Skill("Smash", 25, 3));
    player.skills.push_back(Skill("Heal Minor", 0, 4)); // kita implementasikan heal as skill special
    // starting items
    player.addItem(Item("Potion kecil", "potion", 30));
    player.addItem(Item("Pedang kayu", "weapon", 6));

    // NPC fixed
    vector<NPC> npcs;
    npcs.push_back(NPC("Orang desa", 1,1, {"Hati-hati di jalan.", "Dengar-dengar ada goblin di hutan."}));
    npcs.push_back(NPC("Pedagang", 2,3, {"Saya jual potion, tapi kamu tidak punya koin :)", "Ada kunci di peta kuno!"}));

    bool bossAlive = true;

    cout << "=== RPG PETUALANGAN LANJUTAN ===\n";
    cout << "Masukkan nama karakter: ";
    getline(cin, player.nama);

    bool running = true;
    while (running) {
        cout << "\n=== MENU UTAMA ===\n";
        cout << "1) Tampilkan status\n2) Tampilkan map\n3) Gerak (N/S/E/W)\n4) Cari musuh (encounter)\n5) NPC / Interaksi\n6) Inventory / Gunakan item\n7) Save\n8) Load\n9) Keluar\nPilih: ";
        int pilih; cin >> pilih;

        if (pilih == 1) {
            cout << "\nNama: " << player.nama << "  Level: " << player.level << "  EXP: " << player.exp << "/100\n";
            cout << "HP: " << player.hp << "/" << player.maxHp << "  ATK: " << player.getAttack() << "\n";
            player.showSkills();
            player.showInventory();
        } else if (pilih == 2) {
            gameMap.show(player, npcs, bossAlive);
        } else if (pilih == 3) {
            cout << "Masukkan arah (n/s/e/w): ";
            char d; cin >> d;
            int nx = player.x, ny = player.y;
            if (d=='n') ny--;
            else if (d=='s') ny++;
            else if (d=='e') nx++;
            else if (d=='w') nx--;
            else { cout << "Arah tidak valid.\n"; continue; }

            if (nx < 0 || nx >= MAP_W || ny < 0 || ny >= MAP_H) {
                cout << "Tidak bisa bergerak di luar peta.\n";
                continue;
            }
            player.x = nx; player.y = ny;
            cout << "Kamu bergerak ke (" << player.x << "," << player.y << ")\n";

            // Check tile special: boss tile
            if (player.x==MAP_W-1 && player.y==MAP_H-1 && bossAlive) {
                cout << ">>> Kamu menemui pintu gua besar: Boss menunggu!\n";
                cout << "Masuk? (y/n): ";
                char c; cin >> c;
                if (c=='y') {
                    Enemy boss("Naga Kuno", 220 + player.level*30, 25 + player.level*6, 200);
                    bool dead = combat(player, boss);
                    if (dead) { cout << "Kembalilah bila sudah lebih kuat.\n"; running=false; break; }
                    else {
                        cout << "KAMU MENGALAHKAN BOSS! Selamat!\n";
                        bossAlive = false;
                        player.addItem(Item("Mata Naga", "key", 0));
                    }
                }
            } else {
                // random encounter chance on movement
                int enc = rand()%100;
                if (enc < 35) {
                    Enemy e = generateRandomEnemy(player.level);
                    bool dead = combat(player, e);
                    if (dead) { running=false; break; }
                }
            }
        } else if (pilih == 4) {
            // force encounter
            Enemy e = generateRandomEnemy(player.level);
            bool dead = combat(player, e);
            if (dead) { running=false; break; }
        } else if (pilih == 5) {
            bool inNpc=false;
            for (auto &n : npcs) {
                if (n.x==player.x && n.y==player.y) {
                    cout << "\nKamu bertemu NPC: " << n.name << "\n";
                    n.talk();
                    // simple interaction options
                    cout << "1) Tanyakan kabar  2) Beli potion (50 gold - tidak ada sistem gold sekarang)  3) Batal\nPilih: ";
                    int opt; cin >> opt;
                    if (opt==1) n.talk();
                    else if (opt==2) {
                        cout << "NPC tidak terima uang (belum diimplementasikan).\n";
                    }
                    inNpc=true;
                }
            }
            if (!inNpc) cout << "Tidak ada NPC di lokasi ini.\n";
        } else if (pilih == 6) {
            player.showInventory();
            cout << "1) Gunakan item  2) Buang item  3) Kembali\nPilih: ";
            int op; cin >> op;
            if (op==1) {
                cout << "Masukkan nama item untuk digunakan: ";
                string in; cin >> ws; getline(cin, in);
                bool used = player.useItem(in);
                if (!used) cout << "Gagal menggunakan item.\n";
            } else if (op==2) {
                cout << "Masukkan nama item untuk dibuang: ";
                string in; cin >> ws; getline(cin, in);
                bool found=false;
                for (size_t i=0;i<player.inventory.size();++i) {
                    if (player.inventory[i].name==in) {
                        player.inventory.erase(player.inventory.begin()+i);
                        cout << "Item dibuang.\n";
                        found=true; break;
                    }
                }
                if (!found) cout << "Item tidak ditemukan.\n";
            }
        } else if (pilih == 7) {
            saveGame(player, gameMap, bossAlive);
        } else if (pilih == 8) {
            loadGame(player, gameMap, bossAlive, npcs);
        } else if (pilih == 9) {
            cout << "Keluar dari game? (y/n): ";
            char c; cin >> c;
            if (c=='y') { cout << "Sampai jumpa!\n"; break; }
        } else {
            cout << "Pilihan tidak valid.\n";
        }

        // small passive regen? (opsional)
        if (player.hp < player.maxHp && rand()%100 < 10) player.hp += 1;
    }

    return 0;
}
