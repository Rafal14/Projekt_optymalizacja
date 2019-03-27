#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rysunek.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    plot = nullptr;
    ui->drawBtn->setDisabled(true);

    //wstępne wpisanie liczny zmiennych i ograniczeń
    numVariables  = 2;
    numConstrains = 3;

    ui->spinBoxCostr->setValue(numConstrains);
    ui->spinBoxVars->setValue(numVariables);

    //ustaw wstępną liczbę kolumn i wierszy
    ui->tableWidget->setColumnCount(numVariables+3);
    ui->tableWidget->setRowCount(numConstrains+1);

    connect(ui->spinBoxVars, SIGNAL(valueChanged(int)), this, SLOT(setVariablesNo(int)));
    connect(ui->spinBoxCostr, SIGNAL(valueChanged(int)), this, SLOT(setConstrains(int)));




    for (int l=0; l < numConstrains+1; ++l) {
        for (int k=0; k < numVariables+3; ++k) {
           // QTableWidgetItem *nItem = new QTableWidgetItem();
            ui->tableWidget->setItem(l, k, new QTableWidgetItem());
           // ui->tableWidget->rowCountChanged();
        }
    }
    ui->tableWidget->item(0,0)->setText("max x0");

    QString str;
    for (int i=1; i < numConstrains+1; ++i) {
        str.sprintf("x%d", numVariables+i);
        ui->tableWidget->item(i,0)->setText(str);
    }

    ui->tableWidget->item(0,1)->setText("1");
    ui->tableWidget->item(0,2)->setText("6");

    ui->tableWidget->item(1,1)->setText("-2");
    ui->tableWidget->item(1,2)->setText("-1");
    ui->tableWidget->item(1,4)->setText("-2");

    ui->tableWidget->item(2,1)->setText("-1");
    ui->tableWidget->item(2,2)->setText("1");
    ui->tableWidget->item(2,4)->setText("3");

    ui->tableWidget->item(3,1)->setText("1");
    ui->tableWidget->item(3,2)->setText("1");
    ui->tableWidget->item(3,4)->setText("6");

    ui->tableWidget->item(1,3)->setText("<=");
    ui->tableWidget->item(2,3)->setText("<=");
    ui->tableWidget->item(3,3)->setText("<=");
}

MainWindow::~MainWindow()
{
    //usuniecie zaalokowanej tablicy simplex


    delete ui;
}

