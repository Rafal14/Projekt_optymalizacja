#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "simplextab.h"
#include <QMainWindow>

#include <QString>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>



#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <iostream>




using namespace std;


/*!
 * \file mainwindow.h
 *
 *  Plik zawiera definicję klasy realizującej interfejs graficzny
 *  programu do rozwiązywanie zadania programowania liniowego
 *  całkowitoliczbowego metodą odcięć Gomory'ego.
 *
 */


/*!
 * \brief Moduł umożliwający dostęp do obiektów interfejsu użytkownika ui
 *
 *  Przestrzeń nazwy graficznego interfejsu użytkownika.
 */
namespace Ui {
class MainWindow;
}

/*!
 * \brief Klasa okna głównego
 *
 *  Modeluje pojęcie głównego okna graficznego przy użyciu bilioteki graficznej Qt
 *
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * Konstruktor okna MainWindow
     *
     *
     */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    int on_computeBtn_clicked();

    void on_clearBtn_clicked();

    void setVariablesNo(int varNo);

    void setConstrains(int consNo);

    void displayTab();

    void on_drawBtn_clicked();

private:
    Ui::MainWindow *ui;

    SimplexTab simpTab;  //tabla simpleks

    int numVariables; //liczba zmiennych ustwiona w oknie

    int numConstrains; //liczba ograniczeń ustawiona w oknie

    //orginalna tablica
    vector< vector<double> > orginalTableau;

    vector<double> manyVect;

    QwtPlot *plot;
};

#endif // MAINWINDOW_H
