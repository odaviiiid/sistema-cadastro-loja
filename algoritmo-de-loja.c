#include <stdio.h>   // printf, fgets, fopen, fprintf, fclose, remove, rename
#include <string.h>  // strlen, strcpy, strcspn
#include <ctype.h>   // tolower (usado nas comparacoes sem diferenciar maiusculas)

#define MAX 100                    // maximo de produtos no cadastro
#define ARQUIVO "produtos.txt"     // arquivo oficial dos dados
#define ARQUIVO_TEMP "temp.txt"    // arquivo temporario (alterar e excluir)
#define ARQUIVO_CSV "produtos.csv" // arquivo da exportacao em CSV

// Estrutura que representa um produto (uma linha da tabela)
typedef struct {
    int id;              // identificador gerado automaticamente
    char produto[50];    // nome do produto
    char categoria[30];  // categoria do produto
    float preco;         // preco em reais
    int quantidade;      // quantidade em estoque
    char fornecedor[50]; // nome do fornecedor
} Produto;

void lerTexto(const char *mensagem, char destino[], int tamanho);
int lerInteiro(const char *mensagem);
float lerDecimal(const char *mensagem);

int carregarProdutos(Produto lista[]);                          // le o arquivo e devolve quantos produtos leu
int salvarProdutos(const char *caminho, Produto lista[], int total); // grava o vetor no caminho (1 = ok, 0 = erro)
int compararTexto(const char *a, const char *b);                // como strcmp, sem diferenciar maiusculas
int contemTexto(const char *texto, const char *trecho);         // 1 se "texto" contem "trecho" (sem diferenciar maiusculas)
int textoValido(const char *texto);                              // 1 se tem conteudo e NAO tem ';' nem ',' (protege o arquivo e o CSV)
void buscarPorNome(Produto lista[], int total);                 // pergunta o termo e mostra os encontrados
void buscarPorCategoria(Produto lista[], int total);            // pergunta o termo e mostra os encontrados

void criarArquivo(void);                                        // cria produtos.txt vazio e pede o cadastro dos produtos
void exibirTabela(Produto lista[], int total);                  // mostra o vetor em formato de tabela
void exportarCSV(Produto lista[], int total);                   // grava produtos.csv separado por virgula

int proximoId(Produto lista[], int total);                      // MAIOR id existente + 1 (nunca "total + 1")
int nomeDuplicado(Produto lista[], int total, const char *nome);// 1 se ja existe produto com esse nome
int idDuplicado(Produto lista[], int total, int id);            // 1 se ja existe produto com esse id
void inserirProduto(Produto lista[], int *total);               // pede os dados, valida, insere e grava

int substituirArquivo(Produto lista[], int total);               // auxiliar: grava em temp.txt e substitui o original
void alterarProduto(Produto lista[], int total);                // altera um produto pelo id e grava
void excluirProduto(Produto lista[], int *total);               // exclui um produto pelo id e grava

void trocar(Produto *a, Produto *b);                            // troca dois produtos usando ponteiros
void ordenarPorNome(Produto lista[], int total, int crescente); // 1 = A-Z, 0 = Z-A (mostra, nao altera o arquivo)
void ordenarPorPreco(Produto lista[], int total, int crescente);// 1 = menor-maior, 0 = maior-menor (mostra, nao altera o arquivo)

