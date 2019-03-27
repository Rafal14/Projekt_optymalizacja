#include "simplextab.h"




SimplexTab::SimplexTab()
{
    variablesNo  = 0;
    constrainsNo = 0;

    //liczba odciec
    cutNo = 0;
}

//desktruktor obiektu klasy SimplexTab
SimplexTab::~SimplexTab()
{
    if ( !isEmpty() ) {
        //usuń tablicę sympleksową
        removeTab();

        //usuń tablice indeksów
        delete [] changedRowsIndTab;
        delete [] changedColsIndTab;
    }
}

// c   wektor wsp.
// b   wektor bazowy
// N   macierz niebazowa
void SimplexTab::AddValues(const int  &variablesNum,
                           const int  &constrainsNum,
                                 int  *c,
                                 int  *b,
                                 int **N)
{
    int i,j,k;

    //zapisanie liczby zmiennych i ograniczeń
    variablesNo  = variablesNum;
    constrainsNo = constrainsNum;

    //tablica zmian indeksów wierszy
    changedRowsIndTab = new int [constrainsNum+1];
    changedRowsIndTab[0] = 0;
    for(int l=1; l < constrainsNum+1; ++l) {
        changedRowsIndTab[l] = variablesNum + l;
    }

    //tablica zmian indeksów kolumn
    changedColsIndTab = new int [variablesNum];
    for (int m=0; m < variablesNum; ++m) {
        changedColsIndTab[m] = m+1;
    }


    //utwórz tablicę simpleksową
    tableau = new double * [constrainsNo+1];
    for (i=0; i < constrainsNo+1; ++i) {
        tableau[i] = new double [variablesNo+1];

        for (j=0, k=0; j < variablesNo+1; ++j, ++k) {

            //reguła dla pierwszego wiersza
            if (i==0) {
                //wpisanie pierwszego elementu
                if (j==0) {
                    tableau[i][j] = 0;
                }
                //wpisanie współczynników  -c
                else {
                    tableau[i][j] = c[j-1] * (-1.0);
                }
            }
            //reguła dla pozostałych wierszy
            else {
                //wpisanie wektora bazowego
                if (j==0) {
                    tableau[i][j] = b[i-1];
                }
                //wpisanie elementów macierzy niebazowej
                else {
                    tableau[i][j] = N[i-1][j-1];
                }
            }
        }
    }

    //utwórz kopię orginalnej tablicy sympleks
    for (int row=0; row < constrainsNum+1; ++row) {
        vector<double> rowTab;
        for (int col=0; col < variablesNum+1; ++col) {
            rowTab.push_back(tableau[row][col]);
        }
        orgTab.push_back(rowTab);
    }
}

bool SimplexTab::isEmpty()
{
    if (constrainsNo == 0 || variablesNo == 0) {
        return true;
    }
    return false;
}

void SimplexTab::removeTab()
{
    if ( !isEmpty() ) {
        for (int i=0; i < constrainsNo+1; ++i) {
            delete [] tableau[i];
        }
        delete [] tableau;


        //usuń tablice indeksów
        delete [] changedColsIndTab;
        delete [] changedRowsIndTab;

        constrainsNo = 0;
        variablesNo  = 0;
        cutNo        = 0;

        //usuń wektor odcięć
        while (!cuttPlainVector.empty())
            cuttPlainVector.pop_back();

        //usuń kopie orginalnej tablicy sympleks
        while (!orgTab.empty())
            orgTab.pop_back();
    }
}

double** SimplexTab::getSimplexTableau()
{
    return tableau;
}

void SimplexTab::resizeSimp(const int &vars, const int &constrs, int *indRowTab,
                            int *indColTab, double **newTableau)
{
    if ( !isEmpty() ) {
        removeTab();

        constrainsNo = constrs;
        variablesNo  = vars;

        changedColsIndTab = new int [vars];
        changedRowsIndTab = new int [constrs+1];

        //utwórz tablicę simpleksową
        tableau = new double * [constrs+1];
        for (int i=0; i < constrs+1; ++i) {
            changedRowsIndTab[i] = indRowTab[i];

            tableau[i] = new double [vars+1];
            for (int j=0; j < vars+1; ++j) {
                tableau[i][j] = newTableau[i][j];

                if (j>0)
                    changedColsIndTab[j-1] = indColTab[j-1];
            }
        }
    }
}