int MainWindow::on_computeBtn_clicked()
{

    ui->drawBtn->setDisabled(false);

    QString           str;

    int typeSolution = 0;            //określa typ rozwiązania

    int vars  = numVariables;
    int contr = numConstrains;

    int *coeffC = new int [vars];   // tabela wsp funkcji celu
    int *coeffB = new int [contr];  // tabela wyrow wolnych dla ogranicznen

    int **tabN = new int *[contr]; // tabela wspolczynnikow przy ograniczniach
    for (int m=0; m < contr; ++m) {
        tabN[m] = new int [vars];
    }



    for (int i=0; i < contr+1; ++i) {
        for (int j=1; j < vars+3; ++j) {
            str = ui->tableWidget->item(i,j)->text();

            if (i==0) {
                if (j < vars+1) {
                    coeffC[j-1] = str.toInt();
                }
            }

            if (i > 0) {
                if (j == vars+2) {
                    coeffB[i-1] = str.toInt();
                }
                else {
                    tabN[i-1][j-1] = str.toInt();
                }
            }
        }
    }


    simpTab.AddValues(vars, contr, coeffC, coeffB, tabN);


    //zapisz tablicę sympleks
    double **sTab = simpTab.getSimplexTableau();

    for (int u=0; u < contr+1; ++u) {
        vector<double> orgVect;
        for (int v=0; v < vars+1; ++v) {
            orgVect.push_back( sTab[u][v] );
        }
        orginalTableau.push_back(orgVect);
    }

    //usun tablice pop
    delete [] coeffB;
    delete [] coeffC;

    for (int v=0; v < contr; ++v) {
        delete [] tabN[v];
    }
    delete [] tabN;
    tabN = nullptr;



    //wyświetl początkową tablicę sympleks
    ui->textEdit->insertPlainText("Początkowa postać tablicy sympleks\n");

    //wyświetl tablicę sympleks
    displayTab();

    //stosuj metodę dwufazową do uproszczenia tablicy sympleksowej
    for (int sol=0; (!simpTab.isOptimalSolution()) && (sol!=NULL_SET);)
        sol = simpTab.twoPhaseSimplex();

    //sprawdz typ rozwiązania
    typeSolution = simpTab.checkTypeSolution();

    //sortuj po wierszach i kolumnach
    simpTab.sortRows();
    simpTab.sortCols();

    //wyświetl tablicę sympleksową po zastosowaniu
    //metody dwufazowej
    ui->textEdit->insertPlainText("\n\nTablica po zastosowaniu metody dwufazowej\n");
    displayTab();

    //warunek dla nieskończonej liczby rozwiązań na zbiorze ograniczonym
    if (typeSolution == MANY_SOLUTION_CL_TASK) {
        bool   flag=false;

        vector<double> variables;
        double         elem;

        int  cNum;
        bool isExecuted = false;

        int     *oldColIndTab   = simpTab.getColsIndTab();
        int     *oldRowIndTab   = simpTab.getRowsIndTab();
        double **oldTableau     = simpTab.getSimplexTableau();


        int    *colIndTab = new int [vars];
        int    *rowIndTab = new int [contr+1];
        double **nTableau = new double *[contr+1];


        for (int i=0; i<contr+1; ++i) {
            rowIndTab[i] = oldRowIndTab[i];
            nTableau[i] = new double [vars+1];
            for (int j=0; j < vars+1; ++j) {
                double element = oldTableau[i][j];
                nTableau[i][j] = element;
                if (j>0)
                    colIndTab[j-1] = oldColIndTab[j-1];
            }
        }


        for (int q=1; q < vars+1; ++q) {
            elem = simpTab.findElem(q);
            variables.push_back(elem);
        }
        for (int p=0; (p<6) && (flag==false) ; ++p) {
            simpTab.solveZeroSolution();
            ui->textEdit->insertPlainText("\n\nKolejne rozwiązanie\n");
            displayTab();

            //
            //liczba odcięć
            cNum = simpTab.getCuttingPlainNumber();

            //dopóki nie ma rozwiązania całkowitoliczbowego
            for (;!simpTab.isIntegralSolution(0.001) && cNum<6;) {
                simpTab.doGomoryMethod();

                ui->textEdit->insertPlainText("\n\nTablica z wybranym odcięciem\n");
                displayTab();

                // i odcięcie

                simpTab.dualSimplex();
                cNum = simpTab.getCuttingPlainNumber();

                ui->textEdit->insertPlainText("\n\nTablica po uproszczeniu\n");
                displayTab();
            }

            ui->textEdit->insertPlainText("\n\nZastosowana liczba odcięć:\t");
            ui->textEdit->insertPlainText(QString::number(cNum));
            ui->textEdit->insertPlainText("\n\n");

            manyVect.push_back( simpTab.findElem(1) );
            manyVect.push_back( simpTab.findElem(2) );

            simpTab.resizeSimp(vars, contr, rowIndTab, colIndTab, nTableau);

            for (int w=1; w < vars+1; ++w) {
                elem = simpTab.findElem(w);
                if ( variables[w-1] == elem ) {
                    flag = true;
                }
            }

            int plainNum=0;
            //dopóki nie ma rozwiązania całkowitoliczbowego
            for (;!simpTab.isIntegralSolution(0.001) && plainNum<6;) {
                simpTab.doGomoryMethod();

                ui->textEdit->insertPlainText("\n\nTablica z wybranym odcięciem\n");
                displayTab();

                // i odcięcie

                simpTab.dualSimplex();
                plainNum = simpTab.getCuttingPlainNumber();

                ui->textEdit->insertPlainText("\n\nTablica po uproszczeniu\n");
                displayTab();
            }

            ui->textEdit->insertPlainText("\n\nZastosowana liczba odcięć:\t");
            ui->textEdit->insertPlainText(QString::number(plainNum));
            ui->textEdit->insertPlainText("\n\n");


        }

        delete []colIndTab;
        delete []rowIndTab;
        for (int v=0; v < contr; ++v) {
            delete [] nTableau[v];
        }
        delete [] nTableau;



        return MANY_SOLUTION_CL_TASK;
    }

    if (typeSolution == NULL_SET) {
        ui->textEdit->insertPlainText("\n\nZadanie nieograniczone - zbiór rozwiązań");
        ui->textEdit->insertPlainText("\njest pusty\n\n");
    }


    //określenie typu rozwiązania
    if (typeSolution == MANY_SOLUTION_OP_TASK) {
        ui->textEdit->insertPlainText("\n\nIstnieje nieskończona liczba rozwiązań");
        ui->textEdit->insertPlainText(" na zbiorze nieograniczonym\n");
    }
    if (typeSolution == MANY_SOLUTION_CL_TASK) {
        ui->textEdit->insertPlainText("\n\nIstnieje nieskończona liczba rozwiązań");
        ui->textEdit->insertPlainText(" na zbiorze ograniczonym\n");
    }


    //liczba odcięć
    int cNumber = simpTab.getCuttingPlainNumber();

    //dopóki nie ma rozwiązania całkowitoliczbowego
    for (;!simpTab.isIntegralSolution(0.001) && cNumber<6;) {
        simpTab.doGomoryMethod();

        ui->textEdit->insertPlainText("\n\nTablica z wybranym odcięciem\n");
        displayTab();

        // i odcięcie

        simpTab.dualSimplex();
        cNumber = simpTab.getCuttingPlainNumber();

        ui->textEdit->insertPlainText("\n\nTablica po uproszczeniu\n");
        displayTab();
    }

    ui->textEdit->insertPlainText("\n\nZastosowana liczba odcięć:\t");
    ui->textEdit->insertPlainText(QString::number(cNumber));
    ui->textEdit->insertPlainText("\n\n");

/*
    //test - wypisz wektor odcięć
    ui->textEdit->insertPlainText("\n\n\nOdcięcia\n");
    vector< vector<double> > plVect = simpTab.getCuttingPlainVector();
    for (int p=0; (p<cNumber) && !plVect.empty(); ++p) {
        for (int q=0; q < vars+1; ++q) {
            ui->textEdit->insertPlainText("\t");
            ui->textEdit->insertPlainText(QString::number(plVect[p][q]));
        }
        ui->textEdit->insertPlainText("\n");
    }

    //test wypisz orginala tablice sympleks - wektor wektorów
    ui->textEdit->insertPlainText("\n\n\nOrginalna tab\n");
    for (int k=0; k < contr+1; ++k) {
        for (int m=0; m < vars+1; ++m) {
            ui->textEdit->insertPlainText("\t");
            ui->textEdit->insertPlainText(QString::number(orginalTableau[k][m]));
        }
        ui->textEdit->insertPlainText("\n");
    }
*/
    sTab = nullptr;

    return 0;
}

void MainWindow::on_clearBtn_clicked()
{
    ui->drawBtn->setDisabled(true);

    ui->textEdit->clear();
    if (plot != nullptr) {

        QLayoutItem* child;
        child=ui->verticalLayout->takeAt(0);
        while(child != 0)
        {
            if(child->widget()!=0)
                child->widget()->hide();
            ui->verticalLayout->removeWidget(child->widget());
            delete child;
            child=ui->verticalLayout->takeAt(0);

        }
        delete plot;
        plot = nullptr;
    }

    if ( !simpTab.isEmpty() ) {
        //usuń tablicę sympleksową

        if ( !manyVect.empty() ) {
            manyVect.pop_back();
            manyVect.pop_back();
        }

        int constr = simpTab.getConstrainsNumber();
        for (int i=0; i < constr; ++i) {
            orginalTableau.pop_back();
        }

        simpTab.removeTab();

    }
}

void MainWindow::setVariablesNo(int varNo)
{
    //wyzerowanie dotychczasowych danych
    for (int l=0; l < numConstrains+1; ++l) {
        for (int k=1; k < numVariables+3; ++k) {
            if (ui->tableWidget->item(l, k) == 0) {
                ui->tableWidget->setItem(l, k, new QTableWidgetItem());
            }
            ui->tableWidget->item(l,k)->setText("");
        }
    }

    numVariables = varNo;
    ui->tableWidget->setColumnCount(varNo+3);

    for (int i=1; i < numConstrains+1; ++i) {
        if (ui->tableWidget->item(i, varNo+1) == 0) {
            ui->tableWidget->setItem(i, varNo+1, new QTableWidgetItem());
        }
        ui->tableWidget->item(i, varNo+1)->setText("<=");
    }
}

