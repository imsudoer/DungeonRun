#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

static HANDLE hOut;
static int CW = 80, CH = 25;

void csizeupd()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut, &csbi))
    {
        CW = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        CH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        if (CW < 70)
            CW = 70;
        if (CH < 22)
            CH = 22;
    }
}

void setColor(int c) { SetConsoleTextAttribute(hOut, c); }
void clearScreen()
{
    system("cls");
    csizeupd();
}
void sleepMs(int ms) { Sleep(ms); }
int randInt(int a, int b) { return a + rand() % (b - a + 1); }
int mini(int a, int b) { return a < b ? a : b; }
int maxi(int a, int b) { return a > b ? a : b; }

void hideCur()
{
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize = 1;
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &ci);
}
void showCur()
{
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize = 1;
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &ci);
}

string repeat(char c, int n)
{
    if (n <= 0)
        return "";
    string s;
    s.reserve(n);
    for (int i = 0; i < n; i++)
        s += c;
    return s;
}

int vLen(const string &s)
{
    int len = 0;
    int i = 0;
    int n = (int)s.size();
    while (i < n)
    {
        unsigned char c = (unsigned char)s[i];
        if (c < 0x80)
        {
            len++;
            i++;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            len++;
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            len++;
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            len += 2;
            i += 4;
        }
        else
        {
            len++;
            i++;
        }
    }
    return len;
}

string truncV(const string &s, int maxLen)
{
    int len = 0;
    int i = 0;
    int n = (int)s.size();
    string out;
    while (i < n)
    {
        unsigned char c = (unsigned char)s[i];
        int charBytes = 1, charLen = 1;
        if (c < 0x80)
        {
            charBytes = 1;
            charLen = 1;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            charBytes = 2;
            charLen = 1;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            charBytes = 3;
            charLen = 1;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            charBytes = 4;
            charLen = 2;
        }
        if (len + charLen > maxLen)
            break;
        out.append(s, i, charBytes);
        i += charBytes;
        len += charLen;
    }
    return out;
}

string padCV(const string &s, int w) // center
{
    int len = vLen(s);
    if (len >= w)
        return truncV(s, w);
    int left = (w - len) / 2;
    int right = w - len - left;
    return repeat(' ', left) + s + repeat(' ', right);
}

string padRV(const string &s, int w) // right
{
    int len = vLen(s);
    if (len >= w)
        return truncV(s, w);
    return s + repeat(' ', w - len);
}

void printC(const string &s, int color = 7)
{
    int len = vLen(s);
    int pad = (CW - len) / 2;
    if (pad < 0)
        pad = 0;
    setColor(color);
    cout << repeat(' ', pad) << s << "\n";
    setColor(7);
}

void drawBrd(int width, int color, char corner = '+', char hor = '=')
{
    int pad = (CW - width) / 2;
    if (pad < 0)
        pad = 0;
    setColor(color);
    cout << repeat(' ', pad) << corner << repeat(hor, width - 2) << corner << "\n";
    setColor(7);
}

void drawCBoxL(const string &content, int width, int color)
{
    int pad = (CW - width) / 2;
    if (pad < 0)
        pad = 0;
    int inner = width - 2;
    string padded = padCV(content, inner);
    setColor(color);
    cout << repeat(' ', pad) << '|' << padded << '|' << "\n";
    setColor(7);
}

struct Player
{
    int hp = 100;
    int maxHp = 100;
    int atk = 15;
    int armor = 0;
    int gold = 30;
    int potions = 1;
    int room = 0;
    int kills = 0;
};

struct Monster
{
    string name;
    int hp;
    int maxHp;
    int atk;
    int reward;
    string art[5];
    int artLines;
};

Monster makeMonster(int level)
{
    Monster m;
    int t = randInt(0, 3);
    if (t == 0)
    {
        m.name = "Гоблин";
        m.hp = m.maxHp = 25 + level * 6;
        m.atk = 8 + level;
        m.reward = 10 + level * 3;
        m.art[0] = "  /^\\/^\\";
        m.art[1] = " | o o |";
        m.art[2] = "  \\_v_/";
        m.art[3] = "  /| |\\";
        m.artLines = 4;
    }
    else if (t == 1)
    {
        m.name = "Скелет";
        m.hp = m.maxHp = 35 + level * 7;
        m.atk = 10 + level;
        m.reward = 15 + level * 4;
        m.art[0] = "   .-.";
        m.art[1] = "  (o.o)";
        m.art[2] = "   |=|";
        m.art[3] = "  /| |\\";
        m.artLines = 4;
    }
    else if (t == 2)
    {
        m.name = "Орк";
        m.hp = m.maxHp = 50 + level * 8;
        m.atk = 12 + level;
        m.reward = 20 + level * 5;
        m.art[0] = "  ,---.";
        m.art[1] = "  (>.<)";
        m.art[2] = "  /|^|\\";
        m.art[3] = "   | |";
        m.artLines = 4;
    }
    else
    {
        m.name = "Тень";
        m.hp = m.maxHp = 40 + level * 9;
        m.atk = 14 + level;
        m.reward = 25 + level * 5;
        m.art[0] = "  .vvv.";
        m.art[1] = " ( x x )";
        m.art[2] = "  )===(";
        m.art[3] = "  ~~ ~~";
        m.artLines = 4;
    }
    return m;
}