int main() {
    Produto lista[MAX]; // vetor de estruturas que guarda os produtos em memoria
    int total;          // quantidade de produtos carregados do arquivo
    int opcao;          // opcao escolhida no menu

    do {
        // Exibe o menu principal
        printf("\n======================================\n");
        printf("CADASTRO DE LOJA\n");
        printf("\n 0 - Sair\n 1 - Criar arquivo\n 2 - Listar produtos\n 3 - Inserir produto\n");
        printf(" 4 - Alterar produto\n 5 - Excluir produto\n 6 - Buscar por nome\n");
        printf(" 7 - Buscar por categoria\n 8 - Ordenar por nome (A-Z)\n 9 - Ordenar por nome (Z-A)\n");
        printf("10 - Ordenar por preco (menor -> maior)\n11 - Ordenar por preco (maior -> menor)\n");
        printf("12 - Exportar para CSV\n");
        printf("======================================\n");
        opcao = lerInteiro("Escolha uma opcao: ");

        // O arquivo e a fonte oficial: recarrega o vetor antes de cada acao
        total = carregarProdutos(lista);

        switch (opcao) {
            case 1:  criarArquivo(); break;
            case 2:  exibirTabela(lista, total); break;
            case 3:  inserirProduto(lista, &total); break;       // &total: a funcao atualiza o total
            case 4:  alterarProduto(lista, total); break;
            case 5:  excluirProduto(lista, &total); break;       // &total: a funcao atualiza o total
            case 6:  buscarPorNome(lista, total); break;
            case 7:  buscarPorCategoria(lista, total); break;
            case 8:  ordenarPorNome(lista, total, 1); break;
            case 9:  ordenarPorNome(lista, total, 0); break;
            case 10: ordenarPorPreco(lista, total, 1); break;
            case 11: ordenarPorPreco(lista, total, 0); break;
            case 12: exportarCSV(lista, total); break;
            case 0:  printf("Programa finalizado!\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 0); // repete ate o usuario escolher sair

    return 0;
}

// Le uma linha de texto (aceita espacos) e remove o '\n' que o fgets guarda
void lerTexto(const char *mensagem, char destino[], int tamanho) {
    printf("%s", mensagem);                           // mostra a pergunta
    if (fgets(destino, tamanho, stdin) == NULL) {     // le a linha, respeitando o tamanho do vetor
        destino[0] = '\0';                            // fim da entrada: devolve texto vazio
        return;
    }
    if (strchr(destino, '\n') == NULL) {              // sem '\n': o usuario digitou mais do que cabe no vetor
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }  // descarta o excesso para nao vazar para a proxima leitura
    }
    destino[strcspn(destino, "\n")] = '\0';           // troca o '\n' pelo terminador de string
}

// Le um numero inteiro, repetindo a pergunta ate o usuario digitar um numero valido
int lerInteiro(const char *mensagem) {
    char linha[50];                                   // guarda a linha digitada
    int valor;
    while (1) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof(linha), stdin) == NULL) return 0; // fim da entrada
        if (sscanf(linha, "%d", &valor) == 1) return valor;       // conseguiu converter: devolve
        printf("Valor invalido! Digite um numero inteiro.\n");
    }
}

// Le um numero decimal (use ponto: 49.90), repetindo ate ser valido
float lerDecimal(const char *mensagem) {
    char linha[50];
    float valor;
    while (1) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof(linha), stdin) == NULL) return 0;
        if (sscanf(linha, "%f", &valor) == 1) return valor;
        printf("Valor invalido! Digite um numero (ex.: 49.90).\n");
    }
}

// Le o arquivo produtos.txt para o vetor e devolve quantos produtos leu
int carregarProdutos(Produto lista[]) {
    FILE *arq = fopen(ARQUIVO, "r");                  // abre para leitura
    char linha[300];                                  // guarda uma linha do arquivo
    int total = 0;                                    // contador de produtos lidos
    if (arq == NULL) return 0;                        // arquivo ainda nao existe: nenhum produto
    while (total < MAX && fgets(linha, sizeof(linha), arq) != NULL) { // le linha a linha ate o limite MAX
        Produto p;
        // separa os 6 campos pelo ';' (%[^;] le texto ate o proximo ';')
        int campos = sscanf(linha, "%d;%49[^;];%29[^;];%f;%d;%49[^;\r\n]",
                            &p.id, p.produto, p.categoria, &p.preco, &p.quantidade, p.fornecedor);
        if (campos == 6) {                            // so aceita linhas completas
            lista[total] = p;                         // copia a struct para o vetor
            total++;                                  // conta o produto
        }
    }
    fclose(arq);                                      // fecha o arquivo
    return total;                                     // devolve a quantidade lida
}

