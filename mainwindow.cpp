#include "mainwindow.h"
#include "ui_mainwindow.h"

#define lin_format [this](int i, int j) -> std::string\
    {\
        if(j == ui->eq_size->currentIndex() + 2)\
            return " = ";\
        else if(j > 0)\
            return "+ X" + std::to_string(j + 1) + " * ";\
        else\
            return "X" + std::to_string(j + 1) + " * ";\
    }\

#define mat_format [](int i, int j) -> std::string\
                    {\
                        return "("+ std::to_string(i + 1) + "," + std::to_string(j + 1) +")";\
                    }\

#define res_format [](int i, int j) -> std::string\
                   {\
                         return "X"+ std::to_string(i + 1) + " = ";\
                   }\

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    popup = new PopupWindow(this);

    mat_a = new Matrix<double>(-1, -1);
    mat_b = new Matrix<double>(-1, -1);
    mat_c = new Matrix<double>(-1, -1);

    equation = nullptr;

    layout = new QGridLayout();
    ui->mat_scroll_area->setLayout(layout);
    ui->mat_input_area->setWidget(ui->mat_scroll_area);

    eq_layout = new QGridLayout();
    ui->eq_scroll_area->setLayout(eq_layout);
    ui->eq_input_area->setWidget(ui->eq_scroll_area);

    ui->operation_btn->addItem("+");
    ui->operation_btn->addItem("-");
    ui->operation_btn->addItem("*");
    ui->operation_btn->addItem(".inv");
    ui->operation_btn->addItem(".tr");
    ui->operation_btn->addItem(".gs");
    ui->operation_btn->addItem(".det");

    ui->eq_size->addItem("2x2");
    ui->eq_size->addItem("3x3");
    ui->eq_size->addItem("4x4");
    ui->eq_size->addItem("5x5");
    ui->eq_size->addItem("6x6");
    ui->eq_size->addItem("7x7");

    ui->mat_c_btn->setEnabled(false);

    QObject::connect(ui->actionMatrix_operations, SIGNAL(triggered()), this, SLOT(set_mat_scene()));
    QObject::connect(ui->actionLinear_equations, SIGNAL(triggered()), this, SLOT(set_lin_scene()));
    QObject::connect(ui->operation_btn, SIGNAL(activated(int)), this, SLOT(operation_changed()));
    QObject::connect(ui->mat_configure_btn, SIGNAL(pressed()), this, SLOT(configure_grid()));
    QObject::connect(ui->mat_clr_btn, SIGNAL(pressed()), this, SLOT(clear_mat_input_scene()));
    QObject::connect(ui->mat_save_btn, SIGNAL(pressed()), this, SLOT(set_matrix()));
    QObject::connect(ui->calculate_btn, SIGNAL(pressed()), this, SLOT(calculate()));
    QObject::connect(ui->mat_a_btn, SIGNAL(pressed()), this, SLOT(set_a_scene()));
    QObject::connect(ui->mat_b_btn, SIGNAL(pressed()), this, SLOT(set_b_scene()));
    QObject::connect(ui->mat_c_btn, SIGNAL(pressed()), this, SLOT(set_c_scene()));
    QObject::connect(ui->eq_size, SIGNAL(activated(int)), this, SLOT(eq_size_changed()));
    QObject::connect(ui->mat_from_f_btn, SIGNAL(pressed()), this, SLOT(from_file()));
    QObject::connect(ui->eq_solve, SIGNAL(pressed()), this, SLOT(solve()));
    QObject::connect(ui->matrices_btn, SIGNAL(pressed()), this, SLOT(set_mat_scene()));
    QObject::connect(ui->lin_eq_btn, SIGNAL(pressed()), this, SLOT(set_lin_scene()));
    QObject::connect(ui->mat_back_btn, SIGNAL(pressed()), this, SLOT(set_mat_scene()));
    QObject::connect(ui->eq_clr_btn, SIGNAL(pressed()), this, SLOT(clear_eq()));

    font = new QFont();
    font->setPointSize(16);

    width = -1;
    height = -1;

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_mat_scene()
{
    ui->scenes->setCurrentIndex(0);
    ui->mat_configure_btn->setEnabled(true);
    ui->mat_save_btn->setEnabled(true);
    ui->mat_from_f_btn->setEnabled(true);
    ui->mat_clr_btn->setEnabled(true);
    ui->mat_width->setEnabled(true);
    ui->mat_height->setEnabled(true);
    setFixedSize(681, 170);
}

void MainWindow::set_lin_scene()
{
    ui->scenes->setCurrentIndex(1);
    setFixedSize(681, 365);
    generate_table(ui->eq_size->currentIndex() + 3, ui->eq_size->currentIndex() + 2, eq_layout, eq_input,
                   lin_format);
}