void drawHpBar(const string &label, int hp, int maxHp, int width, int color)
{
    if (hp < 0)
        hp = 0;
    int filled = maxHp > 0 ? hp * width / maxHp : 0;
    string text;
    text.reserve(width + 32);
    string hpStr = to_string(hp) + "/" + to_string(maxHp);
    int totalLen = (int)label.size() + 2 + width + 2 + (int)hpStr.size();
    int pad = (CW - totalLen) / 2;
    if (pad < 0)
        pad = 0;
    cout << repeat(' ', pad);
    setColor(8);
    cout << label << " [";
    setColor(color);
    for (int i = 0; i < filled; i++)
        cout << '#';
    setColor(8);
    for (int i = filled; i < width; i++)
        cout << '-';
    cout << "] ";
    setColor(15);
    cout << hpStr << "\n";
    setColor(7);
}

void drawHUD(const Player &p, bool endless = false)
{
    int boxW = mini(CW - 4, 70);
    drawBrd(boxW, 8);
    int pad = (CW - boxW) / 2;
    if (pad < 0)
        pad = 0;
    int inner = boxW - 2;

    string parts[6];
    parts[0] = "HP " + to_string(p.hp) + "/" + to_string(p.maxHp);
    parts[1] = "АТК " + to_string(p.atk);
    parts[2] = "БРН " + to_string(p.armor);
    parts[3] = "ЗОЛ " + to_string(p.gold);
    parts[4] = "ХИЛ " + to_string(p.potions);
    parts[5] = "КМН " + to_string(p.room + 1);
    int colors[6] = {10, 11, 9, 14, 13, 12};

    int totalLen = 0;
    for (int i = 0; i < 6; i++)
        totalLen += vLen(parts[i]);
    int sepCount = 5;
    int spaceLeft = inner - totalLen - sepCount * 3;
    if (spaceLeft < 0)
        spaceLeft = 0;
    int leftPad = 1;
    int rightPad = spaceLeft - leftPad;
    if (rightPad < 0)
    {
        rightPad = 0;
        leftPad = 0;
    }

    setColor(8);
    cout << repeat(' ', pad) << '|';
    cout << repeat(' ', leftPad);
    for (int i = 0; i < 6; i++)
    {
        setColor(colors[i]);
        cout << parts[i];
        if (i < 5)
        {
            setColor(8);
            cout << " | ";
        }
    }
    setColor(8);
    cout << repeat(' ', rightPad);
    cout << '|' << "\n";
    setColor(7);
    drawBrd(boxW, 8);
}

void drawTitle(const string &title, int color)
{
    int boxW = mini(CW - 4, 70);
    drawBrd(boxW, color);
    drawCBoxL(title, boxW, color);
    drawBrd(boxW, color);
}

void drawArt(const string *lines, int n, int color)
{
    int maxLen = 0;
    for (int i = 0; i < n; i++)
    {
        int l = vLen(lines[i]);
        if (l > maxLen)
            maxLen = l;
    }
    int pad = (CW - maxLen) / 2;
    if (pad < 0)
        pad = 0;
    setColor(color);
    for (int i = 0; i < n; i++)
    {
        cout << repeat(' ', pad) << padRV(lines[i], maxLen) << "\n";
    }
    setColor(7);
}