vector<vector<double> > SimplexTab::getOrginalSimplexTableau()
{
    return orgTab;
}

bool SimplexTab::isNonNegativeBase()
{
    for(int i=1; i < constrainsNo+1; ++i) {
        if (tableau[i][0] < 0) {
            return false;
        }
    }
    return true;
}

bool SimplexTab::isOptimalSolution()
{
    for(int j=0; j < variablesNo+1; ++j) {
        if (tableau[0][j] < 0) {
            return false;
        }
    }
    return true;
}

bool SimplexTab::solveZeroSolution()
{
    int    selectedCol = 0;
    double prior       = 0.1;
    bool   flag        = false;

    for (int m=1; m < variablesNo+1; ++m) {
        if (tableau[0][m] == 0)
            selectedCol = m;
            flag = true;
    }

    if (!flag) {
        return flag;
    }


    //wybierz taki wiersz gdzie dla j-tej kolumny iloraz b_i0 / a_ij jest najmniejszy a_ij > 0
    double actual      =  0.0;
    int    selectedRow =  0;


    prior = INT_MAX;
    for (int k=1; k < constrainsNo+1; ++k) {

        //tylko dla dodatnich wyrazów macierzy niebazowej
        if ( tableau[k][selectedCol] > 0 ) {
            actual = tableau[k][0] / tableau[k][selectedCol];

            if ( actual < prior ) {
                prior      = actual;
                selectedRow = k;
            }
        }
    }

    //upraszczanie tab sympleks
    pivoting(selectedRow, selectedCol);

    return flag;
}

int SimplexTab::checkTypeSolution()
{
    int solution = ONE_SOLUTION;

    //sprawdzenie czy występuje ujemna baza
    for (int k=1; k < constrainsNo+1; ++k) {
        if (tableau[k][0] < 0) {
            return NULL_SET;
        }
    }

    //sprawdz przypadek - nieskończona liczba
    //rozwiązań na nieograniczonym zbiorze
    for (int j=1; j < variablesNo+1; ++j) {
        if (tableau[0][j] == 0) {
            for (int i=1; i < constrainsNo+1; ++i) {
                if (tableau[i][j] > 0) {
                    solution = MANY_SOLUTION_CL_TASK;
                }
                else {
                    solution = MANY_SOLUTION_OP_TASK;
                }
            }

            if (solution == MANY_SOLUTION_OP_TASK
                    || solution == MANY_SOLUTION_CL_TASK)
                return solution;

            for (int r=1; r < constrainsNo+1; ++r) {
                if (tableau[r][0] > 0) {
                    solution = MANY_SOLUTION_CL_TASK;
                }

                if (tableau[r][0] == 0){
                    solution = MANY_SOLUTION_OP_TASK;
                }
            }

            if (solution == MANY_SOLUTION_OP_TASK
                    || solution == MANY_SOLUTION_CL_TASK)
                return solution;
        }
    }

    return solution;
}

int *SimplexTab::getRowsIndTab()
{
    return changedRowsIndTab;
}

int *SimplexTab::getColsIndTab()
{
    return changedColsIndTab;
}

int SimplexTab::getConstrainsNumber()
{
    return constrainsNo;
}

int SimplexTab::getVariablesNumber()
{
    return variablesNo;
}

double SimplexTab::getFractOfInt(const double &value)
{
    double fraction;
    fraction = value - floor(value);
    return fraction;
}

int SimplexTab::getCuttingPlainNumber()
{
    return cutNo;
}

vector<vector<double> > SimplexTab::getCuttingPlainVector()
{
    return cuttPlainVector;
}

bool SimplexTab::isInteger(const double &value, const double &elipson)
{
    double part   = 0.0;
    double diff   = 0.0;
    double minimal= 0.0;

    part = getFractOfInt(value);
    diff = 1 - part;

    //sprawdz czy jest calkowita
    minimal = (part<diff) ? part:diff;

    if ( minimal <= elipson ) {
        return true;
    }
    return false;
}

bool SimplexTab::isIntegralSolution(const double &elipson)
{
    for (int i=1; i < constrainsNo+1; ++i) {
        if (!isInteger(tableau[i][0], elipson)) {
            return false;
        }
    }
    return true;
}

