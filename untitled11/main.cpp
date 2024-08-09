#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <limits>
#include <map>

using namespace std;

class Car {
public:
    string brand;
    int year;
    string technicalSpecs;
    string features;
    string condition;
    double price;

    Car(string b, int y, string ts, string f, string c, double p)
            : brand(b), year(y), technicalSpecs(ts), features(f), condition(c), price(p) {}

    string toString() const {
        stringstream ss;
        ss << brand << " (" << year << ") - " << price << " грн\n"
           << "Технічні характеристики:\n" << technicalSpecs << "\n"
           << "Особливості: " << features << "\n"
           << "Стан: " << condition << "\n";
        return ss.str();
    }
};

class User {
public:
    string firstName;
    string lastName;
    string patronymic;
    string phoneNumber;
    string country;
    string city;

    User(string fn, string ln, string p, string pn, string co, string ci)
            : firstName(fn), lastName(ln), patronymic(p), phoneNumber(pn), country(co), city(ci) {}

    string toString() const {
        return firstName + " " + lastName + " " + patronymic;
    }
};

class AutoDealership {
private:
    vector<Car> cars;
    vector<User> users;

public:
    bool isUkrainian(const string& str) const {
        return any_of(str.begin(), str.end(), [](unsigned char c) {
            return (c >= 0xC0 && c <= 0xFF) || c == 0xA8 || c == 0xB8;
        });
    }

    bool isEnglish(const string& str) const {
        return all_of(str.begin(), str.end(), [](unsigned char c) {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ';
        });
    }

    void addCar(const Car& car) {
        cars.push_back(car);
        saveAllCars();
    }

    void removeCar(const string& brand, int year) {
        cars.erase(remove_if(cars.begin(), cars.end(),
                             [&](const Car& c) { return c.brand == brand && c.year == year; }), cars.end());
        saveAllCars();
    }

    vector<Car> getAllCars() const {
        return cars;
    }

    vector<string> getAllCarBrands() const {
        vector<string> brands;
        for (const auto& car : cars) {
            if (find(brands.begin(), brands.end(), car.brand) == brands.end()) {
                brands.push_back(car.brand);
            }
        }
        return brands;
    }

    void addUser(const User& user) {
        users.push_back(user);
        saveUserData(user);
    }

    bool authenticateAdmin(const string& username, const string& password) {
        ifstream file("admin.txt");
        string un, pw;
        if (file >> un >> pw) {
            return (un == username && pw == password);
        }
        return false;
    }

    void saveToFile() const {
        ofstream carsFile("cars.txt");
        ofstream specsFile("specs.txt");
        for (const auto& car : cars) {
            carsFile << car.brand << "," << car.year << "," << car.price << "\n";
            specsFile << car.brand << "," << car.year << "," << car.technicalSpecs << ","
                      << car.features << "," << car.condition << "\n";
        }
    }

    void loadFromFile() {
        ifstream carsFile("cars.txt");
        ifstream specsFile("specs.txt");
        string carLine, specLine;
        while (getline(carsFile, carLine) && getline(specsFile, specLine)) {
            stringstream carSS(carLine), specSS(specLine);
            string brand, technicalSpecs, features, condition;
            int year;
            double price;
            getline(carSS, brand, ',');
            carSS >> year >> price;
            getline(specSS, brand, ',');
            specSS >> year;
            specSS.ignore();
            getline(specSS, technicalSpecs, ',');
            getline(specSS, features, ',');
            getline(specSS, condition);
            cars.emplace_back(brand, year, technicalSpecs, features, condition, price);
        }
    }

    void saveAllCars() const {
        saveToFile();
    }

    void loadAllCars() {
        loadFromFile();
    }

    void saveFeedback(const string& feedback) const {
        ofstream file("feedback.txt", ios::app);
        time_t now = time(0);
        char* dt = ctime(&now);
        file << dt << feedback << "\n\n";
    }