void drawRoom(const string &type, int roomNum, bool endless)
{
    string header = "КОМНАТА " + to_string(roomNum) + (endless ? "  [БЕСКОНЕЧНЫЙ]" : "");
    drawTitle(header, 6);
    cout << "\n";
    string lines[6];
    int n = 0;
    int color = 7;
    if (type == "fight")
    {
        color = 12;
        lines[n++] = "    .--.";
        lines[n++] = "   |o_o |";
        lines[n++] = "   |:_/ |";
        lines[n++] = "  //   \\ \\";
        lines[n++] = " (|     | )";
        lines[n++] = "/'\\_   _/`\\";
    }
    else if (type == "trap")
    {
        color = 14;
        lines[n++] = " ^   ^   ^   ^   ^";
        lines[n++] = "/|\\ /|\\ /|\\ /|\\ /|\\";
        lines[n++] = "  ШИПЫ ИЗ ПОЛА!";
        lines[n++] = "\\|/ \\|/ \\|/ \\|/ \\|/";
        lines[n++] = " v   v   v   v   v";
    }
    else if (type == "chest")
    {
        color = 14;
        lines[n++] = "   .------------.";
        lines[n++] = "  /    .--.    /|";
        lines[n++] = " +----[==]----+ |";
        lines[n++] = " |   $   $    | +";
        lines[n++] = " +------------+/";
    }
    else if (type == "merchant")
    {
        color = 11;
        lines[n++] = "    (o_o)";
        lines[n++] = "   <)   (>";
        lines[n++] = "    /   \\";
        lines[n++] = " [товары] [зелья]";
    }
    else if (type == "boss")
    {
        color = 4;
        lines[n++] = "  /\\\\___//\\";
        lines[n++] = " ( O   O  )";
        lines[n++] = "  >  ^  <";
        lines[n++] = " /||___||\\";
        lines[n++] = "  ^^   ^^";
    }
    else
    {
        color = 8;
        lines[n++] = ". . . . . .";
        lines[n++] = ". пустой  .";
        lines[n++] = ".         .";
        lines[n++] = ". коридор .";
        lines[n++] = ". . . . . .";
    }
    drawArt(lines, n, color);
    cout << "\n";
    if (type == "fight")
        printC("<< К вам приближается враг >>", 12);
    else if (type == "trap")
        printC("<< Под ногами что-то щёлкнуло >>", 14);
    else if (type == "chest")
        printC("<< Сокровище >>", 14);
    else if (type == "merchant")
        printC("\"Добро пожаловать, путник!\"", 11);
    else if (type == "boss")
        printC("Воздух становится холоднее...", 12);
    cout << "\n";
}

char getKeyTimeout(int ms)
{
    auto start = steady_clock::now();
    while (true)
    {
        if (_kbhit())
            return _getch();
        auto now = steady_clock::now();
        if (duration_cast<milliseconds>(now - start).count() >= ms)
            return 0;
        Sleep(5);
    }
}

string comboRand(int len)
{
    static const char chars[] = "WASDQERFZX";
    string s;
    for (int i = 0; i < len; i++)
        s += chars[rand() % 10];
    return s;
}

void printCL(const string &combo, int color) // comboline
{
    string disp;
    for (size_t i = 0; i < combo.size(); i++)
    {
        disp += "[";
        disp += combo[i];
        disp += "]";
        if (i + 1 < combo.size())
            disp += " ";
    }
    printC(disp, color);
}

double doQTE(const string &combo, int totalMs)
{
    cout << "\n";
    printC("--- БЫСТРОЕ НАЖАТИЕ ---", 14);
    printCL(combo, 11);
    printC("Время: " + to_string(totalMs) + " мс", 8);
    cout << "\n";

    int totalDispLen = (int)combo.size() * 4 - 1;
    int pad = (CW - totalDispLen) / 2;
    if (pad < 0)
        pad = 0;
    cout << repeat(' ', pad);

    int hits = 0;
    auto start = steady_clock::now();
    for (size_t i = 0; i < combo.size(); i++)
    {
        int elapsed = (int)duration_cast<milliseconds>(steady_clock::now() - start).count();
        int remaining = totalMs - elapsed;
        if (remaining <= 0)
            break;
        char k = getKeyTimeout(remaining);
        if (k == 0)
            break;
        char up = (k >= 'a' && k <= 'z') ? k - 32 : k;
        if (up == combo[i])
        {
            setColor(10);
            cout << "[" << up << "]";
            setColor(7);
            hits++;
        }
        else
        {
            setColor(12);
            cout << "[X]";
            setColor(7);
        }
        if (i + 1 < combo.size())
            cout << " ";
    }
    cout << "\n\n";
    return (double)hits / combo.size();
}

double doBlockQTE()
{
    string combo = comboRand(3);
    cout << "\n";
    printC(">>> БЛОК! <<<", 9);
    printCL(combo, 11);
    cout << "\n";
    int totalDispLen = (int)combo.size() * 4 - 1;
    int pad = (CW - totalDispLen) / 2;
    if (pad < 0)
        pad = 0;
    cout << repeat(' ', pad);
    int hits = 0;
    auto start = steady_clock::now();
    int totalMs = 1300;
    for (size_t i = 0; i < combo.size(); i++)
    {
        int elapsed = (int)duration_cast<milliseconds>(steady_clock::now() - start).count();
        int remaining = totalMs - elapsed;
        if (remaining <= 0)
            break;
        char k = getKeyTimeout(remaining);
        if (k == 0)
            break;
        char up = (k >= 'a' && k <= 'z') ? k - 32 : k;
        if (up == combo[i])
        {
            setColor(10);
            cout << "[" << up << "]";
            setColor(7);
            hits++;
        }
        else
        {
            setColor(12);
            cout << "[X]";
            setColor(7);
        }
        if (i + 1 < combo.size())
            cout << " ";
    }
    cout << "\n\n";
    return (double)hits / combo.size();
}