void SimplexTab::sortRows()
{
    int sizeTab = constrainsNo+1;
    //tymczasowy wiersz
    double *rowsTab = new double [variablesNo+1];

    //sortowanie Shella
    int i, j, tmp;

    // liczba elementów w każdym ze zbiorów
    // na które została podzielona tablica
    int step = sizeTab/2;

    // dopóki mozna podzielic zbior na 2 podzbiory
    while(step) {
        for (i = step; i < sizeTab; ++i ) {
            j = i;              // zapamietaj biezacy indeks

            // zapamietaj wiersz
            for (int p=0; p < variablesNo+1; ++p) {
                rowsTab[p] = tableau[i][p];
            }
            tmp = changedRowsIndTab[i];

            // uporządkuj elementy w tablicy od najmniejszego do największego

            for (j=i; ( j >= step)  &&
                  ( changedRowsIndTab[j-step] > tmp ); j -= step ) {
                changedRowsIndTab[j] = changedRowsIndTab[j - step];
                for (int t=0; t < variablesNo+1; ++t) {
                    tableau[j][t] = tableau[j - step][t];
                }
            }
            changedRowsIndTab[j] = tmp;
            for (int q=0; q < variablesNo+1; ++q) {
                tableau[j][q] = rowsTab[q];
            }
        }
        step = step/2;
      }

    //usun tymczasowy wiersz
    delete [] rowsTab;
}

void SimplexTab::sortCols()
{
    int sizeTab = variablesNo;
    //tymczasowa kolumna
    double *colsTab = new double [constrainsNo+1];

    //sortowanie Shella
    int i, j, tmp;

    // liczba elementów w każdym ze zbiorów
    // na które została podzielona tablica
    int step = sizeTab/2;

    // dopóki mozna podzielic zbior na 2 podzbiory
    while(step) {
        for (i = step; i < sizeTab; ++i ) {
            j = i;              // zapamietaj biezacy indeks

            // zapamietaj kolumnę
            for (int p=0; p < constrainsNo+1; ++p) {
                colsTab[p] = tableau[p][i+1];
            }
            tmp = changedColsIndTab[i];

            // uporządkuj elementy w tablicy od najmniejszego do największego

            for (j=i; ( j >= step)  &&
                  ( changedColsIndTab[j-step] > tmp ); j -= step ) {
                changedColsIndTab[j] = changedColsIndTab[j - step];
                for (int t=0; t < constrainsNo+1; ++t) {
                    tableau[t][j+1] = tableau[t][j - step+1];
                }
            }
            changedColsIndTab[j] = tmp;
            for (int q=0; q < constrainsNo+1; ++q) {
                tableau[q][j+1] = colsTab[q];
            }
        }
        step = step/2;
      }

    //usun tymczasowy wiersz
    delete [] colsTab;
}

double SimplexTab::findElem(const int &ind)
{
    for (int j=1; j < variablesNo+1; ++j) {
        if ( changedColsIndTab[j-1] == ind ) {
            return tableau[0][j];
        }
    }
    for (int i=1; i < constrainsNo+1; ++i) {
        if ( changedRowsIndTab[i] == ind ) {
            return tableau[i][0];
        }
    }
    return -1.0;
}

void SimplexTab::pivoting(const int &selRow, const int &selCol)
{
    int    value       =  0;

    //zapisanie wyboru elementu centralnego
    value                            = changedRowsIndTab[selRow];
    changedRowsIndTab[selRow]   = changedColsIndTab[selCol-1];
    changedColsIndTab[selCol-1] = value;


    //dla wybranego elemntu tableau[maximRow][maximcol]
    //wybor elemntu centralnego - jego odwrotnosc

    double mul=0.0;
    for(int p=0; p < constrainsNo+1; ++p) {
        for (int q=0; q < variablesNo+1; ++q) {

            if (p != selRow) {
                if (q != selCol) {

                    //tutaj
                    mul = tableau[selRow][q] * tableau[p][selCol];
                    mul = mul / tableau[selRow][selCol];
                    tableau[p][q] -= mul;
                }
            }

        }
    }

    //nowa wartosc elem centralnego
    tableau[selRow][selCol] = 1 / tableau[selRow][selCol];



    //dla j-tej kolumny elem centralnego stary * (- elem centr )
    for (int l=0; l < constrainsNo+1; ++l) {
        if ( l != selRow ) {
            tableau[l][selCol] = tableau[l][selCol] * (-tableau[selRow][selCol]);
        }
    }

    //dla i-tego wiersza - wiersza centralnego (mnozymy elementy prze 1/p)
    for (int m=0; m < variablesNo+1; ++m) {
        if (m != selCol ) {
            tableau[selRow][m] = tableau[selRow][m] * tableau[selRow][selCol];
        }
    }
}