    void saveAdminData(const string& username, const string& password) const {
        ofstream file("admin.txt");
        file << username << " " << password;
    }

    void saveUserData(const User& user) const {
        ofstream file("users.txt", ios::app);
        file << user.firstName << "," << user.lastName << "," << user.patronymic << ","
             << user.phoneNumber << "," << user.country << "," << user.city << "\n";
    }

    vector<Car> getCarsByFilters(const string& brand, int minYear, int maxYear, double minPrice, double maxPrice,
                                 const string& techSpecs, const string& features, const string& condition) const {
        vector<Car> filteredCars;
        copy_if(cars.begin(), cars.end(), back_inserter(filteredCars),
                [&](const Car& car) {
                    return (brand.empty() || car.brand == brand) &&
                           car.year >= minYear && car.year <= maxYear &&
                           car.price >= minPrice && car.price <= maxPrice &&
                           (techSpecs.empty() || car.technicalSpecs.find(techSpecs) != string::npos) &&
                           (features.empty() || car.features.find(features) != string::npos) &&
                           (condition.empty() || car.condition.find(condition) != string::npos);
                });
        return filteredCars;
    }

    bool buyCar(const string& brand, int year) {
        auto it = find_if(cars.begin(), cars.end(),
                          [&](const Car& c) { return c.brand == brand && c.year == year; });
        if (it != cars.end()) {
            cars.erase(it);
            saveAllCars();
            return true;
        }
        return false;
    }

    void saveApplication(const User& user, const Car& car) const {
        ofstream file("applications.txt", ios::app);
        file << "Заява на придбання автомобіля\n"
             << "Покупець: " << user.firstName << " " << user.lastName << " " << user.patronymic << "\n"
             << "Телефон: " << user.phoneNumber << "\n"
             << "Країна: " << user.country << "\n"
             << "Місто: " << user.city << "\n"
             << "Автомобіль: " << car.brand << " " << car.year << "\n"
             << "Ціна: " << car.price << " грн\n"
             << "Дата: " << getCurrentDateTime() << "\n\n";
    }

    string getCurrentDateTime() const {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
        return ss.str();
    }

    void updateCar(const Car& car) {
        auto it = find_if(cars.begin(), cars.end(),
                          [&](const Car& c) { return c.brand == car.brand && c.year == car.year; });
        if (it != cars.end()) {
            *it = car;
            saveAllCars();
        }
    }

    vector<User> getAllUsers() const {
        return users;
    }

    void updateUser(const User& user) {
        auto it = find_if(users.begin(), users.end(),
                          [&](const User& u) { return u.firstName == user.firstName && u.lastName == user.lastName; });
        if (it != users.end()) {
            *it = user;
            saveAllUsers();
        }
    }

    void removeUser(const User& user) {
        users.erase(remove_if(users.begin(), users.end(),
                              [&](const User& u) { return u.firstName == user.firstName && u.lastName == user.lastName; }),
                    users.end());
        saveAllUsers();
    }

    void saveAllUsers() const {
        ofstream file("users.txt");
        for (const auto& user : users) {
            file << user.firstName << "," << user.lastName << "," << user.patronymic << ","
                 << user.phoneNumber << "," << user.country << "," << user.city << "\n";
        }
    }

    void loadAllUsers() {
        ifstream file("users.txt");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string firstName, lastName, patronymic, phoneNumber, country, city;
            getline(ss, firstName, ',');
            getline(ss, lastName, ',');
            getline(ss, patronymic, ',');
            getline(ss, phoneNumber, ',');
            getline(ss, country, ',');
            getline(ss, city);
            users.emplace_back(firstName, lastName, patronymic, phoneNumber, country, city);
        }
    }
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitForEnter() {
    cout << "\nНатисніть Enter, щоб продовжити...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void displayMainMenu() {
    clearScreen();
    cout << "=== Головне меню ===\n";
    cout << "1) Увійти як адміністратор\n";
    cout << "2) Увійти як покупець\n";
    cout << "3) Посібник для покупця\n";
    cout << "4) Переглянути відгуки про автосалон\n";
    cout << "5) Вийти з програми\n";
    cout << "Виберіть опцію: ";
}