int calcArmor(int dmg, int armor)
{
    int reduced = dmg - armor / 3;
    if (reduced < 1)
        reduced = 1;
    return reduced;
}

void waitDelay(int ms) { sleepMs(ms); }

bool fight(Player &p, Monster m)
{
    bool willBlock = false;
    while (m.hp > 0 && p.hp > 0)
    {
        clearScreen();
        drawTitle("Б И Т В А", 12);
        cout << "\n";
        drawArt(m.art, m.artLines, 12);
        printC(m.name, 15);
        cout << "\n";
        int barW = mini(CW - 24, 50);
        drawHpBar("ВРАГ", m.hp, m.maxHp, barW, 12);
        cout << "\n";
        drawHUD(p);

        cout << "\n";
        printC("--- ВАШ ХОД ---", 14);
        printC("[1] Удар в голову   (x2.0, сложно)", 7);
        printC("[2] Удар в корпус   (x1.0, средне)", 7);
        printC("[3] Удар по ногам   (x0.7, легко)", 7);
        printC("[4] Блокировать следующую атаку", 9);
        printC("[5] Использовать зелье (+40 HP)", 13);
        printC("[6] Попытка сбежать", 8);
        cout << "\n";

        char c = _getch();
        if (c == '5')
        {
            if (p.potions > 0)
            {
                p.potions--;
                p.hp = mini(p.maxHp, p.hp + 40);
                printC("+40 HP", 10);
                waitDelay(700);
            }
            else
            {
                printC("Нет зелий!", 12);
                waitDelay(700);
            }
            continue;
        }
        if (c == '6')
        {
            if (rand() % 2 == 0)
            {
                printC("Вы сбежали...", 10);
                waitDelay(800);
                return false;
            }
            printC("Сбежать не удалось", 12);
            waitDelay(700);
            int dmg = calcArmor(m.atk, p.armor);
            p.hp -= dmg;
            continue;
        }
        if (c == '4')
        {
            willBlock = true;
            printC("Вы готовите защиту...", 9);
            waitDelay(700);
        }
        else
        {
            int comboLen, timeMs;
            double mult;
            if (c == '1')
            {
                comboLen = 6;
                timeMs = 2200;
                mult = 2.0;
            }
            else if (c == '2')
            {
                comboLen = 4;
                timeMs = 2000;
                mult = 1.0;
            }
            else if (c == '3')
            {
                comboLen = 3;
                timeMs = 1800;
                mult = 0.7;
            }
            else
                continue;
            string combo = comboRand(comboLen);
            double acc = doQTE(combo, timeMs);
            int dmg = (int)(p.atk * mult * acc);
            if (dmg < 1 && acc > 0)
                dmg = 1;
            m.hp -= dmg;
            printC("Удар нанёс " + to_string(dmg) + " урона  (точность " + to_string((int)(acc * 100)) + "%)", 10);
            waitDelay(800);
        }

        if (m.hp <= 0)
            break;

        cout << "\n";
        printC(m.name + " атакует!", 12);
        waitDelay(500);

        int incoming = m.atk;
        if (willBlock)
        {
            double br = doBlockQTE();
            int reduction = (int)(incoming * br * 0.85);
            incoming -= reduction;
            if (br >= 1.0)
                printC("ИДЕАЛЬНЫЙ БЛОК!", 11);
            else if (br > 0)
                printC("Частичный блок (-" + to_string(reduction) + ")", 9);
            else
                printC("Блок не удался", 12);
            willBlock = false;
        }
        int dmg = calcArmor(incoming, p.armor);
        p.hp -= dmg;
        printC("Вы получаете " + to_string(dmg) + " урона", 12);
        waitDelay(1000);
    }

    if (p.hp <= 0)
        return false;

    int g = m.reward + randInt(0, 5);
    p.gold += g;
    p.kills++;
    cout << "\n";
    printC("*** ПОБЕДА ***", 10);
    printC("Вы одолели врага: " + m.name + "  +" + to_string(g) + " золота", 14);
    if (rand() % 100 < 25)
    {
        p.potions++;
        printC("Выпало зелье! (+1)", 13);
    }
    waitDelay(2200);
    return true;
}

