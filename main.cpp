#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>
#include <vector>
#include <cstdlib>
#include <ctime>

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(int size, QWidget *parent = nullptr) : QWidget(parent), N(size) {
        srand(time(nullptr));
        firstPlayer = "X";
        currentPlayer = firstPlayer;
        setupUI();
    }

private:
    int N;
    const int WIN_COUNT = 4;
    QString currentPlayer;
    QString firstPlayer;
    QLabel *statusLabel;
    QPushButton *clearButton;
    QGridLayout *gridLayout;
    std::vector<std::vector<QPushButton*>> buttons;

    void setupUI() {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QHBoxLayout *topLayout = new QHBoxLayout();

        statusLabel = new QLabel("Ходит: Игрок 1");
        clearButton = new QPushButton("Clear");
        connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearBoard);

        topLayout->addWidget(statusLabel);
        topLayout->addWidget(clearButton);
        mainLayout->addLayout(topLayout);

        gridLayout = new QGridLayout();
        for (int i = 0; i < N; ++i) {
            std::vector<QPushButton*> row;
            for (int j = 0; j < N; ++j) {
                QPushButton *btn = new QPushButton("");
                btn->setFixedSize(50, 50);
                btn->setStyleSheet("font-size: 18px;");
                connect(btn, &QPushButton::clicked, [=]() { handleClick(i, j); });
                gridLayout->addWidget(btn, i, j);
                row.push_back(btn);
            }
            buttons.push_back(row);
        }

        mainLayout->addLayout(gridLayout);
        setLayout(mainLayout);
        setWindowTitle("Крестики-нолики");
        resize(60 * N, 60 * N);
    }

    void handleClick(int i, int j) {
        QPushButton *btn = buttons[i][j];
        if (!btn->text().isEmpty() || currentPlayer != "X") return;

        btn->setText("X");
        if (checkWin(i, j)) {
            statusLabel->setText("Победитель: Игрок 1");
            disableAll();
            return;
        }
        currentPlayer = "O";
        statusLabel->setText("Ходит: Игрок 2...");
        QTimer::singleShot(300, this, &MainWindow::computerMove);
    }

    void computerMove() {
        std::vector<std::pair<int, int>> empty;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                if (buttons[i][j]->text().isEmpty())
                    empty.push_back({i, j});

        if (empty.empty()) return;

        auto [i, j] = empty[rand() % empty.size()];
        buttons[i][j]->setText("O");

        if (checkWin(i, j)) {
            statusLabel->setText("Победитель: Игрок 2");
            disableAll();
            return;
        }

        currentPlayer = "X";
        statusLabel->setText("Ходит: Игрок 1");
    }

    bool checkWin(int x, int y) {
        QString sym = buttons[x][y]->text();
        auto count = [&](int dx, int dy) {
            int cnt = 1;
            for (int dir = -1; dir <= 1; dir += 2) {
                int i = x + dx * dir, j = y + dy * dir;
                while (i >= 0 && i < N && j >= 0 && j < N && buttons[i][j]->text() == sym) {
                    ++cnt;
                    i += dx * dir;
                    j += dy * dir;
                }
            }
            return cnt;
        };
        return count(1, 0) >= WIN_COUNT || count(0, 1) >= WIN_COUNT || count(1, 1) >= WIN_COUNT || count(1, -1) >= WIN_COUNT;
    }

    void disableAll() {
        for (auto &row : buttons)
            for (auto *btn : row)
                btn->setEnabled(false);
    }

    void clearBoard() {
        for (auto &row : buttons)
            for (auto *btn : row) {
                btn->setText("");
                btn->setEnabled(true);
            }
        firstPlayer = (firstPlayer == "X") ? "O" : "X";
        currentPlayer = firstPlayer;
        statusLabel->setText(QString("Ходит: %1").arg(firstPlayer == "X" ? "Игрок 1" : "Игрок 2"));

        if (currentPlayer == "O") {
            QTimer::singleShot(300, this, &MainWindow::computerMove);
        }
    }
};

class StartWindow : public QWidget {
    Q_OBJECT
public:
    StartWindow(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        QLabel *label = new QLabel("Выберите размер поля (N > 3):");
        layout->addWidget(label);

        QSpinBox *spinBox = new QSpinBox();
        spinBox->setRange(4, 10);
        spinBox->setValue(5);
        layout->addWidget(spinBox);

        QPushButton *startBtn = new QPushButton("Начать игру");
        layout->addWidget(startBtn);

        connect(startBtn, &QPushButton::clicked, [=]() {
            int size = spinBox->value();
            auto *game = new MainWindow(size);
            game->show();
            this->close();
        });

        setWindowTitle("Выбор размера поля");
        resize(300, 100);
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    StartWindow start;
    start.show();
    return app.exec();
}