string getTechnicalSpecs() {
    string specs;
    cout << "Для введення технічних характеристик натисніть 1: ";
    int choice;
    cin >> choice;
    if (choice == 1) {
        string model, bodyType, transmission, drive, sizeAndWeight;
        cout << "Модель машини: ";
        cin.ignore();
        getline(cin, model);
        cout << "Тип кузова: ";
        getline(cin, bodyType);
        cout << "Коробка передач: ";
        getline(cin, transmission);
        cout << "Привід: ";
        getline(cin, drive);
        cout << "Розмір і вага: ";
        getline(cin, sizeAndWeight);
        specs = "Модель: " + model + ", Кузов: " + bodyType + ", КПП: " + transmission +
                ", Привід: " + drive + ", Розмір і вага: " + sizeAndWeight;
    }
    return specs;
}

string getFeatures() {
    string color, headlights;
    int airbags;
    cout << "Колір автомобіля: ";
    cin.ignore();
    getline(cin, color);
    cout << "Тип фар: ";
    getline(cin, headlights);
    cout << "Кількість подушок безпеки: ";
    cin >> airbags;
    return "Колір: " + color + ", Фари: " + headlights + ", Подушок безпеки: " + to_string(airbags);
}

void adminMenu(AutoDealership& dealership) {
    string username, password;
    cout << "Введіть ім'я користувача: ";
    cin >> username;
    cout << "Введіть пароль: ";
    cin >> password;
    if (!dealership.authenticateAdmin(username, password)) {
        throw runtime_error("Авторизація не вдалась. Спробуйте ще раз.");
    }
    while (true) {
        clearScreen();
        cout << "=== Меню адміністратора ===\n";
        cout << "1) Додати машину\n";
        cout << "2) Видалити машину\n";
        cout << "3) Редагувати машину\n";
        cout << "4) Переглянути всі машини\n";
        cout << "5) Переглянути всіх користувачів\n";
        cout << "6) Редагувати дані користувача\n";
        cout << "7) Видалити користувача\n";
        cout << "8) Повернутися назад\n";
        cout << "Виберіть опцію: ";
        int choice;
        cin >> choice;
        switch (choice) {
            case 1: {
                string brand, condition;
                int year;
                double price;
                cout << "Введіть марку машини: ";
                cin.ignore();
                getline(cin, brand);
                while (!(dealership.isUkrainian(brand) || dealership.isEnglish(brand))) {
                    cout << "Така назва марки машини не існує. Введіть ще раз: ";
                    getline(cin, brand);
                }
                cout << "Введіть рік випуску: ";
                cin >> year;
                while (year < 1995 || year > 2024) {
                    if (year < 1995) {
                        cout << "Програма приймає машини лише з 1995 роком випуску. Спробуйте ще раз: ";
                    } else {
                        cout << "Такий рік ще не настав: " << year << ". Введіть ще раз: ";
                    }
                    cin >> year;
                }
                string technicalSpecs = getTechnicalSpecs();
                cout << "Особливості виконання:\n";
                string features = getFeatures();
                cout << "Введіть технічний стан: ";
                cin.ignore();
                getline(cin, condition);
                cout << "Введіть запитувану ціну: ";
                cin >> price;
                Car newCar(brand, year, technicalSpecs, features, condition, price);
                dealership.addCar(newCar);
                cout << "Машина успішно додана.\n";
                waitForEnter();
                break;
            }
            case 2: {
                auto cars = dealership.getAllCars();
                if (cars.empty()) {
                    cout << "У автосалоні немає машин.\n";
                    waitForEnter();
                    continue;
                }
                cout << "Список машин:\n";
                for (size_t i = 0; i < cars.size(); ++i) {
                    cout << i + 1 << ") " << cars[i].brand << " " << cars[i].year << " - " << cars[i].price << " грн\n";
                }
                cout << "Виберіть номер машини для видалення: ";
                size_t carIndex;
                cin >> carIndex;
                if (carIndex < 1 || carIndex > cars.size()) {
                    cout << "Невірний вибір.\n";
                    waitForEnter();
                    continue;
                }
                dealership.removeCar(cars[carIndex - 1].brand, cars[carIndex - 1].year);
                cout << "Машина успішно видалена.\n";
                waitForEnter();
                break;
            }
            case 3: {
                auto cars = dealership.getAllCars();
                if (cars.empty()) {
                    cout << "У автосалоні немає машин.\n";
                    waitForEnter();
                    continue;
                }
                cout << "Список машин:\n";
                for (size_t i = 0; i < cars.size(); ++i) {
                    cout << i + 1 << ") " << cars[i].brand << " " << cars[i].year << " - " << cars[i].price << " грн\n";
                }
                cout << "Виберіть номер машини для редагування: ";
                size_t carIndex;
                cin >> carIndex;
                if (carIndex < 1 || carIndex > cars.size()) {
                    cout << "Невірний вибір.\n";
                    waitForEnter();
                    continue;
                }
                Car& carToEdit = cars[carIndex - 1];
                cout << "Редагування машини " << carToEdit.brand << " " << carToEdit.year << "\n";
                cout << "Нова марка (або Enter, щоб залишити без змін): ";
                string newBrand;
                cin.ignore();
                getline(cin, newBrand);
                if (!newBrand.empty()) carToEdit.brand = newBrand;
                cout << "Новий рік (або 0, щоб залишити без змін): ";
                int newYear;
                cin >> newYear;
                if (newYear != 0) carToEdit.year = newYear;
                cout << "Нові технічні характеристики:\n";
                string newSpecs = getTechnicalSpecs();
                if (!newSpecs.empty()) carToEdit.technicalSpecs = newSpecs;
                cout << "Нові особливості:\n";
                string newFeatures = getFeatures();
                if (!newFeatures.empty()) carToEdit.features = newFeatures;
                cout << "Новий стан (або Enter, щоб залишити без змін): ";
                string newCondition;
                cin.ignore();
                getline(cin, newCondition);
                if (!newCondition.empty()) carToEdit.condition = newCondition;
                cout << "Нова ціна (або 0, щоб залишити без змін): ";
                double newPrice;
                cin >> newPrice;
                if (newPrice != 0) carToEdit.price = newPrice;
                dealership.updateCar(carToEdit);
                cout << "Машину успішно відредаговано.\n";
                waitForEnter();
                break;
            }
            case 4: {
                auto cars = dealership.getAllCars();
                if (cars.empty()) {
                    cout << "У автосалоні немає машин.\n";
                } else {
                    cout << "Список всіх машин:\n";
                    for (const auto& car : cars) {
                        cout << car.toString() << "\n";
                    }
                }
                waitForEnter();
                break;
            }
            case 5: {
                auto users = dealership.getAllUsers();
                if (users.empty()) {
                    cout << "Немає зареєстрованих користувачів.\n";
                } else {
                    cout << "Список всіх користувачів:\n";
                    for (const auto& user : users) {
                        cout << user.toString() << "\n";
                    }
                }
                waitForEnter();
                break;
            }
            case 6: {
                auto users = dealership.getAllUsers();
                if (users.empty()) {
                    cout << "Немає зареєстрованих користувачів.\n";
                    waitForEnter();
                    continue;
                }
                cout << "Список користувачів:\n";
                for (size_t i = 0; i < users.size(); ++i) {
                    cout << i + 1 << ") " << users[i].firstName << " " << users[i].lastName << "\n";
                }
                cout << "Виберіть номер користувача для редагування: ";
                size_t userIndex;
                cin >> userIndex;
                if (userIndex < 1 || userIndex > users.size()) {
                    cout << "Невірний вибір.\n";
                    waitForEnter();
                    continue;
                }
                User& userToEdit = users[userIndex - 1];
                cout << "Редагування користувача " << userToEdit.firstName << " " << userToEdit.lastName << "\n";

                cout << "Нове ім'я (або Enter, щоб залишити без змін): ";
                string newFirstName;
                cin.ignore();
                getline(cin, newFirstName);
                if (!newFirstName.empty()) userToEdit.firstName = newFirstName;

                cout << "Нове прізвище (або Enter, щоб залишити без змін): ";
                string newLastName;
                getline(cin, newLastName);
                if (!newLastName.empty()) userToEdit.lastName = newLastName;

                cout << "Нове по-батькові (або Enter, щоб залишити без змін): ";
                string newPatronymic;
                getline(cin, newPatronymic);
                if (!newPatronymic.empty()) userToEdit.patronymic = newPatronymic;

                cout << "Новий номер телефону (або Enter, щоб залишити без змін): ";
                string newPhoneNumber;
                getline(cin, newPhoneNumber);
                if (!newPhoneNumber.empty()) userToEdit.phoneNumber = newPhoneNumber;

                cout << "Нова країна (або Enter, щоб залишити без змін): ";
                string newCountry;
                getline(cin, newCountry);
                if (!newCountry.empty()) userToEdit.country = newCountry;

                cout << "Нове місто (або Enter, щоб залишити без змін): ";
                string newCity;
                getline(cin, newCity);
                if (!newCity.empty()) userToEdit.city = newCity;

                dealership.updateUser(userToEdit);
                cout << "Дані користувача успішно відредаговано.\n";
                waitForEnter();
                break;
            }
            case 7: {
                auto users = dealership.getAllUsers();
                if (users.empty()) {
                    cout << "Немає зареєстрованих користувачів.\n";
                    waitForEnter();
                    continue;
                }
                cout << "Список користувачів:\n";
                for (size_t i = 0; i < users.size(); ++i) {
                    cout << i + 1 << ") " << users[i].firstName << " " << users[i].lastName << "\n";
                }
                cout << "Виберіть номер користувача для видалення: ";
                size_t userIndex;
                cin >> userIndex;
                if (userIndex < 1 || userIndex > users.size()) {
                    cout << "Невірний вибір.\n";
                    waitForEnter();
                    continue;
                }
                dealership.removeUser(users[userIndex - 1]);
                cout << "Користувача успішно видалено.\n";
                waitForEnter();
                break;
            }
            case 8:
                return;
            default:
                cout << "Невірний вибір. Спробуйте ще раз.\n";
                waitForEnter();
        }
    }
}

