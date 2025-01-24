#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <Windows.h>
#include <fstream>
#include <string>

using namespace std;

// Структура для збереження кольору
struct Color {
    float r, g, b;
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

class Figure { // Базовий клас для геометричних фігур
protected:
    float x, y; // Координати фігури
    Color color; // Колір фігури
    bool visible = true; // Чи видима фігура
    float scale = 1.0f; // Масштаб фігури
public:
    Figure(float x, float y, Color color) : x(x), y(y), color(color) {}
    // Віртуальні методи для реалізації у дочірніх класах
    virtual void draw() = 0;
    virtual void move(float dx, float dy) = 0;
    virtual void changeColor(Color newColor) = 0;

    // Зміна видимості фігури
    virtual void toggleVisibility() {
        visible = !visible;
    }
    virtual bool isVisible() const {
        return visible;
    }

    float getX() const { return x; }// Отримання координат 
    float getY() const { return y; }
    // Зміна масштабу
    void setScale(float newScale) {
        scale = newScale;
    }
    float getScale() const {
        return scale;
    }
};

class Triangle : public Figure { // Клас для трикутника
    float sideLength;
public:
    Triangle(float x, float y, float sideLength, Color color)
        : Figure(x, y, color), sideLength(sideLength) {}

    void draw() override {
        if (visible) {
            float scaledSideLength = sideLength * scale;
            glBegin(GL_TRIANGLES);
            glColor3f(color.r, color.g, color.b);

            //малювання вершин
            glVertex2f(x, y + scaledSideLength / (2 * sqrt(3)));
            glVertex2f(x - scaledSideLength / 2, y - scaledSideLength / (2 * sqrt(3)));
            glVertex2f(x + scaledSideLength / 2, y - scaledSideLength / (2 * sqrt(3)));
            glEnd();
        }
    }

    void move(float dx, float dy) override {
        x += dx;
        y += dy;
    }

    void changeColor(Color newColor) override {
        color = newColor;
    }
};

// Клас для квадрата
class Square : public Figure {
    float sideLength;
public:
    Square(float x, float y, float sideLength, Color color)
        : Figure(x, y, color), sideLength(sideLength) {}

    void draw() override {
        if (visible) {
            float scaledSideLength = sideLength * scale;
            glBegin(GL_QUADS);
            glColor3f(color.r, color.g, color.b);

            //малювання вершин
            glVertex2f(x - scaledSideLength / 2, y - scaledSideLength / 2);
            glVertex2f(x + scaledSideLength / 2, y - scaledSideLength / 2);
            glVertex2f(x + scaledSideLength / 2, y + scaledSideLength / 2);
            glVertex2f(x - scaledSideLength / 2, y + scaledSideLength / 2);
            glEnd();
        }
    }

    void move(float dx, float dy) override {
        x += dx;
        y += dy;
    }

    void changeColor(Color newColor) override {
        color = newColor;
    }
};

// Глобальні змінні для роботи з фігурами
vector<Figure*> figures;
bool moveMode = false;
bool colorMode = false;
int selectedFigureIndex = -1;
bool scaleMode = false;

// Colors
Color pink(205.0f / 255.0f, 104.0f / 255.0f, 185.0f / 255.0f);
Color green(76.0f / 255.0f, 148.0f / 255.0f, 98.0f / 255.0f);
Color yellow(233.0f / 255.0f, 229.0f / 255.0f, 97.0f / 255.0f);
Color purple(124.0f / 255.0f, 95.0f / 255.0f, 183.0f / 255.0f);
Color blue(95.0f / 255.0f, 151.0f / 255.0f, 183.0f / 255.0f);

void displayHelp() {
    cout << "Команди програми:\n";
    cout << "F1 - показати цю підказку\n";
    cout << "C - створити трикутник\n";
    cout << "S - створити квадрат\n";
    cout << "F - вибрати об'єкт\n";
    cout << "T - змінити колір вибраного об'єкта\n";
    cout << "M - перемістити вибраний об'єкт\n";
    cout << "V - перемикнути видимість об'єкта\n";
    cout << "R - маштабування об'єкта\n";
    cout << "W - записати положення об'єктів у файл\n";
    cout << "L - завантажити положення об'єктів з файлу\n";
}

void saveToFile(const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cout << "Помилка збереження до файлу." << endl;
        return;
    }
    for (auto obj : figures) {
        outFile << obj->getX() << ' ' << obj->getY() << ' ' << obj->getScale() << ' ' << obj->isVisible() << '\n';
    }
    outFile.close();
    cout << "Конфігурацію збережено у файл " << filename << endl;
}

void loadFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cout << "Помилка завантаження з файлу." << endl;
        return;
    }
    figures.clear();
    float x, y, scale;
    bool visibility;
    while (inFile >> x >> y >> scale >> visibility) {
        Square* square = new Square(x, y, 0.45f, blue);
        square->setScale(scale);
        if (!visibility) {
            square->toggleVisibility();
        }
        figures.push_back(square);
    }
    inFile.close();
    cout << "Конфігурацію завантажено з файлу " << filename << endl;
}