// Grava o vetor inteiro no caminho informado (1 = ok, 0 = erro)
int salvarProdutos(const char *caminho, Produto lista[], int total) {
    FILE *arq = fopen(caminho, "w");                  // abre para escrita (apaga o conteudo anterior)
    if (arq == NULL) return 0;                        // nao conseguiu abrir
    for (int i = 0; i < total; i++) {                 // percorre todos os produtos
        fprintf(arq, "%d;%s;%s;%.2f;%d;%s\n", lista[i].id, lista[i].produto,
                lista[i].categoria, lista[i].preco, lista[i].quantidade, lista[i].fornecedor);
    }
    if (fclose(arq) != 0) return 0;                   // fclose pode falhar ao descarregar o buffer
    return 1;                                         // gravou com sucesso
}

// Compara dois textos sem diferenciar maiusculas (<0, 0, >0 como strcmp)
int compararTexto(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {                // anda enquanto nenhum terminou
        int ca = tolower((unsigned char)*a);          // minuscula do caractere de a
        int cb = tolower((unsigned char)*b);          // minuscula do caractere de b
        if (ca != cb) return ca - cb;                 // diferentes: devolve a diferenca
        a++;                                          // avanca o ponteiro de a
        b++;                                          // avanca o ponteiro de b
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b); // um terminou: decide pelo tamanho
}

// Devolve 1 se "texto" contem "trecho", sem diferenciar maiusculas
int contemTexto(const char *texto, const char *trecho) {
    size_t n = strlen(trecho);                        // tamanho do trecho procurado
    if (n == 0) return 1;                             // trecho vazio sempre "esta contido"
    for (; *texto != '\0'; texto++) {                 // tenta casar a partir de cada posicao
        size_t i = 0;
        while (i < n && texto[i] != '\0' &&
               tolower((unsigned char)texto[i]) == tolower((unsigned char)trecho[i])) {
            i++;                                      // caractere igual: continua comparando
        }
        if (i == n) return 1;                         // casou o trecho inteiro
    }
    return 0;                                         // nao encontrou
}

// 1 se o texto tem conteudo (nao so espacos) e nao tem ';' nem ','
int textoValido(const char *texto) {
    int temConteudo = 0;                              // marca se achou algo alem de espacos
    for (; *texto != '\0'; texto++) {
        if (*texto == ';' || *texto == ',') return 0; // separadores quebrariam o arquivo/CSV
        if (!isspace((unsigned char)*texto)) temConteudo = 1;
    }
    return temConteudo;
}

// Busca produtos cujo nome contem o termo digitado
void buscarPorNome(Produto lista[], int total) {
    char termo[50];
    Produto achados[MAX];                             // copia dos produtos encontrados
    int n = 0;
    lerTexto("Nome (ou parte do nome): ", termo, sizeof(termo));
    for (int i = 0; i < total; i++) {
        if (contemTexto(lista[i].produto, termo)) achados[n++] = lista[i]; // guarda o que combina
    }
    if (n == 0) printf("Nenhum produto encontrado.\n");
    else exibirTabela(achados, n);                    // mostra o resultado em tabela
}

// Busca produtos cuja categoria contem o termo digitado
void buscarPorCategoria(Produto lista[], int total) {
    char termo[30];
    Produto achados[MAX];
    int n = 0;
    lerTexto("Categoria (ou parte): ", termo, sizeof(termo));
    for (int i = 0; i < total; i++) {
        if (contemTexto(lista[i].categoria, termo)) achados[n++] = lista[i];
    }
    if (n == 0) printf("Nenhum produto encontrado.\n");
    else exibirTabela(achados, n);
}