bool dodgePhase(Player &p, int duration, int density) // idea = undertale
{
    int boxW = mini(CW - 8, 40);
    if (boxW < 20)
        boxW = 20;
    int boxH = 9;
    int px = boxW / 2;
    int py = boxH - 2;
    const int MAXA = 12;
    int ax[MAXA], ay[MAXA];
    bool aActive[MAXA];
    for (int i = 0; i < MAXA; i++)
        aActive[i] = false;

    auto start = steady_clock::now();
    int spawnTimer = 0;

    while (true)
    {
        int elapsed = (int)duration_cast<milliseconds>(steady_clock::now() - start).count();
        if (elapsed >= duration)
            break;
        if (p.hp <= 0)
            return false;

        spawnTimer++;
        if (spawnTimer >= density)
        {
            spawnTimer = 0;
            for (int i = 0; i < MAXA; i++)
            {
                if (!aActive[i])
                {
                    aActive[i] = true;
                    ax[i] = randInt(1, boxW - 2);
                    ay[i] = 0;
                    break;
                }
            }
        }
        for (int i = 0; i < MAXA; i++)
        {
            if (aActive[i])
            {
                ay[i]++;
                if (ay[i] >= boxH)
                    aActive[i] = false;
                else if (ay[i] == py && ax[i] == px)
                {
                    aActive[i] = false;
                    int dmg = calcArmor(6, p.armor);
                    p.hp -= dmg;
                }
            }
        }
        while (_kbhit())
        {
            char k = _getch();
            char up = (k >= 'a' && k <= 'z') ? k - 32 : k;
            if (up == 'A')
            {
                if (px > 1)
                    px--;
            }
            else if (up == 'D')
            {
                if (px < boxW - 2)
                    px++;
            }
            else if (up == 'W')
            {
                if (py > 1)
                    py--;
            }
            else if (up == 'S')
            {
                if (py < boxH - 2)
                    py++;
            }
        }

        clearScreen();
        drawTitle("!! ФАЗА УВОРОТА | WASD !!", 4);
        cout << "\n";
        int barW = mini(CW - 24, 50);
        drawHpBar("ВЫ  ", p.hp, p.maxHp, barW, 10);
        printC("Осталось: " + to_string((duration - elapsed) / 1000) + " с", 8);
        cout << "\n";

        int boxPad = (CW - boxW) / 2;
        if (boxPad < 0)
            boxPad = 0;
        for (int y = 0; y < boxH; y++)
        {
            cout << repeat(' ', boxPad);
            for (int x = 0; x < boxW; x++)
            {
                if (y == 0 || y == boxH - 1)
                {
                    setColor(15);
                    cout << '-';
                }
                else if (x == 0 || x == boxW - 1)
                {
                    setColor(15);
                    cout << '|';
                }
                else
                {
                    bool drawn = false;
                    if (x == px && y == py)
                    {
                        setColor(12);
                        cout << '@';
                        drawn = true;
                    }
                    else
                    {
                        for (int i = 0; i < MAXA; i++)
                        {
                            if (aActive[i] && ax[i] == x && ay[i] == y)
                            {
                                setColor(14);
                                cout << '*';
                                drawn = true;
                                break;
                            }
                        }
                    }
                    if (!drawn)
                        cout << ' ';
                }
            }
            setColor(7);
            cout << "\n";
        }
        sleepMs(110);
    }
    return p.hp > 0;
}

bool bossFight(Player &p, int level)
{
    int bossMaxHp = 200 + level * 60;
    int bossHp = bossMaxHp;
    string bossName = level == 0 ? "Древний Владыка Подземелья" : "Вечный Лорд Хаоса #" + to_string(level + 1);

    clearScreen();
    drawTitle("*** ВСТРЕЧА С БОССОМ ***", 4);
    cout << "\n";
    printC(bossName + " пробуждается...", 12);
    waitDelay(2000);

    int phase = 0;
    while (bossHp > 0 && p.hp > 0)
    {
        clearScreen();
        drawTitle(bossName, 4);
        cout << "\n";
        string art[5] = {
            "  /\\\\___//\\",
            " ( O   O  )",
            "  >  ^  <",
            " /||___||\\",
            "  ^^   ^^"};
        drawArt(art, 5, 11);
        cout << "\n";
        int barW = mini(CW - 24, 60);
        drawHpBar("БОСС", bossHp, bossMaxHp, barW, 12);
        cout << "\n";
        drawHUD(p);

        cout << "\n";
        printC("--- ДЕЙСТВИЕ ---", 14);
        printC("[1] Тяжёлая атака   (8 клавиш, x1.5)", 7);
        printC("[2] Точечный удар   (5 клавиш, x2.5)", 7);
        printC("[3] Использовать зелье (+40 HP)", 13);
        printC("[4] Защита (урон уворота вдвое меньше)", 9);
        cout << "\n";

        char c = _getch();
        bool defended = false;
        if (c == '1')
        {
            string cb = comboRand(8);
            double acc = doQTE(cb, 3000);
            int dmg = (int)(p.atk * 1.5 * acc);
            bossHp -= dmg;
            printC("Босс получает " + to_string(dmg) + " урона!", 10);
            waitDelay(900);
        }
        else if (c == '2')
        {
            string cb = comboRand(5);
            double acc = doQTE(cb, 1500);
            int dmg = (int)(p.atk * 2.5 * acc);
            bossHp -= dmg;
            printC("Критический удар! " + to_string(dmg) + " урона!", 10);
            waitDelay(900);
        }
        else if (c == '3')
        {
            if (p.potions > 0)
            {
                p.potions--;
                p.hp = mini(p.maxHp, p.hp + 40);
                printC("+40 HP", 10);
                waitDelay(700);
            }
            else
            {
                printC("Нет зелий!", 12);
                waitDelay(700);
                continue;
            }
        }
        else if (c == '4')
        {
            defended = true;
            printC("Вы готовитесь к защите...", 9);
            waitDelay(700);
        }
        else
            continue;

        if (bossHp <= 0)
            break;

        cout << "\n";
        printC(">>> БОСС ГОТОВИТ АТАКУ <<<", 12);
        waitDelay(1300);

        phase++;
        int dur = 5000 + phase * 600;
        int dens = 8 - phase - level;
        if (dens < 2)
            dens = 2;
        int hpBefore = p.hp;
        dodgePhase(p, dur, dens);
        if (defended)
        {
            int taken = hpBefore - p.hp;
            p.hp += taken / 2;
            if (p.hp > p.maxHp)
                p.hp = p.maxHp;
        }
    }

    if (p.hp <= 0)
        return false;

    clearScreen();
    cout << "\n\n";
    int boxW = mini(CW - 4, 60);
    drawBrd(boxW, 14);
    drawCBoxL("", boxW, 14);
    drawCBoxL("*** БОСС ПОВЕРЖЕН ***", boxW, 14);
    drawCBoxL(bossName + " падает!", boxW, 14);
    drawCBoxL("", boxW, 14);
    drawBrd(boxW, 14);
    int reward = 100 + level * 50;
    p.gold += reward;
    p.maxHp += 20;
    p.hp = p.maxHp;
    p.atk += 3;
    cout << "\n";
    printC("+" + to_string(reward) + " золота", 14);
    printC("+20 макс. HP, +3 АТК, полное лечение", 10);
    waitDelay(2500);
    return true;
}

