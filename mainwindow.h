#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QFont>
#include <string>
#include <functional>
#include <vector>

#include "matrix.h"
#include "equation.h"
#include "popupwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void set_mat_scene();
    void set_lin_scene();
    void set_a_scene();
    void set_b_scene();
    void set_c_scene();
    void configure_grid();
    void clear_mat_input_scene();
    void set_matrix();
    void enable();
    void calculate();
    void operation_changed();
    void from_file();
    void eq_size_changed();
    void solve();
    void clear_eq();

private:
    Ui::MainWindow *ui;

    QGridLayout* layout;
    QGridLayout* eq_layout;
    QFont* font;

    std::vector<QLineEdit*> mat_input;
    std::vector<QLineEdit*> eq_input;

    PopupWindow* popup;

    int current_matrix;

    int width;
    int height;

    int eq_width;
    int eq_height;

    void clear_widgets(QLayout* layout);

    Matrix<double>* mat_a;
    Matrix<double>* mat_b;
    Matrix<double>* mat_c;

    Equation* equation;

    void generate_table(int width,
                        int height,
                        QGridLayout* layout,
                        std::vector<QLineEdit*>& edits,
                        std::function<std::string(int, int)> format,
                        Matrix<double>* matrix = nullptr);
    void init_scene(Matrix<double>* matrix);
    void init_eq_scene();
    void show_popup(std::string message);
};

#endif // MAINWINDOW_H
