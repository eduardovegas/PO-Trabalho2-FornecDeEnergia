#include "../include/Data.hpp"
#include <stdio.h>
#include <iostream>
#include <ilcplex/ilocplex.h>

void solve(Data& data);

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Usage:\n./bin instance\n");
        return 0;
    }

    Data data(argv[1]);
    solve(data);

    return 0;
}

void solve(Data& data)
{
    IloEnv env;
    IloModel modelo(env);


    ///////////////////////////////////////////////////////
    //variavel Xijk: Xijk = 1, se a unidade j do tipo i esta ligada no periodo k
    IloArray <IloArray <IloBoolVarArray>> x(env, data.getNUsinas());
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        IloArray <IloBoolVarArray> vetAux(env, data.getNUnidadesUsina(i));
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            IloBoolVarArray vetor(env, data.getNPeriodos());
            vetAux[j] = vetor;
        }

        x[i] = vetAux;
    }

    //adiciona a variavel Xijk ao modelo
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                char name[100];
                sprintf(name, "X(%d)(%d)(%d)", i+1, j+1, k+1);
                x[i][j][k].setName(name);
                modelo.add(x[i][j][k]);
            }
        }
    }

    //variavel Zijk: Zijk = 1, se a unidade j do tipo i foi ligada no periodo atual
    IloArray <IloArray <IloBoolVarArray>> z(env, data.getNUsinas());
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        IloArray <IloBoolVarArray> vetAux(env, data.getNUnidadesUsina(i));
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            IloBoolVarArray vetor(env, data.getNPeriodos());
            vetAux[j] = vetor;
        }

        z[i] = vetAux;
    }
    
    //adiciona a variavel Zijk ao modelo
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                char name[100];
                sprintf(name, "Z(%d)(%d)(%d)", i+1, j+1, k+1);
                z[i][j][k].setName(name);
                modelo.add(z[i][j][k]);
            }
        }
    }

    //variavel Pijk: Pijk representando a produção total de cada unidade de usina em determinado periodo
    IloArray <IloArray <IloNumVarArray>> p(env, data.getNUsinas());
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        IloArray <IloNumVarArray> vetAux(env, data.getNUnidadesUsina(i));
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            IloNumVarArray vetor(env, data.getNPeriodos(), 0, IloInfinity);
            /*for(int k = 0; k < data.getNPeriodos; k++)
            {
                vetor[k].setBounds(0, data.getProdMaxUsina(i));
            }*/
            vetAux[j] = vetor;
        }

        p[i] = vetAux;
    }

    //adiciona a variavel Pijk ao modelo
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                char name[100];
                sprintf(name, "P(%d)(%d)(%d)", i+1, j+1, k+1);
                p[i][j][k].setName(name);
                modelo.add(p[i][j][k]);
            }
        }
    }

    //variavel Qijk: Qijk representando o controle da produção da variável Pijk, deixando a FO linear
    IloArray <IloArray <IloNumVarArray>> q(env, data.getNUsinas());
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        IloArray <IloNumVarArray> vetAux(env, data.getNUnidadesUsina(i));
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            IloNumVarArray vetor(env, data.getNPeriodos(), 0, IloInfinity);
            vetAux[j] = vetor;
        }

        q[i] = vetAux;
    }

    //adiciona a variavel Qijk ao modelo
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                char name[100];
                sprintf(name, "Q(%d)(%d)(%d)", i+1, j+1, k+1);
                q[i][j][k].setName(name);
                modelo.add(q[i][j][k]);
            }
        }
    }

    //variavel Fij: Fij representando o controle custo de ligação do primeiro dia
    IloArray <IloBoolVarArray> f(env, data.getNUsinas());
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        IloBoolVarArray vetor(env, data.getNUnidadesUsina(i));
        f[i] = vetor;
    }

    //adiciona a variavel Fij ao modelo
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            char name[100];
            sprintf(name, "F(%d)(%d)", i+1, j+1);
            f[i][j].setName(name);
            modelo.add(f[i][j]);
        }
    }

    //fim das variaveis
    ///////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //Criando a Função Objetivo (FO) 
    IloExpr OBJ(env);

    for(int i = 0; i < data.getNUsinas(); i++)
    {
        int CPMin = data.getCustoProdMinUsina(i);
        int Cadi = data.getCustoAdicionalUsina(i);
        int CLig = data.getCustoLigacaoUsina(i);

        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {

            OBJ += CLig*f[i][j];

            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                OBJ += (CPMin*x[i][j][k] + q[i][j][k]*Cadi)*data.getDuracaoPeriodo(k) + CLig*z[i][j][k];
            }
        }
    }

    // Adicionando a FO
    modelo.add(IloMinimize(env, OBJ));
    //////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //Restricoes

    //Restricoes (1.1): força a usina a produzir pelo menos o mínimo se ela for ligada (1.2): força a ligação de uma usina se está produzindo, e também limita o máximo
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                IloRange r = (p[i][j][k] - (data.getProdMinUsina(i)*x[i][j][k]) >= 0);
                char name[100];
                sprintf(name, "ONOFF(%d)(%d)(%d)", i+1, j+1, k+1);
                r.setName(name);
                modelo.add(r);

                IloRange r2 = (data.getProdMaxUsina(i)*x[i][j][k] - p[i][j][k] >= 0);
                char name2[100];
                sprintf(name2, "ONOFF2(%d)(%d)(%d)", i+1, j+1, k+1);
                r2.setName(name2);
                modelo.add(r2);
            }
        }
    }

    //Restricoes (2): a demanda de cada periodo deve ser satisfeita
    for(int k = 0; k < data.getNPeriodos(); k++)
    {
        IloExpr sumProd(env);

        for(int i = 0; i < data.getNUsinas(); i++)
        {
            for(int j = 0; j < data.getNUnidadesUsina(i); j++)
            {
                sumProd += p[i][j][k];
            }
        }

        IloRange r = (sumProd - data.getDemandaPeriodo(k) >= 0);
        char name[100];
        sprintf(name, "DEM(%d)", k+1);
        r.setName(name);
        modelo.add(r);
    }

    //Restricoes (3): controle da produção para linearizar a FO
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                IloRange r = (q[i][j][k] - (p[i][j][k] - data.getProdMinUsina(i)) >= 0);
                char name[100];
                sprintf(name, "RESQ(%d)(%d)(%d)", i+1, j+1, k+1);
                r.setName(name);
                modelo.add(r);
            }
        }
    }

    //Restricoes (4): se uma usina foi ligada no periodo k(unico caso possivel: 0 1)
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                if(k == data.getNPeriodos()-1) //no ultimo periodo, k+1 se refere ao primeiro
                {
                    IloRange r = (z[i][j][0] - (x[i][j][0] - x[i][j][k]) >= 0);
                    char name[100];
                    sprintf(name, "LIG(%d)(%d)(%d)", i+1, j+1, k+1);
                    r.setName(name);
                    modelo.add(r);
                }
                else
                {
                    IloRange r = (z[i][j][k+1] - (x[i][j][k+1] - x[i][j][k]) >= 0);
                    char name[100];
                    sprintf(name, "LIG(%d)(%d)(%d)", i+1, j+1, k+1);
                    r.setName(name);
                    modelo.add(r); 
                }
            }
        }
    }

    //Restricoes (5): controle do custo de ligação do primeiro dia
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            IloRange r = (f[i][j] - (x[i][j][0] - z[i][j][0]) >= 0);
            char name[100];
            sprintf(name, "PDIA(%d)(%d)", i+1, j+1);
            r.setName(name);
            modelo.add(r);
        }
    }

    
    //fim das restricoes
    ////////////////////////////////////////////////////////*/

    //resolve o modelo
    IloCplex trab(modelo);
    trab.setParam(IloCplex::TiLim, 2*60*60);
    trab.setParam(IloCplex::Threads, 1);