int SimplexTab::solveSimplex()
{
    //sprawdzenie warunku dopuszczalności
    //warunek: nieujemne współczynniki wektora bazowego y_i0
    if ( !isNonNegativeBase() ) {
        return NULL_SET;
    }

    //test optymalności rozwiązania
    //warunek: dodatnie wartości pierszego wiersza
    if ( isOptimalSolution() ) {
        return ONE_SOLUTION;
    }


    //wybór zmiennej wchodzącej do bazy
    int selectedCol    = 0;
    double prior       = 0;
    //wybor kolumny o największym współczynniku (w tab simplex najmniejszym)
    for (int i=1; i < variablesNo+1; ++i) {
        if ( tableau[0][i] < prior ) {
            prior       = tableau[0][i];
            selectedCol = i;
        }
    }

    //wybierz taki wiersz gdzie dla j-tej kolumny iloraz b_i0 / a_ij jest najmniejszy a_ij > 0
    double actual      =  0.0;
    int    selectedRow =  0;


    prior = INT_MAX;
    for (int k=1; k < constrainsNo+1; ++k) {

        //tylko dla dodatnich wyrazów macierzy niebazowej
        if ( tableau[k][selectedCol] > 0 ) {
            actual = tableau[k][0] / tableau[k][selectedCol];

            if ( actual < prior ) {
                prior      = actual;
                selectedRow = k;
            }
        }
    }

    //upraszczanie tab sympleks
    pivoting(selectedRow, selectedCol);

    return 0;
}

int SimplexTab::twoPhaseSimplex()
{
    int  retValue    =0;
    int  val         =0;
    int  selectedRow =0;
    int  selectedCol =0;
    bool  isFound    =false;
    //wybór pomocniczej funkcji celu
    for (int j=1; j < constrainsNo+1; ++j) {
        if (tableau[j][0] < val) {
            val    = tableau[j][0];
            selectedRow = j;
            isFound= true;     //znaleziono taka wartosc
        }
    }

    //realizacja I fazy
    if (isFound) {
        isFound = false;
        val     = 0;


        //wybierz
        for (int c=1; c < variablesNo+1; ++c) {
            if ( tableau[selectedRow][c] < val ) {
                val    = tableau[selectedRow][c];
                selectedCol = c;
                isFound= true;
            }
        }

        if (!isFound) {
            return NULL_SET;  //brak rozwiazania dopuszcalnego
        }


        //wybór elementu
        double frac = 0.0;
        double prev = INT_MAX;
        for (int i=1; i < constrainsNo+1; ++i) {
            if (tableau[i][selectedCol] > 0) {
                frac = tableau[i][0] / tableau[i][selectedCol];

                if (frac < prev) {
                    prev = frac;
                    selectedRow = i;
                }
            }
        }

        //wybrany elem centralny
        pivoting(selectedRow, selectedCol);
    }

    //realizacja algorytmu simplex - prymalny
    //pod warunkiem spelnienia warunku dopuszczalnosci
    retValue = solveSimplex();

    return retValue;
}

int SimplexTab::dualSimplex()
{
    //sprawdzenie warunku na dualną dopuszczalność
    for (int k=1; k < variablesNo+1; ++k) {
        if ( tableau[0][k] < 0 ) {
            return NO_DUAL;
        }
    }

    //test optymalności rozwiązania
    if ( isNonNegativeBase() ) {
        return ONE_SOLUTION;
    }

    //wybór zmiennej usuwanej z bazy
    //wybór wiersza

    int selectedRow    = 0;

    double prior       = 0;
    //wybor wiersza o najmniejszym współczynniku
    for (int i=1; i < constrainsNo+1; ++i) {
        if ( tableau[i][0] < prior ) {
            prior       = tableau[i][0];
            selectedRow = i;
        }
    }

    //wybierz taką kolumne gdzie dla i-tego wiersza y_0j / y_ij jest najmniejszy y_ij > 0
    double actual      =  0.0;
    int selectedCol    = 0;

    prior = INT_MIN;
    for (int l=1; l < variablesNo+1; ++l) {

        //tylko dla ujemnych wartości wybranego wiersza
        if ( tableau[selectedRow][l] < 0 ) {
            actual = tableau[0][l] / tableau[selectedRow][l];

            if ( actual > prior ) {
                prior       = actual;
                selectedCol = l;
            }
        }
    }

    //upraszczanie tab sympleks
    pivoting(selectedRow, selectedCol);

    return 0;
}

