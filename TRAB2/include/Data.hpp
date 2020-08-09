#ifndef _DATA_H_
#define _DATA_H_

#include <vector>
#include <stdio.h>

class Data
{
    private:

        int n_periodos;
        int n_usinas;
        std::vector<int> n_unidades;

        std::vector<int> duracao;
        std::vector<double> demanda;
        std::vector<double> prod_min;
        std::vector<double> prod_max;
        std::vector<double> custo_prod_min;
        std::vector<double> custo_adicional;
        std::vector<double> custo_ligacao;

    public:

        Data(char* filePath);
        
        int getNPeriodos();
        int getNUsinas();
        int getNUnidadesUsina(int usina);
        int getDuracaoPeriodo(int periodo);
        double getDemandaPeriodo(int periodo);
        double getProdMinUsina(int usina);
        double getProdMaxUsina(int usina);
        double getCustoProdMinUsina(int usina);
        double getCustoAdicionalUsina(int usina);
        double getCustoLigacaoUsina(int usina);

};

#endif