// Cria produtos.txt vazio e pede ao usuario o cadastro dos produtos, desde o primeiro
void criarArquivo(void) {
    Produto lista[MAX];                               // vetor em memoria (comeca vazio)
    int total = 0;                                    // quantidade de produtos cadastrados nesta sessao
    char resp[10];                                    // resposta s/n do usuario
    FILE *teste = fopen(ARQUIVO, "r");                // testa se o arquivo ja existe
    if (teste != NULL) {
        fclose(teste);                                // so queriamos saber se existe
        lerTexto("O arquivo ja existe e sera apagado. Continuar? (s/n): ", resp, sizeof(resp));
        if (tolower((unsigned char)resp[0]) != 's') { // so continua se responder s
            printf("Operacao cancelada.\n");
            return;
        }
    }
    if (!salvarProdutos(ARQUIVO, lista, 0)) {         // grava o arquivo sem nenhum produto
        printf("Erro ao criar o arquivo!\n");
        return;
    }
    printf("\n======================================\nArquivo %s criado.\n\nVamos cadastrar os produtos!\n======================================\n", ARQUIVO);
    do {
        inserirProduto(lista, &total);                // pede os dados, valida e grava (ID automatico)
        lerTexto("Cadastrar outro produto? (s/n): ", resp, sizeof(resp));
    } while (tolower((unsigned char)resp[0]) == 's'); // repete enquanto o usuario quiser
    printf("Cadastro inicial finalizado com %d produto(s).\n", total);
}

// Mostra o vetor em formato de tabela (nao altera o arquivo)
void exibirTabela(Produto lista[], int total) {
    if (total == 0) {
        printf("Nenhum produto cadastrado. Use a opcao 1 para iniciar o cadastro.\n");
        return;
    }
    printf("\n-----------------------------------------------------------------------------------------\n");
    printf("| %-4s | %-25s | %-18s | %10s | %5s | %-14s |\n",
           "ID", "Produto", "Categoria", "Preco (R$)", "Qtd", "Fornecedor");
    printf("-----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < total; i++) {                 // uma linha por produto
        printf("| %-4d | %-25.25s | %-18.18s | %10.2f | %5d | %-14.14s |\n",
               lista[i].id, lista[i].produto, lista[i].categoria,
               lista[i].preco, lista[i].quantidade, lista[i].fornecedor);
    }
    printf("-----------------------------------------------------------------------------------------\n");
    printf("Total: %d produto(s)\n", total);
}

// Grava produtos.csv separado por virgula
void exportarCSV(Produto lista[], int total) {
    FILE *arq = fopen(ARQUIVO_CSV, "w");
    if (arq == NULL) {
        printf("Erro ao criar %s!\n", ARQUIVO_CSV);
        return;
    }
    fprintf(arq, "ID,Produto,Categoria,Preco,Quantidade,Fornecedor\n"); // cabecalho
    for (int i = 0; i < total; i++) {
        fprintf(arq, "%d,%s,%s,%.2f,%d,%s\n", lista[i].id, lista[i].produto,
                lista[i].categoria, lista[i].preco, lista[i].quantidade, lista[i].fornecedor);
    }
    fclose(arq);
    printf("Exportado %d produto(s) para %s.\n", total, ARQUIVO_CSV);
}

// Maior id existente + 1 (1 se a lista estiver vazia)
int proximoId(Produto lista[], int total) {
    int maior = 0;
    for (int i = 0; i < total; i++) {
        if (lista[i].id > maior) maior = lista[i].id; // guarda o maior id visto
    }
    return maior + 1;
}

// 1 se ja existe produto com esse nome (sem diferenciar maiusculas)
int nomeDuplicado(Produto lista[], int total, const char *nome) {
    for (int i = 0; i < total; i++) {
        if (compararTexto(lista[i].produto, nome) == 0) return 1;
    }
    return 0;
}

// 1 se ja existe produto com esse id
int idDuplicado(Produto lista[], int total, int id) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) return 1;
    }
    return 0;
}