void customerMenu(AutoDealership& dealership) {
    string firstName, lastName, patronymic, phoneNumber, country, city;
    cout << "Введіть ім'я: ";
    cin >> firstName;
    cout << "Введіть прізвище: ";
    cin >> lastName;
    cout << "Введіть по-батькові: ";
    cin >> patronymic;
    cout << "Введіть номер телефону: ";
    cin >> phoneNumber;
    cout << "Введіть країну проживання: ";
    cin >> country;
    cout << "Введіть місто проживання: ";
    cin >> city;

    User newUser(firstName, lastName, patronymic, phoneNumber, country, city);
    dealership.addUser(newUser);

    while (true) {
        clearScreen();
        cout << "=== Меню покупця ===\n";
        cout << "1) Переглянути всі машини\n";
        cout << "2) Знайти машину за параметрами\n";
        cout << "3) Купити машину\n";
        cout << "4) Залишити відгук про сервіс\n";
        cout << "5) Переглянути заяву на придбання\n";
        cout << "6) Повернутися назад\n";
        cout << "Виберіть опцію: ";
        int choice;
        cin >> choice;
        switch (choice) {
            case 1: {
                auto allCars = dealership.getAllCars();
                for (const auto& car : allCars) {
                    cout << car.toString() << "\n";
                }
                waitForEnter();
                break;
            }
            case 2: {
                string brand;
                int minYear, maxYear;
                double minPrice, maxPrice;
                string techSpecs, features, condition;

                auto brands = dealership.getAllCarBrands();
                cout << "Доступні марки машин:\n";
                for (size_t i = 0; i < brands.size(); ++i) {
                    cout << i + 1 << ") " << brands[i] << "\n";
                }
                cout << "Виберіть номер марки (або 0 для будь-якої): ";
                size_t brandIndex;
                cin >> brandIndex;
                if (brandIndex > 0 && brandIndex <= brands.size()) {
                    brand = brands[brandIndex - 1];
                }

                cout << "Введіть мінімальний рік випуску: ";
                cin >> minYear;
                cout << "Введіть максимальний рік випуску: ";
                cin >> maxYear;
                cout << "Введіть мінімальну ціну: ";
                cin >> minPrice;
                cout << "Введіть максимальну ціну: ";
                cin >> maxPrice;

                cout << "Бажаєте ввести технічні характеристики? (1 - так, 0 - ні): ";
                int techChoice;
                cin >> techChoice;
                if (techChoice == 1) {
                    techSpecs = getTechnicalSpecs();
                }

                cout << "Бажаєте ввести особливості виконання? (1 - так, 0 - ні): ";
                int featuresChoice;
                cin >> featuresChoice;
                if (featuresChoice == 1) {
                    features = getFeatures();
                }

                cout << "Введіть бажаний технічний стан (або Enter для пропуску): ";
                cin.ignore();
                getline(cin, condition);

                auto filteredCars = dealership.getCarsByFilters(brand, minYear, maxYear, minPrice, maxPrice, techSpecs, features, condition);
                if (filteredCars.empty()) {
                    cout << "На жаль, таких машин немає в наявності.\n";
                } else {
                    cout << "Знайдені машини:\n";
                    for (const auto& car : filteredCars) {
                        cout << car.toString() << "\n";
                    }
                }
                waitForEnter();
                break;
            }
            case 3: {
                auto brands = dealership.getAllCarBrands();
                cout << "Доступні марки машин:\n";
                for (size_t i = 0; i < brands.size(); ++i) {
                    cout << i + 1 << ") " << brands[i] << "\n";
                }
                cout << "Виберіть номер марки: ";
                size_t brandIndex;
                cin >> brandIndex;
                if (brandIndex < 1 || brandIndex > brands.size()) {
                    cout << "Невірний вибір.\n";
                    waitForEnter();
                    continue;
                }
                string selectedBrand = brands[brandIndex - 1];

                int minYear, maxYear;
                cout << "Введіть мінімальний рік випуску: ";
                cin >> minYear;
                cout << "Введіть максимальний рік випуску: ";
                cin >> maxYear;

                auto filteredCars = dealership.getCarsByFilters(selectedBrand, minYear, maxYear, 0, numeric_limits<double>::max(), "", "", "");
                if (filteredCars.empty()) {
                    cout << "На жаль, таких машин немає в наявності.\n";
                    waitForEnter();
                    continue;
                }

                cout << "Знайдені машини:\n";
                for (size_t i = 0; i < filteredCars.size(); ++i) {
                    cout << i + 1 << ") " << filteredCars[i].toString() << "\n";
                }

                cout << "Виберіть номер машини для покупки: ";
                size_t carIndex;
                cin >> carIndex;
                if (carIndex < 1 || carIndex > filteredCars.size()) {
                    cout << "Невірний вибір.\n";
                    waitForEnter();
                    continue;
                }

                const Car& selectedCar = filteredCars[carIndex - 1];
                if (dealership.buyCar(selectedCar.brand, selectedCar.year)) {
                    cout << "Вітаємо! Ви успішно придбали " << selectedCar.brand << " " << selectedCar.year << " року.\n";
                    dealership.saveApplication(newUser, selectedCar);
                    cout << "Вашу заявку сформовано і відправлено постачальнику.\n";
                } else {
                    cout << "На жаль, ця машина вже недоступна.\n";
                }
                waitForEnter();
                break;
            }
            case 4: {
                string feedback;
                cout << "Будь ласка, залиште свій відгук про сервіс:\n";
                cin.ignore();
                getline(cin, feedback);
                dealership.saveFeedback(feedback);
                cout << "Дякуємо за ваш відгук!\n";
                waitForEnter();
                break;
            }
            case 5: {
                ifstream file("applications.txt");
                if (file.is_open()) {
                    string line;
                    bool found = false;
                    while (getline(file, line)) {
                        if (line.find(newUser.firstName + " " + newUser.lastName + " " + newUser.patronymic) != string::npos) {
                            found = true;
                            cout << line << "\n";
                            for (int i = 0; i < 6; ++i) {
                                if (getline(file, line)) {
                                    cout << line << "\n";
                                }
                            }
                            break;
                        }
                    }
                    if (!found) {
                        cout << "Ви ще не подавали заяву на придбання автомобіля.\n";
                    }
                    file.close();
                } else {
                    cout << "Не вдалося відкрити файл з заявами.\n";
                }
                waitForEnter();
                break;
            }
            case 6:
                return;
            default:
                cout << "Невірний вибір. Спробуйте ще раз.\n";
                waitForEnter();
        }
    }
}

