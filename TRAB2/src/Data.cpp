#include "../include/Data.hpp"
#include <stdlib.h>

Data::Data(char* filePath)
{
    FILE* f = fopen(filePath, "r");

    if(!f)
    {
        printf("Problem while reading instance %s.\n", filePath);
        exit(1);
    }

    if(fscanf(f, "%d", &n_periodos) != 1)
    {
        printf("Problem while reading instance %s.\n", filePath);
        exit(1);
    }

    if(fscanf(f, "%d", &n_usinas) != 1)
    {
        printf("Problem while reading instance %s.\n", filePath);
        exit(1);
    }

    //lendo unidades por usinas
    n_unidades = std::vector<int>(n_usinas,0);
    for(int i = 0; i < n_usinas; i++)
    {
        if(fscanf(f, "%d", &n_unidades[i]) != 1)
        {
            printf("Problem while reading instance %s.\n", filePath);
            exit(1);
        }
    }

    //lendo duracao e demanda dos periodos
    duracao = std::vector<int>(n_periodos, 0);
    demanda = std::vector<double>(n_periodos, 0);
    for(int i = 0; i < n_periodos; i++)
    {
        if(fscanf(f, "%d", &duracao[i]) != 1)
        {
            printf("Problem while reading instance.\n");
            exit(1);
        }
        if(fscanf(f, "%lf", &demanda[i]) != 1)
        {
            printf("Problem while reading instance.\n");
            exit(1);
        }
    }

    //lendo prod min e prod max das usinas
    prod_min = std::vector<double>(n_usinas, 0);
    prod_max = std::vector<double>(n_usinas, 0);
    for(int i = 0; i < n_usinas; i++)
    {
        if(fscanf(f, "%lf", &prod_min[i]) != 1)
        {
            printf("Problem while reading instance.\n");
            exit(1);
        }
        if(fscanf(f, "%lf", &prod_max[i]) != 1)
        {
            printf("Problem while reading instance.\n");
            exit(1);
        }
    }

    //lendo custos das usinas
    custo_prod_min = std::vector<double>(n_usinas, 0);
    custo_adicional = std::vector<double>(n_usinas, 0);
    custo_ligacao = std::vector<double>(n_usinas, 0);
    for(int i = 0; i < n_usinas; i++)
    {
        if(fscanf(f, "%lf", &custo_prod_min[i]) != 1)
        {
            printf("Problem while reading instance.\n");
            exit(1);
        }

        if(fscanf(f, "%lf", &custo_adicional[i]) != 1)
        {
            printf("Problem while reading instance.\n");
            exit(1);
        }

        if(fscanf(f, "%lf", &custo_ligacao[i]) != 1)
        {
            printf("Problem while reading instance.\n");
            exit(1);
        }
    }

    printf("leu\n");

    /*for(int k = 0; k < n_periodos; k++)
    {
        printf("periodo %d:\nduracao %d e demanda %d\n", k+1, duracao[k], demanda[k]);
    }

    for(int i = 0; i < n_usinas; i++)
    {

        printf("\nusina %d:\n", i+1);

        printf("unidades disponiveis = %d\n", n_unidades[i]);
        
        printf("prod min = %d\nprod max = %d\n", prod_min[i], prod_max[i]);

        printf("custo prod min = %d$/h\ncusto adicional = %d$/h\ncusto de ligacao = %d$\n", custo_prod_min[i], custo_adicional[i], custo_ligacao[i]);
        
    }*/

    fclose(f);
}

int Data::getNPeriodos()
{
    return n_periodos;
}

int Data::getNUsinas()
{
    return n_usinas;
}

int Data::getNUnidadesUsina(int usina)
{
    return n_unidades[usina];
}

int Data::getDuracaoPeriodo(int periodo)
{
    return duracao[periodo];
}

double Data::getDemandaPeriodo(int periodo)
{
    return demanda[periodo];
}

double Data::getProdMinUsina(int usina)
{
    return prod_min[usina];
}

double Data::getProdMaxUsina(int usina)
{
    return prod_max[usina];
}

double Data::getCustoProdMinUsina(int usina)
{
    return custo_prod_min[usina];
}

double Data::getCustoAdicionalUsina(int usina)
{
    return custo_adicional[usina];
}

double Data::getCustoLigacaoUsina(int usina)
{
    return custo_ligacao[usina];
}