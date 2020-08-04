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
        std::vector<int> demanda;
        std::vector<int> prod_min;
        std::vector<int> prod_max;
        std::vector<int> custo_prod_min;
        std::vector<int> custo_adicional;
        std::vector<int> custo_ligacao;

    public:

        Data(char* filePath);
        
        int getNPeriodos();
        int getNUsinas();
        int getNUnidadesUsina(int usina);
        int getDuracaoPeriodo(int periodo);
        int getDemandaPeriodo(int periodo);
        int getProdMinUsina(int usina);
        int getProdMaxUsina(int usina);
        int getCustoProdMinUsina(int usina);
        int getCustoAdicionalUsina(int usina);
        int getCustoLigacaoUsina(int usina);

};

#endif

