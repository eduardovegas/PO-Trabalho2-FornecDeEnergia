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
            IloNumVarArray vetor(env, data.getNPeriodos(), 0, data.getProdMaxUsina(i));
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

    //fim das variaveis
    ///////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //Criando a Função Objetivo (FO) 
    IloExpr sumCL(env);
    IloExpr sumCP(env);
    for(int i = 0; i < data.getNUsinas(); i++)
    {
        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                sumCL += data.getCustoLigacaoUsina(i)*z[i][j][k];
            }
        }
    }

    for(int i = 0; i < data.getNUsinas(); i++)
    {
        int CPMin = data.getCustoProdMinUsina(i);
        int PMin = data.getProdMinUsina(i);
        int Cadi = data.getCustoAdicionalUsina(i);

        for(int j = 0; j < data.getNUnidadesUsina(i); j++)
        {
            for(int k = 0; k < data.getNPeriodos(); k++)
            {
                sumCP += (CPMin +(p[i][j][k] - PMin)*Cadi)*x[i][j][k]*data.getDuracaoPeriodo(k);
            }
        }
    }

    IloExpr OBJ(env);
    OBJ += sumCL + sumCP;
    // Adicionando a FO
    modelo.add(IloMinimize(env, OBJ));
    //////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //Restricoes

    //Restricoes (1): determina se a usina esta ligada ou desligada a partir de sua producao
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

    //Restricoes (3): se uma usina foi ligada no periodo k(unico caso possivel: 0 1)
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
    std::cout << "\nCusto diario minimo: " << trab.getObjValue() << std::endl;
    std::cout << "Custo diario por hora: " << trab.getObjValue()/24.0 << std::endl;

    puts("");
    for(int k = 0; k < data.getNPeriodos(); k++) //Exibe quantas unidades das usinas estao ligadas no periodo k
    {
        printf("/------------------ PERIODO %d ------------------/\n\n", k+1);
        for(int i = 0; i < data.getNUsinas(); i++)
        {
            int aux = 0;

            for(int j = 0; j < data.getNUnidadesUsina(i); j++)
            {
                if(trab.getValue(x[i][j][k]) > 0)
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
                if(trab.getValue(p[i][j][k]) > 0)
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
                aux += trab.getValue(x[i][j][k]) * data.getCustoProdMinUsina(i) * data.getDuracaoPeriodo(k) ;
                aux2 += trab.getValue(x[i][j][k]) * data.getCustoAdicionalUsina(i) * data.getDuracaoPeriodo(k) * (trab.getValue(p[i][j][k]) - data.getProdMinUsina(i));
                aux3 += trab.getValue(z[i][j][k]) * data.getCustoLigacaoUsina(i);
            }
        }

        printf("USINA TIPO #%d:\n", i+1);
        std::cout << "Custo da produção minima = " << aux << std::endl;
        std::cout << "Custo adicional = " << aux2 << std::endl;
        std::cout << "Custo de ligação = " << aux3 << std::endl;

        puts("");

    }

}