void chooseFigure() { //обираємо фігуру для маніпуляції
    cout << "Оберіть фігуру:\n";
    cout << "  1 - Квадрат\n";
    cout << "  2 - Трикутник\n";
    int choice;
    cin >> choice;

    for (size_t i = 0; i < figures.size(); ++i) {
        if ((choice == 1 && dynamic_cast<Square*>(figures[i])) ||
            (choice == 2 && dynamic_cast<Triangle*>(figures[i]))) {
            selectedFigureIndex = i;
            cout << (choice == 1 ? "Квадрат" : "Трикутник") << " вибрано." << endl;
            return;
        }
    }
    cout << "Фігура не знайдена." << endl;
}

//Ключи вибору для маніпуляції над фігурою
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_C)
        {
            figures.push_back(new Triangle(-0.45f, 0.0f, 0.6f, pink));
        }
        else if (key == GLFW_KEY_S)
        {
            figures.push_back(new Square(0.45f, 0.0f, 0.45f, blue));
        }
        else if (key == GLFW_KEY_F1)
        {
            displayHelp();
        }
        else if (key == GLFW_KEY_F)
        {
            chooseFigure(); //вибір фігури
        }
        else if (key == GLFW_KEY_T && selectedFigureIndex != -1) {
            colorMode = !colorMode;
            if (colorMode) {
                cout << "Виберіть колір:\n  G - Зелений\n  Y - Жовтий\n  P - Фіолетовий\nT - вихід з режиму" << endl;
            }
            else
            {
                cout << "Режим зміни кольору деактивовано" << endl;
            }
        }
        else if (colorMode && selectedFigureIndex != -1)
        {
            if (key == GLFW_KEY_G)
            {
                figures[selectedFigureIndex]->changeColor(green);
            }
            else if (key == GLFW_KEY_Y)
            {
                figures[selectedFigureIndex]->changeColor(yellow);
            }
            else if (key == GLFW_KEY_P)
            {
                figures[selectedFigureIndex]->changeColor(purple);
            }
        }
        else if (key == GLFW_KEY_M && selectedFigureIndex != -1)
        {
            moveMode = !moveMode;
            if (moveMode)
            {
                cout << "Режим переміщення:\n  Стрілки - переміщення\nM - вихід" << endl;
            }
            else
            {
                cout << "Режим переміщення вимкнено" << endl;
            }
        }
        else if (moveMode && selectedFigureIndex != -1)
        {
            if (key == GLFW_KEY_RIGHT)
            {
                figures[selectedFigureIndex]->move(0.05f, 0.0f);
            }
            else if (key == GLFW_KEY_LEFT)
            {
                figures[selectedFigureIndex]->move(-0.05f, 0.0f);
            }
            else if (key == GLFW_KEY_UP)
            {
                figures[selectedFigureIndex]->move(0.0f, 0.05f);
            }
            else if (key == GLFW_KEY_DOWN)
            {
                figures[selectedFigureIndex]->move(0.0f, -0.05f);
            }
        }
        else if (key == GLFW_KEY_V && selectedFigureIndex != -1)
        {
            figures[selectedFigureIndex]->toggleVisibility();
        }
        else if (key == GLFW_KEY_R)
        {
            scaleMode = !scaleMode;
            if (scaleMode) {
                cout << "Режим масштабування активовано:\n";
                cout << "  Z -> Збільшити розмір\n";
                cout << "  X -> Зменшити розмір\n";
                cout << "Натисніть R для виходу з режиму\n";
            }
            else {
                cout << "Режим масштабування вимкнено.\n";
            }

        }

        // Режим маштабування
        else if (scaleMode && !figures.empty())
        {
            if (key == GLFW_KEY_Z)
            {
                figures[selectedFigureIndex]->setScale(figures.back()->getScale() * 2.0f);
            }
            else if (key == GLFW_KEY_X)
            {
                figures[selectedFigureIndex]->setScale(figures.back()->getScale() * 0.5f);
            }
        }
        else if (key == GLFW_KEY_W)
        {
            saveToFile("config.txt");
            cout << "Конфігурацію збережено у файл config.txt" << endl;
        }
        else if (key == GLFW_KEY_L) {
            loadFromFile("config.txt");
            cout << "Конфігурацію завантажено з файлу config.txt" << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        return -1;
    }

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    displayHelp();// Виводить список доступних команд для користувача.

    // Створює вікно
    GLFWwindow* window = glfwCreateWindow(800, 600, "Graphic Object Program", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto obj : figures) {  // Перебирає всі об'єкти в списку та викликає їхню функцію малювання.
            obj->draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents(); // Обробляє події вводу 
    }


    for (auto obj : figures) {
        delete obj;
    }
    // Знищення вікна та звільнення ресурсів GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