void trapRoom(Player &p)
{
    cout << "\n";
    printC("Из пола вылетают шипы!", 14);
    string cb = comboRand(4);
    double acc = doQTE(cb, 1500);
    if (acc < 0.7)
    {
        int raw = randInt(10, 25);
        int dmg = calcArmor(raw, p.armor);
        p.hp -= dmg;
        printC("Вы получили " + to_string(dmg) + " урона!", 12);
    }
    else
    {
        printC("Идеальный уворот!", 10);
    }
    waitDelay(2000);
}

void chestRoom(Player &p)
{
    cout << "\n";
    printC("Вы открываете сундук...", 14);
    waitDelay(800);
    int r = rand() % 100;
    if (r < 40)
    {
        int g = randInt(20, 50);
        p.gold += g;
        printC("+" + to_string(g) + " золота!", 14);
    }
    else if (r < 65)
    {
        p.potions++;
        printC("+1 зелье лечения", 13);
    }
    else if (r < 80)
    {
        p.atk += 2;
        printC("Заточенное оружие! +2 АТК", 11);
    }
    else if (r < 92)
    {
        p.armor += 2;
        printC("Обрывки брони! +2 БРН", 9);
    }
    else
    {
        int g = randInt(60, 120);
        p.gold += g;
        printC("ДЖЕКПОТ! +" + to_string(g) + " золота", 14);
    }
    waitDelay(2200);
}

void merchantRoom(Player &p)
{
    int sel = 0;
    const int N = 6;
    while (true)
    {
        clearScreen();
        drawRoom("merchant", p.room + 1, false);
        drawHUD(p);
        cout << "\n";
        printC("--- ТОВАРЫ ТОРГОВЦА ---", 11);
        cout << "\n";

        const char *names[N] = {
            "Зелье лечения (+1)         25 золота",
            "Точильный камень (+3 АТК)  50 золота",
            "Полное лечение             40 золота",
            "Макс. HP +20               60 золота",
            "Броня +3                   55 золота",
            "Уйти"};

        for (int i = 0; i < N; i++)
        {
            string line;
            if (i == sel)
            {
                line = ">  " + string(names[i]) + "  <";
                printC(line, 14);
            }
            else
            {
                line = "   " + string(names[i]);
                printC(line, 8);
            }
        }
        cout << "\n";
        printC("[Вверх/Вниз] выбор   [Enter] купить   [Esc] выйти", 8);

        int k = _getch();
        if (k == 224 || k == 0)
        {
            int k2 = _getch();
            if (k2 == 72)
                sel = (sel - 1 + N) % N;
            else if (k2 == 80)
                sel = (sel + 1) % N;
        }
        else if (k == 13)
        {
            string msg;
            int color = 12;
            if (sel == 0)
            {
                if (p.gold >= 25)
                {
                    p.gold -= 25;
                    p.potions++;
                    msg = "+ Зелье лечения";
                    color = 10;
                }
                else
                    msg = "Не хватает золота";
            }
            else if (sel == 1)
            {
                if (p.gold >= 50)
                {
                    p.gold -= 50;
                    p.atk += 3;
                    msg = "Оружие заточено";
                    color = 10;
                }
                else
                    msg = "Не хватает золота";
            }
            else if (sel == 2)
            {
                if (p.gold >= 40)
                {
                    p.gold -= 40;
                    p.hp = p.maxHp;
                    msg = "Полностью излечены";
                    color = 10;
                }
                else
                    msg = "Не хватает золота";
            }
            else if (sel == 3)
            {
                if (p.gold >= 60)
                {
                    p.gold -= 60;
                    p.maxHp += 20;
                    p.hp += 20;
                    msg = "Макс. HP увеличен";
                    color = 10;
                }
                else
                    msg = "Не хватает золота";
            }
            else if (sel == 4)
            {
                if (p.gold >= 55)
                {
                    p.gold -= 55;
                    p.armor += 3;
                    msg = "Броня усилена";
                    color = 10;
                }
                else
                    msg = "Не хватает золота";
            }
            else
            {
                break;
            }
            cout << "\n";
            printC(msg, color);
            waitDelay(900);
        }
        else if (k == 27)
        {
            break;
        }
    }
}

