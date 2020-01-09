#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <QDebug>

template<class T>
class Matrix
{
private:
    int height;
    int width;

    T** data;
    void swap_rows(int row_1, int row_2);
    int find(int column, int start, int end);

public:

    Matrix() {}
    Matrix(int h, int w, T** data);
    Matrix(int h, int w);
    Matrix(const Matrix& other_matrix);

    ~Matrix();

    void input_matrix();

    double determinant();

    void random_init(int min, int max);
    void print();
    void reverse_gauss();
    inline void set(int h, int w, T** data);
    inline void resize(int h, int w);

    inline T** get_data() const;

    inline int	get_width() const;
    inline int	get_height() const;
    int gauss();

    Matrix<T>& operator+ (const Matrix<T>& other_matrix) const;
    Matrix<T>& operator- (const Matrix<T>& other_matrix) const;
    Matrix<T>& operator* (const Matrix<T>& other_matrix) const;
    Matrix<T>& operator* (const T&  			 number) const;
    friend Matrix<T>& operator* (const T& number, const Matrix<T>& matrix) { return matrix * number; }
    Matrix<T>& operator= (const Matrix<T>& other_matrix);

    Matrix<T> inverse() const;
    Matrix<T> transpose() const;

    static Matrix<T> read_from_file(std::string file_name);
};

template <class T>
Matrix<T>::Matrix(int h, int w, T** other_data)
{
    this->height = h;
    this->width = w;

    data = new T*[this->height];
    for (int i = 0; i < this->height; i++)
        data[i] = new T[this->width];

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            this->data[i][j] = other_data[i][j];
}

template <class T>
Matrix<T>::Matrix(int h, int w)
{
    this->height = h;
    this->width = w;

    if(h <= 0 || w <= 0)
        return;

    data = new T*[this->height];
    for (int i = 0; i < this->height; i++)
        data[i] = new T[this->width];
}

template <class T>
Matrix<T>::Matrix(const Matrix& other_matrix)
{
    width = other_matrix.get_width();
    height = other_matrix.get_height();

    data = new T*[height];
    for (int i = 0; i < height; i++)
    {
        data[i] = new T[width];
        for (int j = 0; j < width; j++)
            data[i][j] = other_matrix.get_data()[i][j];
    }
}

template <class T>
Matrix<T>::~Matrix()
{
    for (int i = 0; i < height; i++)
        delete[] data[i];
    delete[] data;
}

template<class T>
Matrix<T>& Matrix<T>::operator+ (const Matrix<T>& other_matrix) const
{
    Matrix<T>* m = new Matrix<T>(this->height, this->width);

    for (int i = 0; i < m->height; i++)
        for (int j = 0; j < m->width; j++)
            m->data[i][j] = this->data[i][j] + other_matrix.get_data()[i][j];

    return *m;
}

template<class T>
Matrix<T>& Matrix<T>::operator- (const Matrix<T>& other_matrix) const
{
    Matrix<T>* m = new Matrix<T>(this->height, this->width);

    for (int i = 0; i < m->height; i++)
        for (int j = 0; j < m->width; j++)
            m->data[i][j] = this->data[i][j] - other_matrix.get_data()[i][j];

    return *m;
}

template<class T>
Matrix<T>& Matrix<T>::operator* (const Matrix<T>& other_matrix) const
{
    Matrix<T>*m = new Matrix<T>(this->height, other_matrix.width);

    for (int i = 0; i < m->height; i++)
        for (int j = 0; j < m->width; j++)
        {
            T sum = 0;

            for (int k = 0; k < this->width; k++)
                sum += this->data[i][k] * other_matrix.get_data()[k][j];

            m->data[i][j] = sum;
        }

    return *m;
}

template<class T>
Matrix<T>& Matrix<T>::operator* (const T& number) const
{
    Matrix<T> *temp = new Matrix<T>(*this);

    for (int i = 0; i < this->height; i++)
        for (int j = 0; j < this->width; j++)
            temp->get_data()[i][j] *= number;

    return *temp;
}

template <class T>
Matrix<T>& Matrix<T>::operator= (const Matrix<T>& other_matrix)
{
    if(width > 0 && height > 0)
    {
        for (int i = 0; i < height; i++)
            delete[] data[i];
        delete[] data;
    }

    width = other_matrix.get_width();
    height = other_matrix.get_height();

    data = new T*[height];
    for (int i = 0; i < height; i++)
    {
        data[i] = new T[width];
        for (int j = 0; j < width; j++)
            data[i][j] = other_matrix.get_data()[i][j];
    }

    return *this;
}

template <class T>
void Matrix<T>::input_matrix()
{
    for (int i = 0; i < this->height; i++)
        for (int j = 0; j < this->width; j++)
            std::cin >> this->data[i][j];
}

template <class T>
int Matrix<T>::gauss()
{
    int sign = 1;
    for (int i = 0; i < this->height; i++)
    {
        int temp = i;

        for (; temp < this->width && this->data[i][temp] == 0; temp++)
        {
            int index = find(temp, i, this->height);

            if (index > 0)
            {
                swap_rows(i, index);
                sign *= -1;
                break;
            }
        }

        if (temp == width)
            return sign;

        for (int k = i + 1; k < this->height; k++)
        {
            if (this->data[k][temp] == 0)
                continue;
            double coef = -(double)(this->data[k][temp]) / (this->data[i][temp]);

            for (int j = temp; j < this->width; j++)
                this->data[k][j] += coef * this->data[i][j];
        }
    }
    return sign;
}

