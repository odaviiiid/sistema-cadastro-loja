# Cadastro de Loja em C

Programa de console em C para gerenciar o cadastro de produtos de uma loja. Os dados ficam em um arquivo de texto e o programa combina **manipulação de arquivos**, **estruturas (`struct`)**, **ponteiros** e **algoritmos de ordenação (Bubble Sort)**.

Projeto desenvolvido em grupo para a **Atividade Avaliativa 04**.

## Funcionalidades

- **Criar o cadastro do zero:** gera um `produtos.txt` vazio e já pede o cadastro dos produtos, um após o outro
- **Listar** os produtos em formato de tabela
- **Inserir** produto com **ID automático** (maior ID existente + 1, sem o usuário digitar)
- **Verificação de duplicatas:** não aceita dois produtos com o mesmo nome (sem diferenciar maiúsculas de minúsculas)
- **Alterar** produto pelo ID (Enter mantém o valor atual de cada campo)
- **Excluir** produto pelo ID, com confirmação
- **Buscar** por nome e por categoria (aceita parte do texto, sem diferenciar maiúsculas de minúsculas)
- **Ordenar** por nome (A–Z e Z–A) e por preço (menor → maior e maior → menor)
- **Exportar** os dados para CSV, separado por vírgula

## Como compilar e executar

É necessário um compilador C (por exemplo, o GCC).

```bash
gcc -Wall -Wextra -std=c99 -o loja loja.c
./loja
```

No Windows, o executável gerado é `loja.exe`.

## Como usar

Na primeira execução, escolha a opção **1** para criar o cadastro. O programa pergunta nome, categoria, preço, quantidade e fornecedor de cada produto e repete até você responder `n` em "Cadastrar outro produto?".

Menu principal:

| Opção | Ação |
|------:|------|
| 1 | Criar arquivo e cadastrar os primeiros produtos |
| 2 | Listar produtos |
| 3 | Inserir produto |
| 4 | Alterar produto |
| 5 | Excluir produto |
| 6 | Buscar por nome |
| 7 | Buscar por categoria |
| 8 | Ordenar por nome (A–Z) |
| 9 | Ordenar por nome (Z–A) |
| 10 | Ordenar por preço (menor → maior) |
| 11 | Ordenar por preço (maior → menor) |
| 12 | Exportar para CSV |
| 0 | Sair |

Exemplo de listagem:

```
-----------------------------------------------------------------------------------------
| ID   | Produto                   | Categoria          | Preco (R$) |   Qtd | Fornecedor     |
-----------------------------------------------------------------------------------------
| 1    | Camiseta Basica           | Vestuario          |      49.90 |    30 | Moda Brasil    |
| 2    | Mouse Optico USB          | Informatica        |      59.99 |    15 | TecnoShop      |
-----------------------------------------------------------------------------------------
Total: 2 produto(s)
```

## Formato dos dados

O arquivo `produtos.txt` guarda um produto por linha, com campos separados por `;` e preço com ponto:

```
id;produto;categoria;preco;quantidade;fornecedor
1;Camiseta Basica;Vestuario;49.90;30;Moda Brasil
```

A exportação gera o `produtos.csv`, com cabeçalho e campos separados por vírgula:

```
ID,Produto,Categoria,Preco,Quantidade,Fornecedor
1,Camiseta Basica,Vestuario,49.90,30,Moda Brasil
```

## Como funciona

- **Vetor de estruturas:** os produtos ficam em `Produto lista[MAX]` enquanto o programa roda.
- **Arquivo como fonte oficial:** antes de cada ação do menu, o vetor é recarregado do `produtos.txt`.
- **Arquivo temporário:** ao alterar ou excluir, o programa grava em `temp.txt`, apaga o original e renomeia o temporário. Se a gravação falhar, o arquivo original continua intacto.
- **Ponteiros:** `trocar(Produto *a, Produto *b)` troca dois produtos, e `inserirProduto` e `excluirProduto` recebem `int *total` para atualizar a quantidade.
- **Bubble Sort:** a ordenação trabalha em uma cópia do vetor e só exibe o resultado, sem alterar o arquivo.
- **Leitura segura:** não há `scanf`. A entrada usa `fgets` (`lerTexto`, `lerInteiro` e `lerDecimal`), o que evita sobra de caracteres no buffer.

## Limites e regras

- Máximo de **100 produtos** (`MAX`).
- Tamanho dos campos: nome até 49 caracteres, categoria até 29 e fornecedor até 49. Para textos maiores, aumente os vetores na `struct Produto`.
- Nome, categoria e fornecedor não podem ficar vazios nem conter `;` ou `,`, para não corromper o arquivo e o CSV.
- Preço e quantidade não podem ser negativos.
- O código, as mensagens e os dados não usam acentos.

## Divisão do trabalho

| Responsável | Funções |
|-------------|---------|
| Líder | `carregarProdutos`, `salvarProdutos`, `compararTexto`, `contemTexto`, `textoValido`, `buscarPorNome`, `buscarPorCategoria` e a unificação do código |
| Evandro | `criarArquivo`, `exibirTabela`, `exportarCSV` |
| Enzo | `proximoId`, `nomeDuplicado`, `idDuplicado`, `inserirProduto` |
| Anthony | `alterarProduto`, `excluirProduto` |
| Guilherme | `trocar`, `ordenarPorNome`, `ordenarPorPreco` |

## Estrutura do repositório

```
.
├── loja.c        # código-fonte completo
├── README.md
└── .gitignore    # sugestão: ignorar o executável e os arquivos gerados
```

Sugestão de `.gitignore`:

```
loja
loja.exe
produtos.txt
produtos.csv
temp.txt
```