void MainWindow::setConstrains(int consNo)
{
    //stara zmianna
    int oldConstr = numConstrains;

    numConstrains = consNo;
    ui->tableWidget->setRowCount(consNo+1);

    for (int l=oldConstr+1; l < consNo+1; ++l) {
        for (int k=0; k <numVariables+3; ++k) {
            if (ui->tableWidget->item(l, k) == 0) {
                ui->tableWidget->setItem(l, k, new QTableWidgetItem());
            }
            if (k == numVariables+1)
                ui->tableWidget->item(l, k)->setText("<=");
        }
    }
}

void MainWindow::displayTab()
{
    //pod warunkiem, że istnieje tablica
    if (!simpTab.isEmpty()) {

        double **tab = simpTab.getSimplexTableau();

        int *colTab = simpTab.getColsIndTab();
        int *rowTab = simpTab.getRowsIndTab();

        int vars    = simpTab.getVariablesNumber();
        int contr   = simpTab.getConstrainsNumber();

        QString  st;
        QString numStr;

        ui->textEdit->insertPlainText("\t");

        for (int c=0; c < vars; ++c) {
            if ( (colTab[c] % 11) == 0 )
                st.sprintf("\ts%d", colTab[c]/11);
            else
                st.sprintf("\tx%d", colTab[c]);
            ui->textEdit->insertPlainText(st);
        }
        ui->textEdit->insertPlainText("\n");
        for (int i=0; i < contr+1; ++i) {

            if ((rowTab[i] % 11) == 0  && rowTab[i] != 0)
                st.sprintf("s%d", rowTab[i]/11);
            else
                st.sprintf("x%d", rowTab[i]);

            ui->textEdit->insertPlainText(st);

            for (int j=0; j < vars+1; ++j) {
                ui->textEdit->insertPlainText("\t");
                numStr.sprintf("%.3f", tab[i][j]);
                ui->textEdit->insertPlainText(numStr);

            }
            ui->textEdit->insertPlainText("\n");
        }


        colTab = nullptr;
        rowTab = nullptr;
        tab    = nullptr;
    }
}