string roomTypeFor(int idx, int total, bool endless)
{
    if (!endless && idx == total - 1)
        return "boss";
    int r = rand() % 100;
    if (r < 45)
        return "fight";
    if (r < 65)
        return "trap";
    if (r < 85)
        return "chest";
    return "merchant";
}

string hintFor(const string &t)
{
    // return "тишина..."; // for no info uncomm this
    if (t == "fight")
        return "слышен рык...";
    if (t == "trap")
        return "пахнет металлом...";
    if (t == "chest")
        return "что-то блестит...";
    if (t == "merchant")
        return "звенят монеты...";
    if (t == "boss")
        return "тьма пульсирует...";
    return "тишина...";
}

int chooseFork(const Player &p, const string opts[3], const string &title, bool endless)
{
    int sel = 1;
    const char *names[3] = {"Дверь СЛЕВА  ", "Дверь ВПЕРЕДИ", "Дверь СПРАВА "};
    while (true)
    {
        clearScreen();
        drawTitle(title, 13);
        cout << "\n";
        drawHUD(p, endless);
        cout << "\n";
        printC("Какую дверь выбрать?", 14);
        cout << "\n";
        for (int i = 0; i < 3; i++)
        {
            string line;
            if (i == sel)
            {
                line = ">  " + string(names[i]) + " - " + hintFor(opts[i]) + "  <";
                printC(line, 14);
            }
            else
            {
                line = "   " + string(names[i]) + " - " + hintFor(opts[i]);
                printC(line, 7);
            }
        }
        cout << "\n";
        printC("[Влево/Вправо] выбор   [Enter] войти", 8);

        int k = _getch();
        if (k == 224 || k == 0)
        {
            int k2 = _getch();
            if (k2 == 75)
                sel = (sel - 1 + 3) % 3;
            else if (k2 == 77)
                sel = (sel + 1) % 3;
        }
        else if (k == 13)
        {
            return sel;
        }
        else if (k >= '1' && k <= '3')
        {
            return k - '1';
        }
    }
}

int mainMenu()
{
    int sel = 0;
    const int N = 4;
    const char *items[N] = {
        "Сюжетный режим (10 комнат + босс)",
        "Бесконечный режим",
        "Как играть",
        "Выйти"};
    while (true)
    {
        clearScreen();
        cout << "\n\n";
        int boxW = mini(CW - 4, 70);
        drawBrd(boxW, 11);
        drawCBoxL("", boxW, 11);
        drawCBoxL("D U N G E O N   R U N", boxW, 11);
        drawCBoxL("2511-0101.2 x 2511-0101.4", boxW, 8);
        drawCBoxL("", boxW, 11);
        drawBrd(boxW, 11);
        cout << "\n\n";
        for (int i = 0; i < N; i++)
        {
            string line;
            if (i == sel)
            {
                line = ">  " + string(items[i]) + "  <";
                printC(line, 14);
            }
            else
            {
                line = "   " + string(items[i]);
                printC(line, 8);
            }
        }
        cout << "\n";
        printC("[Вверх/Вниз] выбор   [Enter] подтвердить", 8);

        int k = _getch();
        if (k == 224 || k == 0)
        {
            int k2 = _getch();
            if (k2 == 72)
                sel = (sel - 1 + N) % N;
            else if (k2 == 80)
                sel = (sel + 1) % N;
        }
        else if (k == 13)
        {
            return sel;
        }
        else if (k == 27)
        {
            return N - 1;
        }
        else if (k >= '1' && k <= '0' + N)
        {
            return k - '1';
        }
    }
}