void MainWindow::set_a_scene()
{
    ui->mat_c_btn->setEnabled(false);
    current_matrix = 1;
    ui->scenes->setCurrentIndex(2);

    init_scene(mat_a);
}

void MainWindow::set_b_scene()
{
    ui->mat_c_btn->setEnabled(false);
    current_matrix = 2;
    ui->scenes->setCurrentIndex(2);

    init_scene(mat_b);
}

void MainWindow::set_c_scene()
{
    current_matrix = 3;
    ui->scenes->setCurrentIndex(2);

    ui->mat_configure_btn->setEnabled(false);
    ui->mat_save_btn->setEnabled(false);
    ui->mat_from_f_btn->setEnabled(false);
    ui->mat_clr_btn->setEnabled(false);
    ui->mat_width->setEnabled(false);
    ui->mat_height->setEnabled(false);

    init_scene(mat_c);
}

void MainWindow::configure_grid()
{
    for(int i = 0; i < mat_input.size(); i++)
        delete mat_input[i];
    mat_input.clear();
    clear_widgets(layout);

    width = ui->mat_width->text().toInt() > 20 ? 20 : ui->mat_width->text().toInt();
    height = ui->mat_height->text().toInt() > 20 ? 20 : ui->mat_height->text().toInt();
    ui->mat_width->setText(QString::fromUtf8(std::to_string(width).c_str()));
    ui->mat_height->setText(QString::fromUtf8(std::to_string(height).c_str()));

    generate_table(width, height, layout, mat_input,
                   mat_format);
}

void MainWindow::clear_widgets(QLayout* layout)
{
    if (!layout)
       return;
    while (auto item = layout->takeAt(0))
    {
       delete item->widget();
       clear_widgets(item->layout());
    }
 }

void MainWindow::clear_mat_input_scene()
{
    for(int i = 0; i < mat_input.size(); i++)
        delete mat_input[i];
    mat_input.clear();
    clear_widgets(layout);

    ui->mat_width->setText("");
    ui->mat_height->setText("");

    width = -1;
    height = -1;
}

void MainWindow::set_matrix()
{
    if(width <= 0 || height <= 0)
    {
        show_popup("Invalid data");
        return;
    }

    double** array = new double*[height];

    for(int i = 0; i < height; i++)
    {
        array[i] = new double[width];
        for(int j = 0; j < width; j++)
            array[i][j] = mat_input[i * width + j]->text().toDouble();
    }
    switch(current_matrix)
    {
        case 1:
            mat_a->set(height, width, array);
            break;
        case 2:
            mat_b->set(height, width, array);
            break;
    }

    ui->menuBar->setEnabled(true);
    ui->scenes->setCurrentIndex(0);
    setFixedSize(681, 170);
}

void MainWindow::enable()
{
    setEnabled(true);
    popup->hide();
}

void MainWindow::generate_table(int width,
                                int height,
                                QGridLayout* layout,
                                std::vector<QLineEdit*>& edits,
                                std::function<std::string(int i, int j)> format,
                                Matrix<double>* matrix)
{
    QLabel* _temp;
    std::string str;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            edits.push_back(new QLineEdit());
            edits[i * width + j]->setFixedWidth(80);
            edits[i * width + j]->setFixedHeight(30);
            edits[i * width + j]->setFont(*font);

            str = format(i, j);
            _temp = new QLabel();
            _temp->setText(QString::fromUtf8(str.c_str()));
            _temp->setFixedSize(_temp->text().size() * 10, 30);
            _temp->setFont(*font);

            layout->addWidget(_temp, i, j * 2);
            layout->addWidget(edits[i * width + j], i, j * 2 + 1);

            if(matrix)
            {
                str = std::to_string(matrix->get_data()[i][j]);
                str.erase(str.end() - 4, str.end());
                edits[i * width + j]->setText(QString::fromUtf8(str.c_str()));
            }
        }
    }
}

void MainWindow::init_scene(Matrix<double>* matrix)
{
    setFixedSize(681, 365);
    clear_mat_input_scene();
    if(matrix->get_width() > 0 && matrix->get_height() > 0)
    {
        ui->mat_width->setText(QString::fromUtf8(std::to_string(matrix->get_width()).c_str()));
        ui->mat_height->setText(QString::fromUtf8(std::to_string(matrix->get_height()).c_str()));

        width = matrix->get_width();
        height = matrix->get_height();
        generate_table(width, height, layout, mat_input,
                       mat_format, matrix);
    }
}

