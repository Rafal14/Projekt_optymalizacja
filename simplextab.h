#ifndef SIMPLEXTAB_H
#define SIMPLEXTAB_H

/*!
 * \file
 * \brief Definicja klasy SimplexTab
 *
 * Plik zawiera definicję klasy SimplexTab, która zawiera
 * tablicę sympleks i metody działające na tej tablicy
 */



#include <cmath>
#include <vector>
#include <climits>



//wartości zwracane przy wykonywaniu algorytmu sympleks
//
#define NO_DUAL               -3  //niespełniony warunek na dualną dopuszczalność
#define OPEN_TASK             -2  //zbiór rozwiązań dopuszczalnych istnieje
                                  //zadanie nieograniczone
#define NULL_SET              -1  //zbiór rozwiązań dopuszczalnych niespełniony
                                  //zbiór pusty
#define ONE_SOLUTION           1  //zbiór rozwiązań dopuszczalnych istnieje
                                  //i tylko jest 1 rozwiązanie
#define MANY_SOLUTION_CL_TASK  2  //wiele rozwiązań na zbiorze ograniczonym
#define MANY_SOLUTION_OP_TASK  3  //wiele rozwiązań na zbiorze nieograniczonym




using namespace std;



/*!
 * \brief Modeluje pojęcie sympleksu
 *
 * Klasa modeluje pojęcie sympleksu, zawierającego
 * tablicę sympleksową, jej wymiary, wektory indeksów
 * kolumn i wierszy oraz liczbę odcięć i wektor odcięć.
 */
class SimplexTab
{
private:
    /*!
     * \brief tableau
     *
     * Dwuwymiarowa tablica sympleksowa o rozmiarze
     * (liczba ograniczeń+1) x (liczba zmiennych+1)
     */
    double **tableau;  //wskaźnik na wskaźnik do zaalokowania
                       //tablicy  dynamicznej simpleks


    /*!
     * \brief orgTab
     *
     * Dwuwymiarowa tablica będąca kopią orginalnej tablicy sympleksowej
     * o rozmiarze
     * (liczba ograniczeń+1) x (liczba zmiennych+1)
     */
    vector<vector<double>> orgTab;   //orginalna tablica sympleks

    /*!
     * \brief variablesNo
     *
     * Liczba zmiennych zadania programowania liniowego
     * całkowitoliczbowego
     */
    int variablesNo;

    /*!
     * \brief constrainsNo
     *
     * Liczba ograniczeń zadania programowania liniowego
     * całkowitoliczbowego
     */
    int constrainsNo;

    /*!
     * \brief changedRowsIndTab
     *
     * Wektor określający kolejność wierszy w tablicy
     * sympleksowej
     */
    int *changedRowsIndTab;

    /*!
     * \brief changedColsIndTab
     *
     * Wektor określający kolejność kolumn w tablicy
     * sympleksowej
     */
    int *changedColsIndTab;

    /*!
     * \brief cutNo
     *
     * Określa liczbę wykonanych odcięć
     */
    unsigned int cutNo;

    /*!
     * \brief cuttPlainVector
     *
     * Wektor wektora na odcięcia (tablica m x n)
     */
    vector< vector<double> > cuttPlainVector;


public:
    /*!
     * \brief SimplexTab
     *
     * Inicjalizuje obiekt klasy SimplexTab
     * zerując jego wymiary
     */
    SimplexTab();

    /*!
     * \brief ~SimplexTab()
     *
     * Desktruktor obiektu klasy SimplexTab
     */
    ~SimplexTab();

    /*!
     * \brief AddValues
     *
     * Wypełnia początkową tablicę sympleksową
     *
     * \param variablesNum
     * Liczba zmiennych zadania PLC
     *
     * \param constrainsNum
     * Liczba ograniczeń zadania PLC
     *
     * \param c
     * Wektor współczynników funkcji celu
     *
     * \param b
     * Wektor zmiannych bazowych - wyrazów wolnych przy ograniczeniach
     *
     * \param N
     * Tablica zmiennych niebazowych - współczynników przy ograniczeniach
     */
    void AddValues(const int  &variablesNum, const int  &constrainsNum,
                         int *c, int *b, int **N);

    /*!
     * \brief isEmpty
     * Sprawdza czy utworzono tablicę sympleksową
     *
     * \return false - gdy utworzono tablicę sympleks
     * \return true  - w przyciwnym przypadku
     */
    bool isEmpty();

    /*!
     * \brief removeTab
     * Usuwa zaalokowaną tablicę sympleksową
     */
    void removeTab();

    /*!
     * \brief getSimplexTableau
     * Zwraca utworzoną tablicę sympleksową
     *
     * \return wskaźnik na obszar pamięci z zaalokowaną tablicą sympleks
     */
    double **getSimplexTableau();

    void resizeSimp(const int &vars, const int &constrs,
                      int *indRowTab,  int *indColTab,
                      double **newTableau);

    /*!
     * \brief getOrginalSimplexTableau
     *
     * Zwraca orginalną tablicę sympleksową
     *
     * \return wektor wektora (pojemnik z STL) oryginalnej tablicy sympleks
     */
    vector<vector<double>> getOrginalSimplexTableau();