void SimplexTab::doGomoryMethod()
{
    int selRow=0;

    int conNo = constrainsNo;
    int varNo = variablesNo;

    //utwórz tablicę dla wyboru odcięcia
    double plainTab[constrainsNo+1][variablesNo+1];

    //dla sum
    double sumTab[constrainsNo+1];


    //optymalizacja tablicy simpleks
    for (int i=0; i < constrainsNo+1; ++i) {
        for (int j=0; j < variablesNo+1; ++j) {
            plainTab[i][j] = getFractOfInt(tableau[i][j]);
        }
    }

    //regula  wybory wiersza do odcięcia - regula sumy
    double sum=0.0;
    for (int k=0; k < constrainsNo+1; ++k) {
        for (int l=1; l < variablesNo+1; ++l) {
            sum += plainTab[k][l];
        }
        sumTab[k] = sum;
        sum = 0.0;
    }

    for (int m=0; m < constrainsNo+1; ++m) {
        if (sumTab[m] != 0)
            sumTab[m] = plainTab[m][0] / sumTab[m];
        else
            sumTab[m] = INT_MIN;
    }

    for (int n=1; n < constrainsNo+1; ++n) {
        if ( sumTab[n] >= sumTab[n-1] ) {
            selRow = n;
        }
    }


    //powiekszenie tablicy simpeks

    //utwórz tablicę simpleksową
    double **nTableau = new double * [conNo+2];
    for (int r=0; r < conNo+2; ++r) {
        nTableau[r] = new double [varNo+1];

        //przepisz dotychczasow tablice
        for (int s=0; s < varNo+1; ++s) {
            if ( r < conNo+1 )
                nTableau[r][s] = tableau[r][s];
            else
                nTableau[r][s] = -plainTab[selRow][s];
        }
    }
    //usun dotychasową tablicę simpleks
    if ( !isEmpty() ) {
        for (int i=0; i < constrainsNo+1; ++i) {
            delete [] tableau[i];
        }
        delete [] tableau;
    }

    tableau = nullptr;
    //zapisz nowe wartości liczby zmiennych i ograniczeń
    variablesNo = varNo;
    constrainsNo= conNo+1;
    tableau = nTableau;

    nTableau = nullptr;

    //zwiększ licznik odcięć
    ++cutNo;

    //powiększ tablicę ideksów wierszy
    int *nchangedRowTab = new int [constrainsNo+1];
    for (int q=0; q < constrainsNo+1; ++q) {
        if (q < constrainsNo)
            nchangedRowTab[q] = changedRowsIndTab[q];
        else
            nchangedRowTab[q] = cutNo * 11;
    }
    delete [] changedRowsIndTab;
    changedRowsIndTab = nullptr;

    changedRowsIndTab = nchangedRowTab;


    //przeliczenie odcięcia do wyrysowania na wykresie

    //zapisanie odcięcia w wektorze wektorów
    vector<double> plVector;
/*
    if (varNo == 2) {
        double maxValue=0.0;


        if (abs(plainTab[selRow][1]) < 0.001) {
            if (changedColsIndTab[2] < 11) {
                maxValue = orgTab[selRow][0] - (plainTab[selRow][0] / plainTab[selRow][2]);
            }
        }
        else {
            if (abs(plainTab[selRow][2]) < 0.001) {
                if (changedColsIndTab[1] < 11) {
                    maxValue = orgTab[selRow][0] - (plainTab[selRow][0] / plainTab[selRow][1]);
                }
            }
            else {
                maxValue = plainTab[selRow][1] ? (plainTab[selRow][1] >= plainTab[selRow][2]) : plainTab[selRow][2];
                maxValue = orgTab[selRow][0] - (plainTab[selRow][0] / maxValue);
            }
        }

        plVector.push_back( maxValue );
        plVector.push_back( orgTab[selRow][1] );
        plVector.push_back( orgTab[selRow][2] );
        cuttPlainVector.push_back( plVector );
    }

    */


}
