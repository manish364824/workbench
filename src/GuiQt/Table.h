#ifndef TABLE_H
#define TABLE_H
#include <QColor>
#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class Table;
}

class Table : public QWidget
{
    Q_OBJECT
    
public:
    explicit Table(QWidget *parent = 0);
    ~Table();
    void createModel();
    void populate(int xSize, int ySize);
    void populate(std::vector< std::vector <QColor>> &colors);
    void createColors(int xSize, int ySize, std::vector< std::vector <QColor>> &colors);

private:
    Ui::Table *ui;
    QStandardItemModel *model;
};

#endif // TABLE_H