void buyerGuide() {
    clearScreen();
    cout << "=== Посібник для покупця ===\n";
    cout << "1. Перегляньте доступні машини в нашому автосалоні.\n";
    cout << "2. Використовуйте пошук за параметрами, щоб знайти машину, яка вам підходить.\n";
    cout << "3. Для покупки машини виберіть опцію 'Купити машину' в меню покупця.\n";
    cout << "4. Виберіть марку автомобіля зі списку доступних.\n";
    cout << "5. Введіть діапазон року випуску, який вас цікавить.\n";
    cout << "6. Виберіть конкретну машину зі списку знайдених.\n";
    cout << "7. Після успішної покупки ви отримаєте підтвердження.\n";
    cout << "8. Ваша заявка буде автоматично сформована і відправлена постачальнику.\n";
    cout << "9. Ви можете переглянути свою заяву в меню покупця.\n";
    cout << "10. Не забудьте залишити відгук про наш сервіс!\n";
    waitForEnter();
}

void displayFeedbacks() {
    clearScreen();
    cout << "=== Відгуки про автосалон ===\n";
    ifstream file("feedback.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << "\n";
        }
        file.close();
    } else {
        cout << "Не вдалося відкрити файл з відгуками.\n";
    }
    waitForEnter();
}

int main() {
    AutoDealership dealership;
    dealership.loadAllCars();
    dealership.loadAllUsers();

    // Перевіряємо, чи існує файл з даними адміністратора
    ifstream adminFile("admin.txt");
    if (!adminFile) {
        string adminUsername, adminPassword;
        cout << "Файл з даними адміністратора не знайдено. Будь ласка, створіть обліковий запис адміністратора:\n";
        cout << "Ім'я користувача: ";
        cin >> adminUsername;
        cout << "Пароль: ";
        cin >> adminPassword;
        dealership.saveAdminData(adminUsername, adminPassword);
    }
    adminFile.close();

    while (true) {
        displayMainMenu();
        int choice;
        cin >> choice;
        try {
            switch (choice) {
                case 1:
                    adminMenu(dealership);
                    break;
                case 2:
                    customerMenu(dealership);
                    break;
                case 3:
                    buyerGuide();
                    break;
                case 4:
                    displayFeedbacks();
                    break;
                case 5:
                    cout << "Дякуємо за використання програми. До побачення!\n";
                    return 0;
                default:
                    cout << "Невірний вибір. Спробуйте ще раз.\n";
                    waitForEnter();
            }
        } catch (const exception& e) {
            cout << "Помилка: " << e.what() << "\n";
            waitForEnter();
        }
    }
    return 0;
}