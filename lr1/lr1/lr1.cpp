#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdexcept>
#include <cmath>
#include <iomanip>

using namespace std;

// Власні винятки
class FileNotFound : public exception {
    string msg;
public:
    FileNotFound(const string& filename) : msg("Ne vdalosia vidkryty fail: " + filename) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

class DivisionByZero : public exception {
    string msg;
public:
    DivisionByZero(const string& info) : msg("Pomylka dilennia na nul: " + info) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// Інтерполяція з таблиці (x y)
double interpolate(const string& filename, double x) {
    ifstream file(filename);
    if (!file.is_open())
        throw FileNotFound(filename);

    double x1, y1, x2, y2;
    bool first = true;
    while (file >> x2 >> y2) {
        if (!first) {
            if (x >= x1 && x <= x2) {
                if (x1 == x2) throw DivisionByZero("x1 == x2 pid chas interpoliatsii");
                return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
            }
        }
        x1 = x2; y1 = y2;
        first = false;
    }

    throw out_of_range("pid chas interpoliatsii " + filename);
}

// Пошук текстового значення
double getGText(const string& filename, const string& text) {
    ifstream file(filename);
    if (!file.is_open())
        throw FileNotFound(filename);

    string word;
    double val;
    while (file >> word >> val) {
        if (word == text) return val;
    }

    return 0.0; // Якщо не знайдено
}

// Обчислення M
double calcM(double u, double v, const string& file1, const string& file2) {
    double Tu = interpolate(file1, u);
    double Tv = interpolate(file1, v);
    double Uu = interpolate(file2, u);
    double Uv = interpolate(file2, v);

    return Tu * u - Tv * v + Uu - Uv;
}

// Обчислення D
double calcD(double u, double v, const string& file1, const string& file2) {
    return calcM(u, v, file1, file2) + calcM(v, u, file1, file2);
}

// Основна функція
double func(double u, double v, const string& text,
    const string& file1, const string& file2, const string& file3) {
    if (v == 0) throw DivisionByZero("v = 0 у func");

    double part1 = 0.45 * u + 0.55 * v;
    double dval;

    try {
        dval = calcD(u, v, file1, file2);
    }
    catch (exception& e) {
        cerr << "[Poperedzhennia] Vykorystovuietsia zapasna formula D cherez vyniatok: " << e.what() << endl;
        dval = 1.23 * u - 1.15 * v;
    }

    double gtext = getGText(file3, text);
    return part1 + dval + gtext;
}

int main() {

    double u, v;
    string text;

    cout << fixed << setprecision(4);
    cout << "Vvedit u, v: ";
    cin >> u >> v;
    cout << "Vvedit tekstovyi riadok: ";
    cin >> text;

    try {
        double result = func(u, v, text, "dat1.dat", "dat2.dat", "dat3.dat");
        cout << "Rezultat: " << result << endl;
    }
    catch (exception& e) {
        cout << "Pomylka: " << e.what() << endl;
    }

    return 0;
}