//    trab.setParam(IloCplex::Param::MIP::Interval, 1);
//    trab.setParam(IloCplex::Param::MIP::Display, 5);
    trab.exportModel("modelo.lp");

    try
    {
        trab.solve();
    }
    catch(IloException& e)
    {
        std::cout << e;
    }

    std::cout << "\nstatus: " << trab.getStatus() << std::endl;
    //std::cout << "\nCusto diario minimo: " << trab.getObjValue() << std::endl;
    //std::cout << "Custo diario por hora: " << trab.getObjValue()/24.0 << std::endl;
    printf("\nCusto diario minimo: %.1lf$\n", trab.getObjValue());
    printf("Custo diario por hora: %.1lf$\n", trab.getObjValue()/24.0);

    puts("");
    for(int k = 0; k < data.getNPeriodos(); k++) //Exibe quantas unidades das usinas estao ligadas no periodo k
    {
        printf("/------------------ PERIODO %d ------------------/\n\n", k+1);
        for(int i = 0; i < data.getNUsinas(); i++)
        {
            int aux = 0;

            for(int j = 0; j < data.getNUnidadesUsina(i); j++)
            {
                if(trab.getValue(x[i][j][k]) > 0.0)
                {
                    aux++;
                }
            }

            printf("     %d unidades da usina do tipo #%d ligadas\n", aux, i+1);
            
            if(i == data.getNUsinas()-1)
            {
                puts("");
            }
        }
        
    }

    puts("X=================================================X\n");
    
    for(int k = 0; k < data.getNPeriodos(); k++) //Exibe o quanto cada unidade de usina vai produzir no periodo k
    {
        printf("/------------------ PERIODO %d ------------------/\n\n", k+1);
        
        for(int i = 0; i < data.getNUsinas(); i++)
        {
            for(int j = 0; j < data.getNUnidadesUsina(i); j++)
            {
                if(trab.getValue(p[i][j][k]) > 0.000000)
                {
                    printf(" Usina do tipo #%d unidade #%d irá produzir: ", i+1, j+1);
                    std::cout << trab.getValue(p[i][j][k]) << "MW" << std::endl;
                }
            }
            
            if(i != data.getNUsinas() - 1)
            {
                puts("");
            }
        }

        puts("");
    }

    puts("X================================================X\n");

    for(int i = 0; i < data.getNUsinas(); i++) //Exibe os custos diarios de cada usina
    {
        double aux = 0;
        double aux2 = 0;
        double aux3 = 0;

        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++) 
            {
                aux += trab.getValue(x[i][j][k]) * data.getCustoProdMinUsina(i) * data.getDuracaoPeriodo(k);

                if(trab.getValue(q[i][j][k]) > 0.000000)
                {
                    aux2 += trab.getValue(q[i][j][k]) * data.getCustoAdicionalUsina(i) * data.getDuracaoPeriodo(k);
                }

                aux3 += trab.getValue(z[i][j][k]) * data.getCustoLigacaoUsina(i);
            }
        }

        printf("USINA TIPO #%d:\n", i+1);
        printf("Custo adicional = %.1lf$\n", aux2);
        printf("Custo da produção minima = %.1lf$\n", aux);
        printf("Custo de ligação = %.1lf$\n", aux3);

        puts("");

    }

}