//metoda do otwierania nowego okna z wygenerowanym rysunkiem w R^2 z narysowanymi odcięciami
void MainWindow::on_drawBtn_clicked()
{
    //rysunek rys1;
    //    rys1.setModal(true);
    //    rys1.exec();

    //Stworzenie tablicy odcięć:

    double **tab_odciec1 = new double *[3]; //Utworzenie kolumn

int g_odciecia = simpTab.getCuttingPlainNumber();
g_odciecia=0;
//Odciecia
if(g_odciecia>0)
{

    for(int i=0; i<3 ; i++)
    {
        tab_odciec1[i]= new double [g_odciecia];  //Utworzenie wierszy
    }


    vector< vector<double> > plVect1 = simpTab.getCuttingPlainVector();

    //cout<<endl<<"Odciecia1 : "<<plVect1[0][0]<< " "<<plVect1[0][1]<<" "<<plVect1[0][2]<<endl;
    //cout<<endl<<"Odciecia2 : "<<plVect1[1][0]<< " "<<plVect1[1][1]<<" "<<plVect1[1][2]<<endl;
    //Wstawienie współczynników równań odcięć do macierzy

    //tab_odciec1[kolumny][wiersze]            plVect1[wiersze][kolumny]
    for(int i=0; i<g_odciecia ; i++)
    {
        tab_odciec1[0][i] = plVect1[i][0]; //Wyraz wolny
        tab_odciec1[1][i] = plVect1[i][1]; //Współczynnik A1
        tab_odciec1[2][i] = plVect1[i][2]; //Współczynnik A2
    }

}

int mac_big = orginalTableau.size();
cout<<endl<<" Mac : "<<mac_big<<endl;

//orginalTableau[wiersze][kolumny]
//cout<<endl<<"ORGINAL TAB: "<<orginalTableau[0][0]<<" " << orginalTableau[0][1] <<" "<< orginalTableau[0][2] <<" JEST"<<endl;
//cout<<endl<<"ORGINAL TAB: "<<orginalTableau[1][0]<<" " << orginalTableau[1][1] <<" "<< orginalTableau[1][2] <<" JEST"<<endl;
//cout<<endl<<"ORGINAL TAB: "<<orginalTableau[2][0]<<" " << orginalTableau[2][1] <<" "<< orginalTableau[2][2] <<" JEST"<<endl;
//cout<<endl<<"ORGINAL TAB: "<<orginalTableau[3][0]<<" " << orginalTableau[3][1] <<" "<< orginalTableau[3][2] <<" JEST"<<endl;




double **tab_wynik = simpTab.getSimplexTableau(); //utworzenie tablicy z punktem wynikowym

//cout<<endl<<"Tab wynik : "<<tab_wynik[1][0]<<" "<<tab_wynik[2][0]<<endl;

//Obliczenie wyrazu wolnego dla funkcji celu:

double w_w_f_c= (((-1)*(orginalTableau[0][1]*tab_wynik[1][0]))+ ((-1)*(orginalTableau[0][2]*tab_wynik[2][0])));

//cout<<endl<<"WWFC : "<<w_w_f_c<<endl;

/* UWAGA - TEST    WIDŻETU  QWT DO RYSOWANIA FUNKCJI */

    int Rozmiar_macierz_ograniczen = (orginalTableau.size())-1; //Sprawdzenie ilości kolumn macierzy, przykładowo macierz[5][10], zwróci wartość 5

    cout<<endl<<"Rozmiar_macierz_ograniczen : "<<Rozmiar_macierz_ograniczen<<endl; //3

    /* Macierz znaków */
    /* A_1x_1 * A_2x_2 >= b_1 */
    /* (">=" -> 1) ("<=" -> 0) */
    int *znaki= new int[Rozmiar_macierz_ograniczen];

    for(int i=0; i< Rozmiar_macierz_ograniczen ; i++) //Wstawienie znaków
    {
        if(orginalTableau[i+1][0]<0) //większościowe
        {
            znaki[i]=1;
            cout<<endl<<"znaki["<<i<<"] = "<<znaki[i]<<endl;
        }

        if(orginalTableau[i+1][0]>=0) //mniejszosciowe
        {
            znaki[i]=0;
            cout<<endl<<"znaki["<<i<<"] = "<<znaki[i]<<endl;
        }
    }

    /* Utworzenie głównej macierzy do generowania wykresu */

    double **M = new double *[3]; //Utworzenie kolumn

    for(int i=0; i<3 ; i++)
    {
        M[i]= new double [Rozmiar_macierz_ograniczen];  //Utworzenie wierszy
    }

        for(int j=0 ; j<Rozmiar_macierz_ograniczen ; j++)
        {
        M[1][j]=orginalTableau[j+1][1];
        cout<<endl<<"1*: "<<orginalTableau[j+1][1];
        M[2][j]=orginalTableau[j+1][2];
        cout<<" 2*:  "<<orginalTableau[j+1][2]<<endl;
        }

    //for(int i=1 ; i<Rozmiar_macierz_glowna ; i++) //Wpisanie do macierzy wyrazów wolnych
    //    {
    //    M[0][i]=W_w_w_t[i-1];
    //    }

    for(int i=0 ; i<Rozmiar_macierz_ograniczen; i++) //Wpisanie do macierzy wyrazów wolnych
        {
        M[0][i]=orginalTableau[i+1][0];
        }

    /* Sprawdzenie czy podane macierze i wektory składają się z liczb całkowitoliczbowych z ustalonym błędem */


    /* Sprawdzenie wielkości macierzy lub wektora */
    /* Sprawdzenie ilości kolumn */
    //int ilosc_kolumn1 = (sizeof(M)/sizeof(*M));
    /* Sprawdzenie ilości wierszy */
    //int ilosc_wierszy1 = (sizeof(M[0])/sizeof(M[0][0]));

    /* Obliczenia na macierzy glownej i generowanie wykresu */

    //utworzenie obiektu typu wykres
    plot = new QwtPlot();

    //dodanie wykresu do okna Mainwindow
    ui->verticalLayout->addWidget(plot);

    plot->setTitle("Wykres warstwic");

    //ustawienie tła wykresu
    plot->setCanvasBackground(Qt::white);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach( plot );



    /* Obliczenia dla funkcji ograniczen*/

    /* Tworzenie macierzy punktów w których funkcje liniowe przecinaja osie */

    double **Punkty = new double *[8]; //Tworzenie kolumn

    for(int i=0; i<8 ; i++)
    {
        Punkty[i] = new double[Rozmiar_macierz_ograniczen]; //Tworzenie wierszy
    }

    /* Znajdowanie punktów przecinających się na osiach współrzędnych */

    // Pierwsze punkty
    for(int j=0; j<Rozmiar_macierz_ograniczen ; j++)
    {
        Punkty[2][j]=0; //x (x1)
        Punkty[3][j]=M[0][j]/M[2][j];   //y (x2)
    }

    //Drugie punkty
    for(int j=0; j<Rozmiar_macierz_ograniczen ; j++)
    {
        Punkty[4][j]=M[0][j]/M[1][j];   //x (x1)
        Punkty[5][j]=0; //y (x2)
    }

    /* Obliczenia dla odciec*/

    /* Tworzenie macierzy punktów w których funkcje liniowe przecinaja osie */

    double **Punkty_odciecia = new double *[8]; //Tworzenie kolumn

    if(g_odciecia>0)
    {

        for(int i=0; i<8 ; i++)
        {
            Punkty_odciecia[i] = new double[g_odciecia]; //Tworzenie wierszy
        }

        /* Znajdowanie punktów przecinających się na osiach współrzędnych */

        // Pierwsze punkty
        for(int j=0; j<g_odciecia ; j++)
        {
            Punkty_odciecia[2][j]=0; //x (x1)
            Punkty_odciecia[3][j]=tab_odciec1[0][j]/tab_odciec1[2][j];   //y (x2)
        }

        //Drugie punkty
        for(int j=0; j<g_odciecia ; j++)
        {
            Punkty_odciecia[4][j]=tab_odciec1[0][j]/tab_odciec1[1][j];   //x (x1)
            Punkty_odciecia[5][j]=0; //y (x2)
        }
    }



    /* Obliczenia dla FUNKCJI CELU */

    /* Tworzenie macierzy punktów w których funkcje liniowe przecinaja osie */

    double **Punkty_F_C = new double *[8]; //Tworzenie kolumn

    for(int i=0; i<8 ; i++)
    {
        Punkty_F_C[i] = new double[1]; //Tworzenie wierszy
    }

    /* Znajdowanie punktów przecinających się na osiach współrzędnych */

    // Pierwsze punkty
    for(int j=0; j<1 ; j++)
    {
        Punkty_F_C[2][j]=0; //x (x1)
        Punkty_F_C[3][j]=w_w_f_c/((-1)*orginalTableau[j][2]);   //y (x2)
    }

    //Drugie punkty
    for(int j=0; j<1 ; j++)
    {
        Punkty_F_C[4][j]=w_w_f_c/((-1)*orginalTableau[j][1]);   //x (x1)
        Punkty_F_C[5][j]=0; //y (x2)
    }

    /* Przedłużanie funkcji liniowych */

    /* Szukanie najdłuższego i najkrótszego punktu */

    int zm1=0; //Najdluższy punkt
    int zm2=0; //Najkrótszy punkt */

    for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x1
        for(int j=0 ; j<Rozmiar_macierz_ograniczen ; j++)
        {
            if((Punkty[(i*2)][j])>=zm1)
                zm1=Punkty[(i*2)][j];
        }

    for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x2
        for(int j=0 ; j<Rozmiar_macierz_ograniczen ; j++)
        {
            if((Punkty[((i*2)+1)][j])>=zm2)
                zm2=Punkty[((i*2)+1)][j];
        }

    /* Zwiększanie zasięgu rysowania funkcji na wykresie*/

    /* Szukanie najdłuższego i najkrótszego punktu */

    int zmg1=0; //Najdluższy punkt
    int zmg2=0; //Najkrótszy punkt */

    if(g_odciecia>0)
    {

        for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x1
            for(int j=0 ; j<g_odciecia ; j++)
            {
                if((Punkty_odciecia[(i*2)][j])>=zm1)
                    zm1=Punkty_odciecia[(i*2)][j];
            }

        for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x2
            for(int j=0 ; j<g_odciecia ; j++)
            {
                if((Punkty_odciecia[((i*2)+1)][j])>=zm2)
                    zm2=Punkty_odciecia[((i*2)+1)][j];
            }
    }

    //DLA FUNKCJI CELU
    int zmg1_F_C=0; //Najdluższy punkt
    int zmg2_F_C=0; //Najkrótszy punkt */

    for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x1
        for(int j=0 ; j<1 ; j++)
        {
            if((Punkty_F_C[(i*2)][j])>=zm1)
                zm1=Punkty_F_C[(i*2)][j];
        }

    for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x2
        for(int j=0 ; j<1 ; j++)
        {
            if((Punkty_F_C[((i*2)+1)][j])>=zm2)
                zm2=Punkty_F_C[((i*2)+1)][j];
        }


    /* Zwiększanie zasięgu rysowania funkcji na wykresie*/
    zmg1=zm1*1;
    zmg2=zm2*1;

    int Naj_p_x1=floor(zm1); //Największy punkt na osi x1
    //cout<<endl<<"NAJWIEKSZY PUNKT OS x1 : "<<Naj_p_x1<<endl;
    zm1=zm1*1;
    zm2=zm2*1;



    /* Uzupełnianie punktami granicznymi na początku i końcu funkcji*/

    /* Punkty początkowe */

    for(int j=0 ; j<Rozmiar_macierz_ograniczen ; j++)
    {
        Punkty[0][j]=((-1)*(zm1)); //x
        Punkty[1][j]=(((-1)*((M[1][j])*(((-1)*(zm1)))))+M[0][j])/(M[2][j]); //y
    }

    /* Punkty końcowe */

    for(int j=0 ; j<Rozmiar_macierz_ograniczen ; j++)
    {
        Punkty[6][j]=zm1; //x
        Punkty[7][j]=((((-1)*(M[1][j])*(zm1)))+(M[0][j]))/(M[2][j]); //y
    }




    /* Uzupełnianie punktami granicznymi na początku i końcu funkcji dla odciec*/

    /* Punkty początkowe dla odciec*/

    if(g_odciecia>0)
    {

        for(int j=0 ; j<g_odciecia ; j++)
        {
            Punkty_odciecia[0][j]=((-1)*(zmg1)); //x
            Punkty_odciecia[1][j]=(((-1)*((tab_odciec1[1][j])*(((-1)*(zmg1)))))+tab_odciec1[0][j])/(tab_odciec1[2][j]); //y
        }

        /* Punkty końcowe dla odciec*/

        for(int j=0 ; j<g_odciecia ; j++)
        {
            Punkty_odciecia[6][j]=zmg1; //x
            Punkty_odciecia[7][j]=((((-1)*(tab_odciec1[1][j])*(zmg1)))+(tab_odciec1[0][j]))/(tab_odciec1[2][j]); //y
        }
    }


    /* Uzupełnianie punktami granicznymi na początku i końcu FUNKCJI CELU*/

    /* Punkty początkowe dla FUNKCJI CELU*/


    for(int j=0 ; j<1 ; j++)
    {
        Punkty_F_C[0][j]=((-1)*(zmg1)); //x
        Punkty_F_C[1][j]=(((-1)*((((-1)*orginalTableau[j][1]))*(((-1)*(zmg1)))))+w_w_f_c)/(((-1)*orginalTableau[j][2])); //y
    }

    /* Punkty końcowe dla odciec*/

    for(int j=0 ; j<1 ; j++)
    {
        Punkty_F_C[6][j]=zmg1; //x
        Punkty_F_C[7][j]=((((-1)*(((-1)*orginalTableau[j][1]))*(zmg1)))+(w_w_f_c))/(((-1)*orginalTableau[j][2])); //y
    }





    /* Teren wspólny */

    /* Wyznaczanie punktów terenu wspólnego */


    //Rozmiar macierzy skladającej się z ograniczeń i odcięć:
    int Rozmiar_calosci = (Rozmiar_macierz_ograniczen + g_odciecia);

    //Połączenie macierzy ograniczeń z macierzą odcięć

    double **M_calosc = new double *[3]; //Utworzenie kolumn

    for(int i=0; i<3 ; i++)
    {
        M_calosc[i]= new double [(Rozmiar_macierz_ograniczen+g_odciecia)];  //Utworzenie wierszy
    }

    //UZUPEŁNIENIE MACIERZY OGRANICZENIAMI

    //M_calosc[kolumny][wiersze]     //M[kolumny][wiersze]
        for(int i=0; i<Rozmiar_macierz_ograniczen ; i++)
        {
        M_calosc[0][i]=M[0][i];
        M_calosc[1][i]=M[1][i];
        M_calosc[2][i]=M[2][i];
        }

    //UZUPEŁNIENIE MACIERZY ODCIĘCIAMI

        for(int i=0; i<g_odciecia ; i++)
        {
        M_calosc[0][(i+Rozmiar_macierz_ograniczen)]=tab_odciec1[0][i];
        M_calosc[1][(i+Rozmiar_macierz_ograniczen)]=tab_odciec1[1][i];
        M_calosc[2][(i+Rozmiar_macierz_ograniczen)]=tab_odciec1[2][i];
        }

    //Połączenie wektora znaków ograniczeń z wektorem znaków odcięć

    int *znaki_calosc= new int[(Rozmiar_macierz_ograniczen+g_odciecia)];

        //UZUPEŁNIENIE MACIERZY ZNAKAMI OGRANICZEŃ

        //M_calosc[kolumny][wiersze]     //M[kolumny][wiersze]
            for(int i=0; i<Rozmiar_macierz_ograniczen ; i++)
            {
            znaki_calosc[i]=znaki[i];
            }

        //UZUPEŁNIENIE MACIERZY ZNAKAMI ODCIĘĆ

            for(int i=0; i<g_odciecia ; i++)
            {
            znaki_calosc[(i+Rozmiar_macierz_ograniczen)]=1;
            }

            //cout<<endl<<"0 :"<<M_calosc[0][0]<<" "<<M_calosc[1][0]<<" "<<M_calosc[2][0]<<endl;
            //cout<<endl<<"1 :"<<M_calosc[0][1]<<" "<<M_calosc[1][1]<<" "<<M_calosc[2][1]<<endl;
            //cout<<endl<<"2 :"<<M_calosc[0][2]<<" "<<M_calosc[1][2]<<" "<<M_calosc[2][2]<<endl;
            //cout<<endl<<"3 :"<<M_calosc[0][3]<<" "<<M_calosc[1][3]<<" "<<M_calosc[2][3]<<endl;
            //cout<<endl<<"4 :"<<M_calosc[0][4]<<" "<<M_calosc[1][4]<<" "<<M_calosc[2][4]<<endl;

    int ilosc_punktow=(Naj_p_x1*100);

    ilosc_punktow=floor(ilosc_punktow);

    double Punkty_dol[2][ilosc_punktow];       // ( x , y )
    double Punkty_gora[2][ilosc_punktow];      // ( x , y )

    //Punkty_dol[0][0] = 1; Punkty_dol[1][0] = 2;     Punkty_gora[0][0] = 5; Punkty_gora[1][0] = 6;
    //Punkty_dol[0][1] = 3; Punkty_dol[1][1] = 4;     Punkty_gora[0][1] = 7; Punkty_gora[1][1] = 8;

    double wartosc_najnizsza = 0, wartosc_najwyzsza = 0, os_x1 = 0;
    int numer_rownania_wartosc_najnizsza = 0;
    int numer_rownania_wartosc_najwyzsza = 0;
    //((((-1)*((M[1][j])*os_x1))+(M[0][j]))/(M[2][j]))
    //   (>= 1)       (<= 0)

    for(int i=1; i<ilosc_punktow ; i++)
    {

        wartosc_najnizsza = 0;
        wartosc_najwyzsza = 1000000;
        os_x1=i*0.01;

        Punkty_dol[0][i-1]=os_x1;
        Punkty_dol[1][i-1]=0;
        cout<<endl<<os_x1<<" "<<Punkty_dol[0][i-1]<<endl;

        Punkty_gora[0][i-1]=os_x1;
        Punkty_gora[1][i-1]=30;

        //Szukanie dolnego najwiekszego ograniczenia
        for(int j=0 ; j<Rozmiar_calosci ; j++)
        {
            if(znaki_calosc[j]==1) //Dla ograniczen dolnych najwieksze dolne ograniczenie
            {
                //cout<<"*1*";
                if(((((-1)*((M_calosc[1][j])*os_x1))+(M_calosc[0][j]))/(M_calosc[2][j]))>0) //dolne ograniczenie jako 0   //NIEPEWNA CZESC KODU!!!!!!!!!!!!!!!!!!
                {
                    if(((((-1)*((M_calosc[1][j])*os_x1))+(M_calosc[0][j]))/(M_calosc[2][j]))>wartosc_najnizsza) //Porownanie z innymi liniami
                    {
                        //cout<<"*2*";
                        wartosc_najnizsza=((((-1)*((M_calosc[1][j])*os_x1))+(M_calosc[0][j]))/(M_calosc[2][j]));
                        Punkty_dol[0][i-1]=os_x1;
                        Punkty_dol[1][i-1]=wartosc_najnizsza;
                    }
                }
                else
                {
                    //cout<<"*3*";
                    //Punkty_dol[0][i-1]=os_x1; //NIEPEWNA CZESC KODU!!!!!!!!!!!!!!!!!!
                    //Punkty_dol[1][i-1]=0;
                }
            }
        }

        for(int j=0 ; j<Rozmiar_calosci ; j++) //Dla ograniczen gornych sprawdzenie czy istnieje
        {
            if(znaki_calosc[j]==0)
            {
                //cout<<"*4*";
                if(Punkty_dol[1][i-1]<=((((-1)*((M_calosc[1][j])*os_x1))+(M_calosc[0][j]))/(M_calosc[2][j])))
                {
                //cout<<"*5*";
                }
                else
                {
                    //cout<<"*6*";
                    Punkty_dol[0][i-1]=os_x1;
                    Punkty_dol[1][i-1]=0;
                }
            }
        }

    // Szukanie najmniejszego gornego ograniczenia

        for(int k=0 ; k<Rozmiar_calosci ; k++)
        {
            if(znaki_calosc[k]==0)
            {
                if((((((-1)*((M_calosc[1][k])*os_x1))+(M_calosc[0][k]))/(M_calosc[2][k]))<=wartosc_najwyzsza)&&(((((-1)*((M_calosc[1][k])*os_x1))+(M_calosc[0][k]))/(M_calosc[2][k]))<=Punkty_gora[1][i-1]))
                {
                    wartosc_najwyzsza = ((((-1)*((M_calosc[1][k])*os_x1))+(M_calosc[0][k]))/(M_calosc[2][k]));
                    Punkty_gora[0][i-1]=os_x1;
                    Punkty_gora[1][i-1]=wartosc_najwyzsza;
                }
                else
                {
                    //juz podstawione ze sie rowna 20
                }

                if(((((-1)*((M_calosc[1][k])*os_x1))+(M_calosc[0][k]))/(M_calosc[2][k]))<0) //NIEPEWNA CZESC KODU !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                {
                    Punkty_gora[1][i-1]=0;
                }
            }

        }

        //Sprawdzenie czy dla dolnych ograniczen jest spelnione

        for(int k=0; k<Rozmiar_calosci ; k++) //NIEPEWNA CZESC KODU !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        {
            if(znaki_calosc[k]==1)
            {
                if(Punkty_gora[1][i-1]>((((-1)*((M_calosc[1][k])*os_x1))+(M_calosc[0][k]))/(M_calosc[2][k])))
                {

                }
                else
                {
                    //Punkty_gora[1][i-1]=0; // <-zmienic trzeba
                    Punkty_gora[1][i-1]=Punkty_dol[1][i-1];
                }
            }
        }

    //if(i<=10)
    //cout<<endl<< Punkty_dol[0][i-1] <<" "<< Punkty_dol[1][i-1] <<" -> "<< Punkty_gora[0][i-1] <<" "<< Punkty_gora[1][i-1] <<endl;
    }

    /* Rysowanie wykresu terenu wspolnego */

    QwtPlotCurve *curve_wspolne = new QwtPlotCurve() ;
    curve_wspolne->setTitle( "Some Points" );
    curve_wspolne->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    /* [x][y]  [kolumny][wiersze] */

    QPolygonF points_wspolne;

    for(int j=0; j<(ilosc_punktow-1); j++)
    {
            points_wspolne << QPointF( Punkty_dol[0][j], Punkty_dol[1][j] )<< QPointF( Punkty_gora[0][j], Punkty_gora[1][j] ); //( x , y ) << ( x , y )
    }


    curve_wspolne->setSamples( points_wspolne );

    curve_wspolne->setPen(Qt::yellow);
    curve_wspolne->attach( plot );


    //Punkt rozwiazania na wykresie

    //QwtPlotMarker* punkt = new QwtPlotMarker();
    //punkt->setSymbol(QwtSymbol(QwtSymbol::Diamond, Qt::red, Qt::NoPen, QSize(10,10)));
    //punkt->setSymbol(QwtSymbol(QwtSymbol::Ellipse, Qt::red, Qt::NoPen, QSize(10,10)));
    //punkt->setSymbol(Qt::red);
    //punkt->setValue(QPointF(1 , 4));
    //punkt->attach( plot );


    if(Rozmiar_macierz_ograniczen>=1)
    {
        QwtPlotCurve *curve11 = new QwtPlotCurve();
        curve11->setTitle( "Some Points" );
        curve11->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points11;
        points11 << QPointF( tab_wynik[1][0] , tab_wynik[2][0] ) << QPointF( (tab_wynik[1][0]*1.001) , (tab_wynik[2][0]*1.001) );
        curve11->setSamples( points11 );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve11->setPen(Qt::red,10,Qt::SolidLine);
        curve11->attach( plot );

        //dla drugiego punktu
        if ( !manyVect.empty() ) {
            QPolygonF points99;
            QwtPlotCurve *curve99 = new QwtPlotCurve();
            curve99->setTitle( "Some Points1" );
            curve99->setRenderHint( QwtPlotItem::RenderAntialiased, true );


            points99 << QPointF( manyVect[0], manyVect[1] ) << QPointF( (manyVect[0]*1.001) , (manyVect[1]*1.001) );
            curve99->setSamples( points99 );
            curve99->setPen(Qt::black,10,Qt::SolidLine);
            curve99->attach( plot );
        }
    }


    //FUNKCJA CELU

    if(Rozmiar_macierz_ograniczen>=1)
    {
        QwtPlotCurve *curve_F_C = new QwtPlotCurve();
        curve_F_C->setTitle( "Some Points" );
        curve_F_C->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points_F_C;
        points_F_C << QPointF( Punkty_F_C[0][0], Punkty_F_C[1][0] ) << QPointF( Punkty_F_C[2][0], Punkty_F_C[3][0] )
                << QPointF( Punkty_F_C[4][0], Punkty_F_C[5][0] ) << QPointF( Punkty_F_C[6][0], Punkty_F_C[7][0] );
        curve_F_C->setSamples( points_F_C );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve_F_C->setPen(Qt::red,2,Qt::DashLine);
        curve_F_C->attach( plot );
    }

    //cout<<endl<<"1R : "<<Punkty[0][0]<<" "<<Punkty[1][0]<<" "<<Punkty[2][0]<<" "<<Punkty[3][0]<<" "<<Punkty[4][0]<<" "<<Punkty[5][0]<<" "<<Punkty[6][0]<<" "<<Punkty[7][0]<<endl;
    //cout<<endl<<"2R : "<<Punkty[0][1]<<" "<<Punkty[1][1]<<" "<<Punkty[2][1]<<" "<<Punkty[3][1]<<" "<<Punkty[4][1]<<" "<<Punkty[5][1]<<" "<<Punkty[6][1]<<" "<<Punkty[7][1]<<endl;
    //cout<<endl<<"3R : "<<Punkty[0][2]<<" "<<Punkty[1][2]<<" "<<Punkty[2][2]<<" "<<Punkty[3][2]<<" "<<Punkty[4][2]<<" "<<Punkty[5][2]<<" "<<Punkty[6][2]<<" "<<Punkty[7][2]<<endl;
    //cout<<endl<<"4R : "<<Punkty[0][3]<<" "<<Punkty[1][3]<<" "<<Punkty[2][3]<<" "<<Punkty[3][3]<<" "<<Punkty[4][3]<<" "<<Punkty[5][3]<<" "<<Punkty[6][3]<<" "<<Punkty[7][3]<<endl;
    //cout<<endl<<"5R : "<<Punkty[0][4]<<" "<<Punkty[1][4]<<" "<<Punkty[2][4]<<" "<<Punkty[3][4]<<" "<<Punkty[4][4]<<" "<<Punkty[5][4]<<" "<<Punkty[6][4]<<" "<<Punkty[7][4]<<endl;

    //cout<<endl<<"1O : "<<Punkty_odciecia[0][0]<<" "<<Punkty_odciecia[1][0]<<" "<<Punkty_odciecia[2][0]<<" "<<Punkty_odciecia[3][0]<<" "<<Punkty_odciecia[4][0]<<" "<<Punkty_odciecia[5][0]<<" "<<Punkty_odciecia[6][0]<<" "<<Punkty_odciecia[7][0]<<endl;
    //cout<<endl<<"2O : "<<Punkty_odciecia[0][1]<<" "<<Punkty_odciecia[1][1]<<" "<<Punkty_odciecia[2][1]<<" "<<Punkty_odciecia[3][1]<<" "<<Punkty_odciecia[4][1]<<" "<<Punkty_odciecia[5][1]<<" "<<Punkty_odciecia[6][1]<<" "<<Punkty_odciecia[7][1]<<endl;



    //Pierwsza funkcja liniowa

    if(Rozmiar_macierz_ograniczen>=1)
    {
        QwtPlotCurve *curve1 = new QwtPlotCurve();
        curve1->setTitle( "Some Points" );
        curve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points1;
        points1 << QPointF( Punkty[0][0], Punkty[1][0] ) << QPointF( Punkty[2][0], Punkty[3][0] )
                << QPointF( Punkty[4][0], Punkty[5][0] ) << QPointF( Punkty[6][0], Punkty[7][0] );
        curve1->setSamples( points1 );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve1->setPen(Qt::red);
        curve1->attach( plot );
    }


    //Druga funkcja liniowa

    if(Rozmiar_macierz_ograniczen>=2)
    {
        QwtPlotCurve *curve2 = new QwtPlotCurve();
        curve2->setTitle( "Some Points" );
        curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points2;
        points2 << QPointF( Punkty[0][1], Punkty[1][1] ) << QPointF( Punkty[2][1], Punkty[3][1] )
                << QPointF( Punkty[4][1], Punkty[5][1] ) << QPointF( Punkty[6][1], Punkty[7][1] );
        curve2->setSamples( points2 );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve2->setPen(Qt::blue);
        curve2->attach( plot );
    }

    //Trzcia funkcja liniowa

    if(Rozmiar_macierz_ograniczen>=3)
    {
        QwtPlotCurve *curve3 = new QwtPlotCurve();
        curve3->setTitle( "Some Points" );
        curve3->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points3;
        points3 << QPointF( Punkty[0][2], Punkty[1][2] ) << QPointF( Punkty[2][2], Punkty[3][2] )
                << QPointF( Punkty[4][2], Punkty[5][2] ) << QPointF( Punkty[6][2], Punkty[7][2] );
        curve3->setSamples( points3 );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve3->setPen(Qt::blue);
        curve3->attach( plot );
    }

    //Czwarta funkcja liniowa

    if(Rozmiar_macierz_ograniczen>=4)
    {
        QwtPlotCurve *curve4 = new QwtPlotCurve();
        curve4->setTitle( "Some Points" );
        curve4->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points4;
        points4 << QPointF( Punkty[0][3], Punkty[1][3] ) << QPointF( Punkty[2][3], Punkty[3][3] )
                << QPointF( Punkty[4][3], Punkty[5][3] ) << QPointF( Punkty[6][3], Punkty[7][3] );
        curve4->setSamples( points4 );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve4->setPen(Qt::blue);
        curve4->attach( plot );
    }

    //Piąta funkcja liniowa

    if(Rozmiar_macierz_ograniczen>=5)
    {
        QwtPlotCurve *curve5 = new QwtPlotCurve();
        curve5->setTitle( "Some Points" );
        curve5->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points5;
        points5 << QPointF( Punkty[0][4], Punkty[1][4] ) << QPointF( Punkty[2][4], Punkty[3][4] )
                << QPointF( Punkty[4][4], Punkty[5][4] ) << QPointF( Punkty[6][4], Punkty[7][4] );
        curve5->setSamples( points5 );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve5->setPen(Qt::blue);
        curve5->attach( plot );
    }

    //Szósta funkcja liniowa

    if(Rozmiar_macierz_ograniczen>=6)
    {
        QwtPlotCurve *curve6 = new QwtPlotCurve();
        curve6->setTitle( "Some Points" );
        curve6->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF points6;
        points6 << QPointF( Punkty[0][5], Punkty[1][5] ) << QPointF( Punkty[2][5], Punkty[3][5] )
                << QPointF( Punkty[4][5], Punkty[5][5] ) << QPointF( Punkty[6][5], Punkty[7][5] );
        curve6->setSamples( points6 );

        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curve6->setPen(Qt::blue);
        curve6->attach( plot );
    }

    //ODCIĘCIA

    //Pierwsze odcięcie
    if(g_odciecia>=1)
    {
        QwtPlotCurve *curveg1 = new QwtPlotCurve();
        curveg1->setTitle( "Some Points" );
        curveg1->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF pointsg1;
        pointsg1 << QPointF( Punkty_odciecia[0][0], Punkty_odciecia[1][0] ) << QPointF( Punkty_odciecia[2][0], Punkty_odciecia[3][0] )
                << QPointF( Punkty_odciecia[4][0], Punkty_odciecia[5][0] ) << QPointF( Punkty_odciecia[6][0], Punkty_odciecia[7][0] );
        curveg1->setSamples( pointsg1 );
    cout<<endl<<"Punkty: "<<Punkty[0][0]<<" "<<Punkty_odciecia[0][0]<<endl;
        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curveg1->setPen(Qt::black,2);
        curveg1->attach( plot );
    }

    //Drugie odcięcie
    if(g_odciecia>=2)
    {
        QwtPlotCurve *curveg2 = new QwtPlotCurve();
        curveg2->setTitle( "Some Points" );
        curveg2->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF pointsg2;
        pointsg2 << QPointF( Punkty_odciecia[0][1], Punkty_odciecia[1][1] ) << QPointF( Punkty_odciecia[2][1], Punkty_odciecia[3][1] )
                << QPointF( Punkty_odciecia[4][1], Punkty_odciecia[5][1] ) << QPointF( Punkty_odciecia[6][1], Punkty_odciecia[7][1] );
        curveg2->setSamples( pointsg2 );
    cout<<endl<<"Punkty: "<<Punkty[0][1]<<" "<<Punkty_odciecia[0][1]<<endl;
        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curveg2->setPen(Qt::black,2);
        curveg2->attach( plot );
    }

    //Trzecie odcięcie
    if(g_odciecia>=3)
    {
        QwtPlotCurve *curveg3 = new QwtPlotCurve();
        curveg3->setTitle( "Some Points" );
        curveg3->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF pointsg3;
        pointsg3 << QPointF( Punkty_odciecia[0][2], Punkty_odciecia[1][2] ) << QPointF( Punkty_odciecia[2][2], Punkty_odciecia[3][2] )
                << QPointF( Punkty_odciecia[4][2], Punkty_odciecia[5][2] ) << QPointF( Punkty_odciecia[6][2], Punkty_odciecia[7][2] );
        curveg3->setSamples( pointsg3 );
    cout<<endl<<"Punkty: "<<Punkty[0][0]<<" "<<Punkty_odciecia[0][0]<<endl;
        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curveg3->setPen(Qt::black,2);
        curveg3->attach( plot );
    }

    //Czwarte odcięcie
    if(g_odciecia>=4)
    {
        QwtPlotCurve *curveg4 = new QwtPlotCurve();
        curveg4->setTitle( "Some Points" );
        curveg4->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF pointsg4;
        pointsg4 << QPointF( Punkty_odciecia[0][3], Punkty_odciecia[1][3] ) << QPointF( Punkty_odciecia[2][3], Punkty_odciecia[3][3] )
                << QPointF( Punkty_odciecia[4][3], Punkty_odciecia[5][3] ) << QPointF( Punkty_odciecia[6][3], Punkty_odciecia[7][3] );
        curveg4->setSamples( pointsg4 );
    cout<<endl<<"Punkty: "<<Punkty[0][0]<<" "<<Punkty_odciecia[0][0]<<endl;
        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curveg4->setPen(Qt::black,2);
        curveg4->attach( plot );
    }

    //Piąte odcięcie
    if(g_odciecia>=5)
    {
        QwtPlotCurve *curveg5 = new QwtPlotCurve();
        curveg5->setTitle( "Some Points" );
        curveg5->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QPolygonF pointsg5;
        pointsg5 << QPointF( Punkty_odciecia[0][4], Punkty_odciecia[1][4] ) << QPointF( Punkty_odciecia[2][4], Punkty_odciecia[3][4] )
                << QPointF( Punkty_odciecia[4][4], Punkty_odciecia[5][4] ) << QPointF( Punkty_odciecia[6][4], Punkty_odciecia[7][4] );
        curveg5->setSamples( pointsg5 );
    cout<<endl<<"Punkty: "<<Punkty[0][0]<<" "<<Punkty_odciecia[0][0]<<endl;
        //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

        curveg5->setPen(Qt::black,2);
        curveg5->attach( plot );
    }

    QwtPlotZoomer *przybliz;
    przybliz= new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot->canvas());
}