// Pede os dados, valida, insere no vetor e grava no arquivo
void inserirProduto(Produto lista[], int *total) {
    Produto novo;
    if (*total >= MAX) {                              // vetor cheio
        printf("Cadastro cheio (maximo %d produtos).\n", MAX);
        return;
    }
    lerTexto("Nome do produto: ", novo.produto, sizeof(novo.produto));
    if (!textoValido(novo.produto)) { printf("Nome invalido (vazio, ';' ou ',').\n"); return; }
    if (nomeDuplicado(lista, *total, novo.produto)) { printf("Ja existe um produto com esse nome!\n"); return; }
    lerTexto("Categoria: ", novo.categoria, sizeof(novo.categoria));
    if (!textoValido(novo.categoria)) { printf("Categoria invalida (vazia, ';' ou ',').\n"); return; }
    novo.preco = lerDecimal("Preco (ex.: 49.90): ");
    if (novo.preco < 0) { printf("Preco nao pode ser negativo.\n"); return; }
    novo.quantidade = lerInteiro("Quantidade: ");
    if (novo.quantidade < 0) { printf("Quantidade nao pode ser negativa.\n"); return; }
    lerTexto("Fornecedor: ", novo.fornecedor, sizeof(novo.fornecedor));
    if (!textoValido(novo.fornecedor)) { printf("Fornecedor invalido (vazio, ';' ou ',').\n"); return; }

    novo.id = proximoId(lista, *total);               // ID automatico, o usuario nao digita
    if (idDuplicado(lista, *total, novo.id)) { printf("Erro: ID duplicado.\n"); return; }

    lista[*total] = novo;                             // coloca no fim do vetor
    (*total)++;                                       // atualiza o total pelo ponteiro
    if (salvarProdutos(ARQUIVO, lista, *total)) printf("Produto inserido com ID %d.\n", novo.id);
    else { (*total)--; printf("Erro ao gravar o arquivo!\n"); }
}

// Grava em temp.txt e, so se deu certo, substitui o original (evita corromper dados)
int substituirArquivo(Produto lista[], int total) {
    if (!salvarProdutos(ARQUIVO_TEMP, lista, total)) return 0; // falhou no temporario: original intacto
    remove(ARQUIVO);                                  // apaga o original
    if (rename(ARQUIVO_TEMP, ARQUIVO) != 0) return 0; // temporario vira o oficial
    return 1;
}

// Altera um produto pelo id; Enter em um campo mantem o valor atual
void alterarProduto(Produto lista[], int total) {
    char buf[60];
    int id = lerInteiro("ID do produto a alterar: ");
    int pos = -1;
    for (int i = 0; i < total; i++) if (lista[i].id == id) pos = i; // procura o indice
    if (pos == -1) { printf("Produto nao encontrado.\n"); return; }

    Produto *p = &lista[pos];                         // ponteiro para o produto escolhido
    printf("Deixe em branco (Enter) para manter o valor atual.\n");

    printf("Nome atual: %s\n", p->produto);
    lerTexto("Novo nome: ", buf, sizeof(p->produto));
    if (buf[0] != '\0') {
        if (!textoValido(buf)) { printf("Nome invalido. Nada foi alterado.\n"); return; }
        for (int i = 0; i < total; i++) {             // duplicidade contra os OUTROS produtos
            if (i != pos && compararTexto(lista[i].produto, buf) == 0) {
                printf("Ja existe um produto com esse nome. Nada foi alterado.\n");
                return;
            }
        }
    }
    char novoNome[50];
    strcpy(novoNome, buf);                            // guarda para aplicar so no fim

    printf("Categoria atual: %s\n", p->categoria);
    char novaCat[30];
    lerTexto("Nova categoria: ", novaCat, sizeof(novaCat));
    if (novaCat[0] != '\0' && !textoValido(novaCat)) { printf("Categoria invalida. Nada foi alterado.\n"); return; }

    printf("Preco atual: %.2f\n", p->preco);
    char txt[50];
    float novoPreco = p->preco;
    lerTexto("Novo preco: ", txt, sizeof(txt));
    if (txt[0] != '\0') {
        if (sscanf(txt, "%f", &novoPreco) != 1 || novoPreco < 0) { printf("Preco invalido. Nada foi alterado.\n"); return; }
    }

    printf("Quantidade atual: %d\n", p->quantidade);
    int novaQtd = p->quantidade;
    lerTexto("Nova quantidade: ", txt, sizeof(txt));
    if (txt[0] != '\0') {
        if (sscanf(txt, "%d", &novaQtd) != 1 || novaQtd < 0) { printf("Quantidade invalida. Nada foi alterado.\n"); return; }
    }

    printf("Fornecedor atual: %s\n", p->fornecedor);
    char novoForn[50];
    lerTexto("Novo fornecedor: ", novoForn, sizeof(novoForn));
    if (novoForn[0] != '\0' && !textoValido(novoForn)) { printf("Fornecedor invalido. Nada foi alterado.\n"); return; }

    // tudo validado: aplica as mudancas
    if (novoNome[0] != '\0') strcpy(p->produto, novoNome);
    if (novaCat[0] != '\0') strcpy(p->categoria, novaCat);
    p->preco = novoPreco;
    p->quantidade = novaQtd;
    if (novoForn[0] != '\0') strcpy(p->fornecedor, novoForn);

    if (substituirArquivo(lista, total)) printf("Produto alterado com sucesso.\n");
    else printf("Erro ao gravar o arquivo!\n");
}

