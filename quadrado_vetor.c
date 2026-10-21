/*
Ana Julia Yaguti Matilha
Carolina Lee
Pedro Casas Pequeno Junior
*/

/*
O desafio é:
1. Inicializar um vetor de inteiros de tamanho fixo (`DATA_SIZE = 100`) contendo valores de `1` a `100`.
2. Distribuir esse vetor igualmente entre **5 processos MPI**.
3. Cada processo aplica a **função de transformação** localmente:  
   - \( f(x) = x \cdot x \) (ou seja, o quadrado de cada elemento).
4. Após a transformação, reunir os resultados com `MPI_Gather` para reconstruir o vetor final transformado no processo mestre (rank 0).
5. O processo mestre exibe o vetor final transformado.
---
- **DATA_SIZE = 100**: tamanho total do vetor de dados.
- **Número de processos = 5**
- Cada processo ficará responsável por exatamente **20 elementos**.
- Os dados são números inteiros no intervalo `[1, 100]`.
---
1. O programa deve ser executado com exatamente 5 processos.
2. O vetor original deve ser criado **somente no processo de rank 0**, contendo os valores de 1 a 100.
3. Utilizar `MPI_Scatter` para dividir o vetor entre os processos.
4. Cada processo aplica localmente a transformação:  
   - \( x 
ightarrow x^2 \)
5. Os resultados devem ser reunidos no processo 0 com `MPI_Gather`.
6. O processo 0 deve imprimir:
   - O vetor original (antes da transformação)
   - O vetor transformado (com os quadrados dos valores)

*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define processadores_num 5
#define N 100

int main(int argc, char *argv[]) {
    int i;
    int rank;
    int total_processos; 
    int *data = NULL;              // Ponteiro para o vetor completo (só usado pelo root)
    int *local_data;               // Vetor local com parte dos dados em cada processo
    int *resultado;                // Vetor para o resultado 

    MPI_Init(&argc, &argv);

    // Obtém o rank (identificador) do processo atual
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtém o número total de processos em execução
    MPI_Comm_size(MPI_COMM_WORLD, &total_processos);

    resultado = (int *)malloc(N * sizeof(int));
    // validação dos processos
    if (total_processos != processadores_num) {
        if (rank == 0) {
            printf("ERRO: Este código foi projetado para rodar com numero exato de processo");
            printf("Execute usando: mpirun -np %d ./programa\n", processadores_num);
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }


    // Cada processo irá receber uma parte igual do vetor
    int chunk_tam = N / processadores_num;

    // Apenas o processo 0 (root) aloca e preenche o vetor com valores de 1 a 100
    if (rank == 0) {
        data = (int *)malloc(N * sizeof(int));
        for (i = 0; i < N; i++) {
            data[i] = i + 1;
        }
    }

    // Todos os processos alocam memória para seu pedaço do vetor
    local_data = (int *)malloc(chunk_tam * sizeof(int));

    // Scatter: distribui pedaços do vetor do root para todos os processos
    MPI_Scatter(data, chunk_tam, MPI_INT, local_data, chunk_tam, MPI_INT, 0, MPI_COMM_WORLD);
    

    for (i = 0; i < chunk_tam; i++) {
        local_data[i] = local_data[i]*local_data[i];
    }

    // Gather para reunior os vetores no processo mestre
    MPI_Gather(local_data, chunk_tam, MPI_INT, resultado, chunk_tam, MPI_INT, 0, MPI_COMM_WORLD);

    // Apenas o processo root (0) imprime o vetor final
    if (rank == 0) {
        printf("Processo %d] Vetor original: ", rank);
        for (i = 0; i < N; i++) {
            printf(" %d", data[i]);
        }
        printf("\n");
        printf("\n");
        printf("[Processo %d] Vetor transformado:", rank);
        for (i = 0; i < N; i++) {
            printf(" %d", resultado[i]);
        }
        printf("\n");
        //printf("\n[Processo %d] Vetor original: %d\n", rank, *data);
        //printf("[Processo %d] Vetor transformado: %d\n", rank, *resultado);
        free(data);
        free(resultado);
    }
   
    // Libera a memória do vetor local
    free(local_data);

    // Finaliza o ambiente MPI
    MPI_Finalize();
    return 0;
}
