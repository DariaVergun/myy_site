#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <Windows.h>
#include <fstream>
#include <string>

using namespace std;

// ��������� ��� ���������� �������
struct Color {
    float r, g, b;
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

class Figure { // ������� ���� ��� ������������ �����
protected:
    float x, y; // ���������� ������
    Color color; // ���� ������
    bool visible = true; // �� ������ ������
    float scale = 1.0f; // ������� ������
public:
    Figure(float x, float y, Color color) : x(x), y(y), color(color) {}
    // ³������� ������ ��� ��������� � ������� ������
    virtual void draw() = 0;
    virtual void move(float dx, float dy) = 0;
    virtual void changeColor(Color newColor) = 0;

    // ���� �������� ������
    virtual void toggleVisibility() {
        visible = !visible;
    }
    virtual bool isVisible() const {
        return visible;
    }

    float getX() const { return x; }// ��������� ��������� 
    float getY() const { return y; }
    // ���� ��������
    void setScale(float newScale) {
        scale = newScale;
    }
    float getScale() const {
        return scale;
    }
};

class Triangle : public Figure { // ���� ��� ����������
    float sideLength;
public:
    Triangle(float x, float y, float sideLength, Color color)
        : Figure(x, y, color), sideLength(sideLength) {}

    void draw() override {
        if (visible) {
            float scaledSideLength = sideLength * scale;
            glBegin(GL_TRIANGLES);
            glColor3f(color.r, color.g, color.b);

            //��������� ������
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

// ���� ��� ��������
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

            //��������� ������
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

// �������� ���� ��� ������ � ��������
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
    cout << "������� ��������:\n";
    cout << "F1 - �������� �� �������\n";
    cout << "C - �������� ���������\n";
    cout << "S - �������� �������\n";
    cout << "F - ������� ��'���\n";
    cout << "T - ������ ���� ��������� ��'����\n";
    cout << "M - ���������� �������� ��'���\n";
    cout << "V - ����������� �������� ��'����\n";
    cout << "R - ������������ ��'����\n";
    cout << "W - �������� ��������� ��'���� � ����\n";
    cout << "L - ����������� ��������� ��'���� � �����\n";
}

void saveToFile(const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cout << "������� ���������� �� �����." << endl;
        return;
    }
    for (auto obj : figures) {
        outFile << obj->getX() << ' ' << obj->getY() << ' ' << obj->getScale() << ' ' << obj->isVisible() << '\n';
    }
    outFile.close();
    cout << "������������ ��������� � ���� " << filename << endl;
}

void loadFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile) {
        cout << "������� ������������ � �����." << endl;
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
    cout << "������������ ����������� � ����� " << filename << endl;
}

void chooseFigure() { //������� ������ ��� ����������
    cout << "������ ������:\n";
    cout << "  1 - �������\n";
    cout << "  2 - ���������\n";
    int choice;
    cin >> choice;

    for (size_t i = 0; i < figures.size(); ++i) {
        if ((choice == 1 && dynamic_cast<Square*>(figures[i])) ||
            (choice == 2 && dynamic_cast<Triangle*>(figures[i]))) {
            selectedFigureIndex = i;
            cout << (choice == 1 ? "�������" : "���������") << " �������." << endl;
            return;
        }
    }
    cout << "Գ���� �� ��������." << endl;
}

//����� ������ ��� ���������� ��� �������
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
            chooseFigure(); //���� ������
        }
        else if (key == GLFW_KEY_T && selectedFigureIndex != -1) {
            colorMode = !colorMode;
            if (colorMode) {
                cout << "������� ����:\n  G - �������\n  Y - ������\n  P - Գ��������\nT - ����� � ������" << endl;
            }
            else
            {
                cout << "����� ���� ������� ������������" << endl;
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
                cout << "����� ����������:\n  ������ - ����������\nM - �����" << endl;
            }
            else
            {
                cout << "����� ���������� ��������" << endl;
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
                cout << "����� ������������� ����������:\n";
                cout << "  Z -> �������� �����\n";
                cout << "  X -> �������� �����\n";
                cout << "�������� R ��� ������ � ������\n";
            }
            else {
                cout << "����� ������������� ��������.\n";
            }

        }

        // ����� ������������
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
            cout << "������������ ��������� � ���� config.txt" << endl;
        }
        else if (key == GLFW_KEY_L) {
            loadFromFile("config.txt");
            cout << "������������ ����������� � ����� config.txt" << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        return -1;
    }

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    displayHelp();// �������� ������ ��������� ������ ��� �����������.

    // ������� ����
    GLFWwindow* window = glfwCreateWindow(800, 600, "Graphic Object Program", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto obj : figures) {  // �������� �� ��'���� � ������ �� ������� ���� ������� ���������.
            obj->draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents(); // �������� ��䳿 ����� 
    }


    for (auto obj : figures) {
        delete obj;
    }
    // �������� ���� �� ��������� ������� GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