// Exclui um produto pelo id (com confirmacao) e grava
void excluirProduto(Produto lista[], int *total) {
    char resp[10];
    int id = lerInteiro("ID do produto a excluir: ");
    int pos = -1;
    for (int i = 0; i < *total; i++) if (lista[i].id == id) pos = i;
    if (pos == -1) { printf("Produto nao encontrado.\n"); return; }

    printf("Excluir \"%s\"? ", lista[pos].produto);
    lerTexto("(s/n): ", resp, sizeof(resp));
    if (tolower((unsigned char)resp[0]) != 's') { printf("Exclusao cancelada.\n"); return; }

    for (int i = pos; i < *total - 1; i++) lista[i] = lista[i + 1]; // puxa os seguintes uma posicao
    (*total)--;                                       // um produto a menos
    if (substituirArquivo(lista, *total)) printf("Produto excluido com sucesso.\n");
    else printf("Erro ao gravar o arquivo!\n");
}

// Troca dois produtos usando ponteiros
void trocar(Produto *a, Produto *b) {
    Produto aux = *a;                                 // guarda o conteudo de a
    *a = *b;                                          // a recebe b
    *b = aux;                                         // b recebe o antigo a
}

// Ordena por nome (1 = A-Z, 0 = Z-A) e mostra; trabalha numa copia, o arquivo nao muda
void ordenarPorNome(Produto lista[], int total, int crescente) {
    Produto copia[MAX];
    for (int i = 0; i < total; i++) copia[i] = lista[i];
    for (int i = 0; i < total - 1; i++) {             // Bubble Sort: passadas
        for (int j = 0; j < total - 1 - i; j++) {     // compara vizinhos
            int cmp = compararTexto(copia[j].produto, copia[j + 1].produto);
            if ((crescente && cmp > 0) || (!crescente && cmp < 0)) trocar(&copia[j], &copia[j + 1]);
        }
    }
    printf(crescente ? "\nProdutos por nome (A-Z):" : "\nProdutos por nome (Z-A):");
    exibirTabela(copia, total);
}

// Ordena por preco (1 = menor-maior, 0 = maior-menor) e mostra; arquivo nao muda
void ordenarPorPreco(Produto lista[], int total, int crescente) {
    Produto copia[MAX];
    for (int i = 0; i < total; i++) copia[i] = lista[i];
    for (int i = 0; i < total - 1; i++) {
        for (int j = 0; j < total - 1 - i; j++) {
            if ((crescente && copia[j].preco > copia[j + 1].preco) ||
                (!crescente && copia[j].preco < copia[j + 1].preco)) trocar(&copia[j], &copia[j + 1]);
        }
    }
    printf(crescente ? "\nProdutos por preco (menor -> maior):" : "\nProdutos por preco (maior -> menor):");
    exibirTabela(copia, total);
}