void MainWindow::calculate()
{
    if(mat_a->get_width() == -1 || mat_a->get_height() == -1)
    {
        show_popup("Matrix A was not initialised");
        return;
    }
    switch(ui->operation_btn->currentIndex())
    {
        case 0:
            if(mat_b->get_width() == -1 || mat_b->get_height() == -1)
            {
                show_popup("Matrix B was not initialised");
                return;
            }
            if(mat_a->get_width() != mat_b->get_width() ||
               mat_a->get_height() != mat_b->get_height())
            {
                show_popup("Dimensions should agree");
                return;
            }

            ui->mat_c_btn->setEnabled(true);
            *mat_c = *mat_a + *mat_b;
            break;

        case 1:
            if(mat_b->get_width() == -1 || mat_b->get_height() == -1)
            {
                show_popup("Matrix B was not initialised");
                return;
            }
            if(mat_a->get_width() != mat_b->get_width() ||
               mat_a->get_height() != mat_b->get_height())
            {
                show_popup("Dimensions should agree");
                return;
            }
            ui->mat_c_btn->setEnabled(true);
            *mat_c = *mat_a - *mat_b;
            break;
        case 2:
            if(mat_b->get_width() == -1 || mat_b->get_height() == -1)
            {
                show_popup("Matrix B was not initialised");
                return;
            }
            if(mat_a->get_width() != mat_b->get_height())
            {
                show_popup("Dimensions should agree");
                return;
            }
            ui->mat_c_btn->setEnabled(true);
            *mat_c = *mat_a * *mat_b;
            break;
        case 3:
            if(mat_a->get_width() != mat_a->get_height())
            {
                show_popup("Dimensions should agree");
                return;
            }
            if(mat_a->determinant() == 0)
            {
                show_popup("Determinant is zero");
                return;
            }
            ui->mat_c_btn->setEnabled(true);
            *mat_c = mat_a->inverse();
            break;
        case 4:
            ui->mat_c_btn->setEnabled(true);
            *mat_c = mat_a->transpose();
            break;
        case 5:
            mat_a->gauss();
            mat_a->reverse_gauss();
            ui->mat_c_btn->setEnabled(true);
            *mat_c = *mat_a;
            break;
        case 6:
            if(mat_a->get_width() != mat_a->get_height())
            {
                show_popup("Dimensions should agree");
                return;
            }
            std::string temp = "Determinant is:" + std::to_string(mat_a->determinant());
            temp.erase(temp.end() - 4, temp.end());
            show_popup(temp);
            break;
    }
}

void MainWindow::show_popup(std::string message)
{
    setEnabled(false);
    popup->set_text(message);
    popup->show();
}

void MainWindow::operation_changed()
{
    ui->mat_b_btn->setEnabled(true);
    ui->mat_c_btn->setEnabled(true);
    if(ui->operation_btn->currentIndex() > 2)
       ui->mat_b_btn->setEnabled(false);
    if(ui->operation_btn->currentIndex() == 6)
        ui->mat_c_btn->setEnabled(false);
    ui->mat_c_btn->setEnabled(false);
}

void MainWindow::from_file()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open File");
    *(current_matrix == 1 ? mat_a : mat_b) = Matrix<double>::read_from_file(file_name.toStdString());
    init_scene(current_matrix == 1 ? mat_a : mat_b);
}

void MainWindow::eq_size_changed()
{
    for(int i = 0; i < eq_input.size(); i++)
        delete eq_input[i];
    eq_input.clear();
    clear_widgets(eq_layout);
    generate_table(ui->eq_size->currentIndex() + 3, ui->eq_size->currentIndex() + 2, eq_layout, eq_input,
                   lin_format);
    ui->eq_solve->setEnabled(true);
}

void MainWindow::solve()
{
    eq_height = ui->eq_size->currentIndex() + 2;
    eq_width = ui->eq_size->currentIndex() + 3;

    double** array = new double*[eq_height];

    for(int i = 0; i < eq_height; i++)
    {
        array[i] = new double[eq_width];
        for(int j = 0; j < eq_width; j++)
            array[i][j] = eq_input[i * eq_width + j]->text().toDouble();
    }

    Matrix<double> temp(eq_height, eq_width, array);

    for(int i = 0; i < height; i++)
        delete[] array[i];
    delete[] array;

    if(equation)
        delete equation;

    equation = new Equation(temp);

    std::vector<double> result = equation->solve();

    double** temp_arr = new double*[result.size()];
    for(int i = 0; i < result.size(); i++)
        temp_arr[i] = new double(result[i]);

    Matrix<double> temp_m(result.size(), 1, temp_arr);

    for(int i = 0; i < result.size(); i++)
        delete temp_arr[i];
    delete[] temp_arr;

    if(result.size() != 0)
    {
        clear_widgets(eq_layout);

        for(int i = 0; i < mat_input.size(); i++)
            delete eq_input[i];
        eq_input.clear();


        generate_table(1, result.size(), eq_layout, eq_input,
                       res_format, &temp_m);
        ui->eq_solve->setEnabled(false);

    }

    if(result.size() == 0)
        show_popup("Can't solve.");
}

void MainWindow::clear_eq()
{
     eq_size_changed();
     ui->eq_solve->setEnabled(true);
}