    /*!
     * \brief isNonNegativeBase
     * Sprawdź czy wyrazy wolne ograniczeń są większe od zera
     *
     * \return true - gdy baza jest niezerowa
     * \return false- w przeciwnym przypadku
     */
    bool isNonNegativeBase();

    /*!
     * \brief isOptimalSolution
     * Sprawdz czy rozwiązanie jest optymalne dla algorymu sympleks prymalnego,
     * czyli czy pierwszy wiersz ma elementy niezerowe
     *
     * \return true - gdy rozwiązanie jest optymalne
     * \return false- w przeciwnym przypadku
     */
    bool isOptimalSolution();

    /*!
     * \brief solveZeroSolution
     * Sprawdz czy rozwiązanie nie ma zera dla algorymu sympleks prymalnego,
     * czyli czy pierwszy wiersz ma elementy zerowe
     *
     * \return true - gdy rozwiązanie jest zerowe
     * \return false- w przeciwnym przypadku
     */
    bool solveZeroSolution();

    /*!
     * \brief checkTypeSolution
     * Sprawdź czy jaki jest typ rozwiązania
     *
     * \return kod określający typ rozwiązania
     */
    int  checkTypeSolution();

    /*!
     * \brief getRowsIndTab
     * Zwraca wektor zawierający indeksy wierszy tablicy sympleksowej
     *
     * \return wskaźnik do obszaru pamięci zawierającego wektor indeksów wierszy
     *         tablicy sympleksowej
     */
    int *getRowsIndTab();

    /*!
     * \brief getColsIndTab
     * Zwraca wektor zawierający indeksy kolumn tablicy sympleksowej
     *
     * \return wskaźnik do obszaru pamięci zawierającego wektor indeksów kolumn
     *         tablicy sympleksowej
     */
    int *getColsIndTab();

    /*!
     * \brief getConstrainsNumber
     * Zwraca liczbę ogranień
     *
     * \return liczba ograniczeń zadania PLC
     */
    int getConstrainsNumber();

    /*!
     * \brief getVariablesNumber
     * Zwraca liczbę zmiennych
     *
     * \return liczba zmiennych zadania PLC
     */
    int getVariablesNumber();

    /*!
     * \brief getFractOfInt
     * Zwraca część ułamkową
     *
     * \param value
     * Referencja do zmiennej typu double
     *
     * \return część całkowita zmiennej
     */
    double getFractOfInt(const double &value);

    /*!
     * \brief getCuttingPlainNumber
     * \return liczbę odcięć
     */
    int getCuttingPlainNumber();

    /*!
     * \brief getCuttingPlainVector
     * \return zwraca wektor ciec
     */
    vector< vector<double> > getCuttingPlainVector();

    /*!
     * \brief isInteger
     * Sprawdza czy liczba jest całkowita przy określonej
     * dokładności elipson
     *
     * \param value
     * Wartość liczbowa do sprawdzenia
     *
     * \param elipson
     * Określa dokładności sprawdzania
     *
     * \return true - gdy liczba jest całkowita dla zadanej dokładości
     * \return false- gdy liczba nie jest całkowita dla zadanej dokładności
     */
    bool isInteger(const double &value, const double &elipson);

    /*!
     * \brief isIntegralSolution
     * Sprawdza czy rozwiązanie (pierwsza kolumna tab) jest cakłowitoliczbowa
     *
     * \param elipson
     * Określa dokładności sprawdzania
     *
     * \return true - gdy rozwiązanie jest całkowitoliczbowe
     * \return false- gdy rozwiązanie nie jest całkowitoliczbowe
     */
    bool isIntegralSolution(const double &elipson);

    /*!
     * \brief sortRows
     * Sortuje tablicę sympleksową względem wierszy
     */
    void sortRows();

    /*!
     * \brief sortCols
     * Sortuje tablicę sympleksową względem kolumn
     */
    void sortCols();

    /*!
     * \brief findElem
     * Znajduje element o podanych indeksach
     * \param ind
     * określa indeks elementu
     *
     * \return wartość elementu
     */
    double findElem(const int &ind);

    /*!
     * \brief pivoting
     * Upraszcza tablicę sympleksową
     *
     * \param selRow
     * Określa wiersz elemntu centralnego
     *
     * \param selCol
     * Określa kolumnę elementu centralnego
     */
    void pivoting(const int &selRow, const int &selCol);

    /*!
     * \brief solveSimplex
     * Rozwiązuje tablicę sympleksową z zastosowaniem
     * algorytmu prymalnego - stosowany w drugiej fazie
     * metody dwufazowej
     *
     * \return kod typu rozwiązania
     */
    int solveSimplex();

    /*!
     * \brief twoPhaseSimplex
     * Rozwiązuje zadanie PL z zastosowaniem algorytmu
     * dwufazowego
     *
     * \return kod typu rozwiązania
     */
    int twoPhaseSimplex();

    /*!
     * \brief dualSimplex
     * Rozwiązuje zadanie PL z zastosowaniem algorytmu
     * dualnego, metoda wykorzystywana w kolejnych iteracjach
     * odcięć
     *
     * \return kod typu rozwiązania
     */
    int dualSimplex();

    /*!
     * \brief doGomoryMethod
     * Rozwiązuje zadanie PLC z zastosowaniem algorytmu
     * odcięć Gomory'ego
     *
     */
    void doGomoryMethod();
};

#endif // SIMPLEXTAB_H