void showHelp()
{
    clearScreen();
    cout << "\n";
    drawTitle("КАК ИГРАТЬ", 11);
    cout << "\n";
    printC("- На каждой развилке выбираешь одну из трёх дверей.", 7);
    printC("- Комнаты: бой, ловушка, сундук, торговец, босс.", 7);
    printC("- В бою: выбор цели (голова/корпус/ноги), QTE.", 7);
    printC("- 'Блок' - короткий QTE, гасит входящий урон.", 7);
    printC("- Броня снижает любой получаемый урон.", 7);
    printC("- Босс: фаза уворота - движение на WASD.", 7);
    printC("- В бесконечном режиме боссы каждые 10 комнат, выжить сложнее.", 7);
    cout << "\n";
    printC("[Esc] вернуться в меню", 8);
    while (true)
    {
        int k = _getch();
        if (k == 27 || k == 13)
            break;
    }
}

void runStory()
{
    Player p;
    int totalRooms = 10;
    while (p.room < totalRooms && p.hp > 0)
    {
        string opts[3];
        for (int i = 0; i < 3; i++)
        {
            if (p.room == totalRooms - 1)
                opts[i] = "boss";
            else
                opts[i] = roomTypeFor(p.room, totalRooms, false);
        }
        string title = "РАЗВИЛКА  -  Комната " + to_string(p.room + 1) + "/" + to_string(totalRooms);
        int idx = chooseFork(p, opts, title, false);
        string type = opts[idx];
        clearScreen();
        drawRoom(type, p.room + 1, false);
        drawHUD(p);
        waitDelay(900);
        if (type == "fight")
        {
            Monster m = makeMonster(p.room / 2);
            if (!fight(p, m) && p.hp <= 0)
                break;
        }
        else if (type == "trap")
            trapRoom(p);
        else if (type == "chest")
            chestRoom(p);
        else if (type == "merchant")
            merchantRoom(p);
        else if (type == "boss")
        {
            if (!bossFight(p, 0))
                break;
            p.room++;
            break;
        }
        p.room++;
    }
    clearScreen();
    cout << "\n\n";
    if (p.hp <= 0)
    {
        drawTitle("ВЫ ПОГИБЛИ", 4);
        cout << "\n";
        printC("Вы пали в комнате №" + to_string(p.room + 1), 12);
    }
    else
    {
        drawTitle("ПОДЗЕМЕЛЬЕ ОЧИЩЕНО", 10);
        cout << "\n";
        printC("Как насчет более глубоких пещер?", 14);
    }
    printC("Золото: " + to_string(p.gold) + "   Убийств: " + to_string(p.kills), 11);
    cout << "\n";
    printC("[Enter] вернуться в меню", 8);
    while (true)
    {
        int k = _getch();
        if (k == 13 || k == 27)
            break;
    }
}

void runEndless()
{
    Player p;
    int bossLevel = 0;
    while (p.hp > 0)
    {
        bool bossRoom = ((p.room + 1) % 10 == 0);
        string opts[3];
        for (int i = 0; i < 3; i++)
        {
            if (bossRoom)
                opts[i] = "boss";
            else
                opts[i] = roomTypeFor(p.room, 999999, true);
        }
        string title = "РАЗВИЛКА  -  Бесконечная комната " + to_string(p.room + 1);
        int idx = chooseFork(p, opts, title, true);
        string type = opts[idx];
        clearScreen();
        drawRoom(type, p.room + 1, true);
        drawHUD(p, true);
        waitDelay(900);
        if (type == "fight")
        {
            Monster m = makeMonster(p.room / 2 + 1);
            if (!fight(p, m) && p.hp <= 0)
                break;
        }
        else if (type == "trap")
            trapRoom(p);
        else if (type == "chest")
            chestRoom(p);
        else if (type == "merchant")
            merchantRoom(p);
        else if (type == "boss")
        {
            if (!bossFight(p, bossLevel))
                break;
            bossLevel++;
        }
        p.room++;
    }
    clearScreen();
    cout << "\n\n";
    drawTitle("БЕСКОНЕЧНЫЙ ЗАБЕГ ЗАВЕРШЁН", 4);
    cout << "\n";
    printC("Пройдено комнат: " + to_string(p.room), 14);
    printC("Побеждено боссов: " + to_string(bossLevel), 14);
    printC("Всего убийств: " + to_string(p.kills), 11);
    printC("Золото: " + to_string(p.gold), 14);
    cout << "\n";
    printC("[Enter] вернуться в меню", 8);
    while (true)
    {
        int k = _getch();
        if (k == 13 || k == 27)
            break;
    }
}

int main()
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned)time(0));
    csizeupd();
    hideCur();

    while (true)
    {
        int choice = mainMenu();
        if (choice == 0)
            runStory();
        else if (choice == 1)
            runEndless();
        else if (choice == 2)
            showHelp();
        else
            break;
    }

    showCur();
    clearScreen();
    printC("Спасибо за игру!", 11);
    cout << "\n";
    return 0;
}