template <class T>
void Matrix<T>::reverse_gauss()
{
    if (determinant() == 0)
    {
        std::cout << "determinant is equals to 0" << std::endl;
        return;
    }

    for (int i = 0; i < height; i++)
    {
        double coef = data[i][i];
        if(coef == 0)
            continue;
        for (int j = i; j < width; j++)
            data[i][j] /= coef;
    }

    for (int i = height - 1; i > 0; i--)
    {
        for (int temp = i - 1; temp >= 0; temp--)
        {
            double coef = data[temp][i];
            for (int j = i; j < width; j++)
                data[temp][j] -= data[i][j] * coef;
        }
    }
}

template<class T>
Matrix<T> Matrix<T>::transpose() const
{
    Matrix<T> temp = Matrix(this->width, this->height);

    for (int i = 0; i < this->width; i++)
        for (int j = 0; j < this->height; j++)
            temp.get_data()[i][j] = this->data[j][i];

    return temp;
}

template<class T>
Matrix<T> Matrix<T>::inverse() const
{
    T** data = new T*[height];

    for (int i = 0; i < this->height; i++)
        data[i] = new T[2 * this->width];

    for (int i = 0; i < this->height; i++)
        for (int j = 0; j < 2 * this->width; j++)
        {
            if (j < this->width)
                data[i][j] = this->data[i][j];
            else if (i != j - this->width)
                data[i][j] = 0;
            else
                data[i][j] = 1;
        }

    Matrix<T> temp(this->height, this->width * 2, data);
    temp.gauss();
    temp.reverse_gauss();

    T** result_data = new T*[this->height];

    for (int i = 0; i < this->height; i++)
    {
        result_data[i] = new T[this->width];
        for (int j = 0; j < this->width; j++)
            result_data[i][j] = temp.get_data()[i][j + this->width];
    }
    return Matrix<T>(this->height, this->width, result_data);
}

template <class T>
double Matrix<T>::determinant()
{
    Matrix<T> temp(*this);
    int sign = temp.gauss();
    double determinant = 1;

    for (int i = 0; i < this->height; i++)
        determinant *= temp.get_data()[i][i];
    return sign * determinant;
}

template <class T>
int Matrix<T>::find(int column, int start, int end)
{
    for (int i = start; i < end; i++)
        if (this->data[i][column] != 0)
            return i;
    return -1;
}

template <class T>
void Matrix<T>::swap_rows(int row_1, int row_2)
{
    T temp;
    for (int i = 0; i < width; i++)
    {
        temp = this->data[row_1][i];
        this->data[row_1][i] = this->data[row_2][i];
        this->data[row_2][i] = temp;
    }
}

template <class T>
void Matrix<T>::random_init(int min, int max)
{
    for (int i = 0; i < this->height; i++)
        for (int j = 0; j < this->width; j++)
            this->data[i][j] = min + rand() % (max - min + 1);
}

template <class T>
void Matrix<T>::print()
{
    qDebug() << "width " << width;
    qDebug() << "height " << height;

    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
            qDebug() << this->data[i][j] << "\t\t";
    }
}

template <class T>
inline void Matrix<T>::set(int h, int w, T** data)
{
    if(height > 0 && width > 0)
    {
        for (int i = 0; i < height; i++)
            delete[] this->data[i];
        delete[] this->data;
    }

    width = w;
    height = h;

    this->data = data;
}

template<class T>
inline void Matrix<T>::resize(int h, int w)
{
    for (int i = 0; i < height; i++)
        delete[] data[i];
    delete[] data;

    this->width = w;
    this->height = h;

    data = new T*[height];
    for (int i = 0; i < height; i++)
        data[i] = new T[width];
}

template <class T>
T** Matrix<T>::get_data() const
{
    return data;
}

template <class T>
int Matrix<T>::get_width() const
{
    return width;
}

template <class T>
int Matrix<T>::get_height() const
{
    return height;
}

template<class T>
Matrix<T> Matrix<T>::read_from_file(std::string file_name)
{
    std::ifstream fin;
    fin.open(file_name.c_str());

    if (!fin.is_open())
    {
        return Matrix<double>(-1, -1, nullptr);
    }

    int count = 0;
    int temp;

    while (!fin.eof())
    {
        fin >> temp;
        count++;
    }

    fin.seekg(0, std::ios::beg);
    fin.clear();

    int count_space = 0;
    char symbol;
    while (!fin.eof())
    {
        fin.get(symbol);
        if (symbol == ' ') count_space++;
        if (symbol == '\n') break;
    }

    fin.seekg(0, std::ios::beg);
    fin.clear();

    int width = count_space + 1;
    int height = count / width;

    double** data;

    data = new double*[height];

    for (int i = 0; i < height; i++)
        data[i] = new double[width];

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            fin >> data[i][j];

    fin.close();

    Matrix<double> result(height, width, data);

    for (int i = 0; i < height; i++)
        delete[] data[i];
    delete[] data;

    return result;
}
