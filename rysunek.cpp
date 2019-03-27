#include "rysunek.h"
#include "ui_rysunek.h"

#include "mainwindow.h"
#include <QString>
#include <QStringList>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

#include <math.h>
#include <iostream>
using namespace std;



rysunek::rysunek(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rysunek)
{
    ui->setupUi(this);


    /* UWAGA - TEST    WIDŻETU  QWT DO RYSOWANIA FUNKCJI */

        //MainWindow.orginalTableau.push_back(1);

        //cout<<"test1 = "<<test1<<endl;

        /* Otrzymywane dane do utworzenia wykresu (TESTOWE)*/

        /* Wektor wspołczynników celu */
        double W_w_c_t [2] = {0.5 , 0.5};           //Wektor współczynników celu testowy
        double Rozmiar_W_w_c_t = (sizeof(W_w_c_t)/sizeof(*W_w_c_t)); //Sprawdzenie rozmiaru

        /* Wektor wyrazów wolnych */
        double W_w_w_t [3] = {2, 3, 6};                 //Wektor wyrazów wolnych testowy

        /* Macierz wspołczynników ograniczeń */
        double M_w_o_t [2][3];                 //Macierz wspołczynników celu testowa
        /* [kolumny][wiersze] */

        M_w_o_t[0][0]=2; M_w_o_t[1][0]=1;  //M_w_o_t[2][0]=222;
        M_w_o_t[0][1]=-1; M_w_o_t[1][1]=1;   //M_w_o_t[2][1]=444;
        M_w_o_t[0][2]=1; M_w_o_t[1][2]=1;  //M_w_o_t[2][2]=888;

        int Rozmiar_macierz_ograniczen = (sizeof(W_w_w_t)/sizeof(*W_w_w_t)); //Sprawdzenie ilości kolumn macierzy, przykładowo macierz[5][10], zwróci wartość 5
        int Rozmiar_macierz_glowna = Rozmiar_macierz_ograniczen+1;

        /* Macierz znaków */
        /* A_1x_1 * A_2x_2 >= b_1 */
        /* (">=" -> 1) ("<=" -> 0) */
        int *znaki= new int[Rozmiar_macierz_ograniczen];
        znaki[0]=1;           znaki[1]=1;           znaki[2]=0;          znaki[3]=0;
        /*niebieska linia     czerwona linia        zielona linia        żółta linia */

        /* Utworzenie głównej macierzy do generowania wykresu */

        double **M = new double *[3]; //Utworzenie kolumn

        for(int i=0; i<3 ; i++)
        {
            M[i]= new double [Rozmiar_macierz_glowna];  //Utworzenie wierszy
        }

        /* Wypełnienie macierzy podanymi współczynnikami [kolumny][wiersze]*/

        M[0][0]=0; M[1][0]=W_w_c_t[0]; M[2][0]=W_w_c_t[1]; //Wpisanie wektora współczynnióow celu

        /*Zamiast 0 dajemy 2 */
        M[0][0]=0.5;

        /* Zmiana znaków */
        //for(int i=0; i<3 ;i++)
        //{
        //   M[i][0]=M[i][0]*(-1);
        //}

        for(int i=1 ; i<3; i++) //Wpisanie do macierzy współczynników macierzy ograniczeń
            for(int j=1 ; j<Rozmiar_macierz_glowna ; j++)
            {
            M[i][j]=M_w_o_t[i-1][j-1];
            }

        for(int i=1 ; i<Rozmiar_macierz_glowna ; i++) //Wpisanie do macierzy wyrazów wolnych
            {
            M[0][i]=W_w_w_t[i-1];
            }

        /* Sprawdzenie czy podane macierze i wektory składają się z liczb całkowitoliczbowych z ustalonym błędem */


        /* Sprawdzenie wielkości macierzy lub wektora */
        /* Sprawdzenie ilości kolumn */
        //int ilosc_kolumn1 = (sizeof(M)/sizeof(*M));
        /* Sprawdzenie ilości wierszy */
        //int ilosc_wierszy1 = (sizeof(M[0])/sizeof(M[0][0]));

        /* Obliczenia na macierzy glownej i generowanie wykresu */





        //utworzenie obiektu typu wykres
        QwtPlot *plot = new QwtPlot();

        //dodanie wykresu do okna Mainwindow
        ui->verticalLayout->addWidget(plot);

        plot->setTitle("Wykres warstwic");

        //ustawienie tła wykresu
        plot->setCanvasBackground(Qt::white);

        QwtPlotGrid *grid = new QwtPlotGrid();
        grid->attach( plot );



        /* Obliczenia */

        /* Tworzenie macierzy punktów w których funkcje liniowe przecinaja osie */

        double **Punkty = new double *[8]; //Tworzenie kolumn

        for(int i=0; i<8 ; i++)
        {
            Punkty[i] = new double[Rozmiar_macierz_glowna]; //Tworzenie wierszy
        }

        /* Znajdowanie punktów przecinających się na osiach współrzędnych */

        // Pierwsze punkty
        for(int j=0; j<Rozmiar_macierz_glowna ; j++)
        {
            Punkty[2][j]=0; //x (x1)
            Punkty[3][j]=M[0][j]/M[2][j];   //y (x2)
        }

        //Drugie punkty
        for(int j=0; j<Rozmiar_macierz_glowna ; j++)
        {
            Punkty[4][j]=M[0][j]/M[1][j];   //x (x1)
            Punkty[5][j]=0; //y (x2)
        }

        /* Przedłużanie funkcji liniowych */

        /* Szukanie najdłuższego i najkrótszego punktu */

        int zm1=0; //Najdluższy punkt
        int zm2=0; //Najkrótszy punkt */

        for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x1
            for(int j=0 ; j<Rozmiar_macierz_glowna ; j++)
            {
                if((Punkty[(i*2)][j])>=zm1)
                    zm1=Punkty[(i*2)][j];
            }

        for(int i=1 ; i<3 ; i++) //Szukanie najdalszego punktu na osi x2
            for(int j=0 ; j<Rozmiar_macierz_glowna ; j++)
            {
                if((Punkty[((i*2)+1)][j])>=zm2)
                    zm2=Punkty[((i*2)+1)][j];
            }

        /* Zwiększanie zasięgu rysowania funkcji na wykresie*/
        zm1=zm1*2;
        zm2=zm2*2;


        /* Uzupełnianie punktami granicznymi na początku i końcu funkcji*/

        /* Punkty początkowe */

        for(int j=0 ; j<Rozmiar_macierz_glowna ; j++)
        {
            Punkty[0][j]=((-1)*(zm1)); //x
            Punkty[1][j]=(((-1)*((M[1][j])*(((-1)*(zm1)))))+M[0][j])/(M[2][j]); //y
        }

        /* Punkty końcowe */

        for(int j=0 ; j<Rozmiar_macierz_glowna ; j++)
        {
            Punkty[6][j]=zm1; //x
            Punkty[7][j]=((((-1)*(M[1][j])*(zm1)))+(M[0][j]))/(M[2][j]); //y
        }



        /* szukanie wspólnego */


            /* PRZYKŁAD
                QwtPlotCurve *curve2 = new QwtPlotCurve();
                curve2->setTitle( "Some Points" );
                curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );

                QPolygonF points2;
                points2 << QPointF( 0.0, 2.4 ) << QPointF( 1.0, 4.0 )
                       << QPointF( 2.0, 5.5 ) << QPointF( 3.0, 6.8 )
                       << QPointF( 4.0, 3.9 ) << QPointF( 5.0, 6.1 );
                curve2->setSamples( points2 );

                curve1->setPen(Qt::blue);
                curve2->attach( plot );
            */

        /* Teren wspólny */

        /* Wyznaczanie punktów terenu wspólnego */

        double Punkty_dol[2][100];       // ( x , y )
        double Punkty_gora[2][100];      // ( x , y )

        //Punkty_dol[0][0] = 1; Punkty_dol[1][0] = 2;     Punkty_gora[0][0] = 5; Punkty_gora[1][0] = 6;
        //Punkty_dol[0][1] = 3; Punkty_dol[1][1] = 4;     Punkty_gora[0][1] = 7; Punkty_gora[1][1] = 8;

        int wartosc_najnizsza = 0, wartosc_najwyzsza = 0, os_x1 = 0;
        int numer_rownania_wartosc_najnizsza = 0;
        int numer_rownania_wartosc_najwyzsza = 0;
        //((((-1)*((M[1][j])*os_x1))+(M[0][j]))/(M[2][j]))
        //   (>= 1)       (<= 0)

        for(int i=1; i<100 ; i++)
        {

            wartosc_najnizsza = 0;
            wartosc_najwyzsza = 1000000;
            os_x1=i*1;

            Punkty_dol[0][i-1]=os_x1;
            Punkty_dol[1][i-1]=0;


            Punkty_gora[0][i-1]=os_x1;
            Punkty_gora[1][i-1]=30;

            //Szukanie dolnego najwiekszego ograniczenia
            for(int j=0 ; j<Rozmiar_macierz_glowna ; j++)
            {
                if(znaki[j]==1) //Dla ograniczen dolnych najwieksze dolne ograniczenie
                {
                    cout<<"*1*";
                    if(((((-1)*((M[1][j])*os_x1))+(M[0][j]))/(M[2][j]))>0) //dolne ograniczenie jako 0   //NIEPEWNA CZESC KODU!!!!!!!!!!!!!!!!!!
                    {
                        if(((((-1)*((M[1][j])*os_x1))+(M[0][j]))/(M[2][j]))>wartosc_najnizsza) //Porownanie z innymi liniami
                        {
                            cout<<"*2*";
                            wartosc_najnizsza=((((-1)*((M[1][j])*os_x1))+(M[0][j]))/(M[2][j]));
                            Punkty_dol[0][i-1]=os_x1;
                            Punkty_dol[1][i-1]=wartosc_najnizsza;
                        }
                    }
                    else
                    {
                        cout<<"*3*";
                        //Punkty_dol[0][i-1]=os_x1; //NIEPEWNA CZESC KODU!!!!!!!!!!!!!!!!!!
                        //Punkty_dol[1][i-1]=0;
                    }
                }
            }

            for(int j=0 ; j<Rozmiar_macierz_glowna ; j++) //Dla ograniczen gornych sprawdzenie czy istnieje
            {
                if(znaki[j]==0)
                {
                    cout<<"*4*";
                    if(Punkty_dol[1][i-1]<=((((-1)*((M[1][j])*os_x1))+(M[0][j]))/(M[2][j])))
                    {
                    cout<<"*5*";
                    }
                    else
                    {
                        cout<<"*6*";
                        Punkty_dol[0][i-1]=os_x1;
                        Punkty_dol[1][i-1]=0;
                    }
                }
            }

        // Szukanie najmniejszego gornego ograniczenia

            for(int k=0 ; k<Rozmiar_macierz_glowna ; k++)
            {
                if(znaki[k]==0)
                {
                    if((((((-1)*((M[1][k])*os_x1))+(M[0][k]))/(M[2][k]))<=wartosc_najwyzsza)&&(((((-1)*((M[1][k])*os_x1))+(M[0][k]))/(M[2][k]))<=Punkty_gora[1][i-1]))
                    {
                        wartosc_najwyzsza = ((((-1)*((M[1][k])*os_x1))+(M[0][k]))/(M[2][k]));
                        Punkty_gora[0][i-1]=os_x1;
                        Punkty_gora[1][i-1]=wartosc_najwyzsza;
                    }
                    else
                    {
                        //juz podstawione ze sie rowna 20
                    }

                    if(((((-1)*((M[1][k])*os_x1))+(M[0][k]))/(M[2][k]))<0) //NIEPEWNA CZESC KODU !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    {
                        Punkty_gora[1][i-1]=0;
                    }
                }

            }

            //Sprawdzenie czy dla dolnych ograniczen jest spelnione

            for(int k=0; k<Rozmiar_macierz_glowna ; k++) //NIEPEWNA CZESC KODU !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            {
                if(znaki[k]==1)
                {
                    if(Punkty_gora[1][i-1]>((((-1)*((M[1][k])*os_x1))+(M[0][k]))/(M[2][k])))
                    {

                    }
                    else
                    {
                        //Punkty_gora[1][i-1]=0; // <-zmienic trzeba
                        Punkty_gora[1][i-1]=Punkty_dol[1][i-1];
                    }
                }
            }

        if(i<=10)
        cout<<endl<< Punkty_dol[0][i-1] <<" "<< Punkty_dol[1][i-1] <<" -> "<< Punkty_gora[0][i-1] <<" "<< Punkty_gora[1][i-1] <<endl;
        }

        /* Rysowanie wykresu terenu wspolnego */

        QwtPlotCurve *curve_wspolne = new QwtPlotCurve() ;
        curve_wspolne->setTitle( "Some Points" );
        curve_wspolne->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        /* [x][y]  [kolumny][wiersze] */

        QPolygonF points_wspolne;

        for(int j=0; j<10; j++)
        {
                points_wspolne << QPointF( Punkty_dol[0][j], Punkty_dol[1][j] )<< QPointF( Punkty_gora[0][j], Punkty_gora[1][j] ); //( x , y ) << ( x , y )
        }


        curve_wspolne->setSamples( points_wspolne );

        curve_wspolne->setPen(Qt::blue);
        curve_wspolne->attach( plot );




        //Pierwsza funkcja liniowa

        if(Rozmiar_macierz_glowna>=1)
        {
            QwtPlotCurve *curve1 = new QwtPlotCurve();
            curve1->setTitle( "Some Points" );
            curve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );

            QPolygonF points1;
            points1 << QPointF( Punkty[0][0], Punkty[1][0] ) << QPointF( Punkty[2][0], Punkty[3][0] )
                    << QPointF( Punkty[4][0], Punkty[5][0] ) << QPointF( Punkty[6][0], Punkty[7][0] );
            curve1->setSamples( points1 );

            //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

            curve1->setPen(Qt::blue);
            curve1->attach( plot );
        }


        //Druga funkcja liniowa

        if(Rozmiar_macierz_glowna>=2)
        {
            QwtPlotCurve *curve2 = new QwtPlotCurve();
            curve2->setTitle( "Some Points" );
            curve2->setRenderHint( QwtPlotItem::RenderAntialiased, true );

            QPolygonF points2;
            points2 << QPointF( Punkty[0][1], Punkty[1][1] ) << QPointF( Punkty[2][1], Punkty[3][1] )
                    << QPointF( Punkty[4][1], Punkty[5][1] ) << QPointF( Punkty[6][1], Punkty[7][1] );
            curve2->setSamples( points2 );

            //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

            curve2->setPen(Qt::red);
            curve2->attach( plot );
        }

        //Trzcia funkcja liniowa

        if(Rozmiar_macierz_glowna>=3)
        {
            QwtPlotCurve *curve3 = new QwtPlotCurve();
            curve3->setTitle( "Some Points" );
            curve3->setRenderHint( QwtPlotItem::RenderAntialiased, true );

            QPolygonF points3;
            points3 << QPointF( Punkty[0][2], Punkty[1][2] ) << QPointF( Punkty[2][2], Punkty[3][2] )
                    << QPointF( Punkty[4][2], Punkty[5][2] ) << QPointF( Punkty[6][2], Punkty[7][2] );
            curve3->setSamples( points3 );

            //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

            curve3->setPen(Qt::green);
            curve3->attach( plot );
        }

        //Czwarta funkcja liniowa

        if(Rozmiar_macierz_glowna>=4)
        {
            QwtPlotCurve *curve4 = new QwtPlotCurve();
            curve4->setTitle( "Some Points" );
            curve4->setRenderHint( QwtPlotItem::RenderAntialiased, true );

            QPolygonF points4;
            points4 << QPointF( Punkty[0][3], Punkty[1][3] ) << QPointF( Punkty[2][3], Punkty[3][3] )
                    << QPointF( Punkty[4][3], Punkty[5][3] ) << QPointF( Punkty[6][3], Punkty[7][3] );
            curve4->setSamples( points4 );

            //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

            curve4->setPen(Qt::yellow);
            curve4->attach( plot );
        }

        //Piąta funkcja liniowa

        if(Rozmiar_macierz_glowna>=5)
        {
            QwtPlotCurve *curve5 = new QwtPlotCurve();
            curve5->setTitle( "Some Points" );
            curve5->setRenderHint( QwtPlotItem::RenderAntialiased, true );

            QPolygonF points5;
            points5 << QPointF( Punkty[0][4], Punkty[1][4] ) << QPointF( Punkty[2][4], Punkty[3][4] )
                    << QPointF( Punkty[4][4], Punkty[5][4] ) << QPointF( Punkty[6][4], Punkty[7][4] );
            curve5->setSamples( points5 );

            //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

            curve5->setPen(Qt::black);
            curve5->attach( plot );
        }

        //Szósta funkcja liniowa

        if(Rozmiar_macierz_glowna>=6)
        {
            QwtPlotCurve *curve6 = new QwtPlotCurve();
            curve6->setTitle( "Some Points" );
            curve6->setRenderHint( QwtPlotItem::RenderAntialiased, true );

            QPolygonF points6;
            points6 << QPointF( Punkty[0][5], Punkty[1][5] ) << QPointF( Punkty[2][5], Punkty[3][5] )
                    << QPointF( Punkty[4][5], Punkty[5][5] ) << QPointF( Punkty[6][5], Punkty[7][5] );
            curve6->setSamples( points6 );

            //double tabx[2] = {0.3, 4.3}; //double taby[2] = {14.0, 18.0}; //curve->setRawSamples(tabx, taby, 2);

            curve6->setPen(Qt::red);
            curve6->attach( plot );
        }



}

rysunek::~rysunek()
{
    delete ui;
}
