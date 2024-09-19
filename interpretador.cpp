#include <bits/stdc++.h>

using namespace std;

// Estruturas para armazenar variáveis e linhas de código
map<string, int> memoriaVars; // Armazena as variáveis e seus valores
map<int, string> codigoFonte; // Armazena as linhas do programa BASIC
int linhaAtual = 10;          // Controla a linha atual a ser executada
bool programaParado = false;   // Indicador para finalizar a execução do programa

// Função para dividir uma string com base em um delimitador
vector<string> dividirString(const string& str, char delimitador) {
    vector<string> tokens;
    string token;
    istringstream fluxoDeTokens(str);
    while (getline(fluxoDeTokens, token, delimitador)) {
        if (!token.empty()) { // Ignora partes vazias
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Função para processar múltiplos comandos na mesma linha separados por ":"
vector<string> dividirPorDoisPontos(const string& str) {
    return dividirString(str, ':');
}

// Carrega o código BASIC a partir de um arquivo de texto
void carregarPrograma(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro ao tentar abrir o arquivo!" << endl;
    }
    string linha;
    while (getline(arquivo, linha)) {
        int numeroLinha;
        string comando;
        istringstream fluxLinha(linha);
        fluxLinha >> numeroLinha;
        getline(fluxLinha, comando);
        codigoFonte[numeroLinha] = comando;
        cout << "Carregando linha " << numeroLinha << ": " << comando << endl;
    }
}

// Avalia uma condição entre dois valores
bool avaliarCondicao(int esq, string operador, int dir) {
    if (operador == "<=") return esq <= dir;
    if (operador == ">=") return esq >= dir;
    if (operador == "<") return esq < dir;
    if (operador == ">") return esq > dir;
    if (operador == "==") return esq == dir;
    if (operador == "!=") return esq != dir;
    return false;
}

// Função para avaliar uma expressão matemática simples
int avaliarExpressao(const vector<string>& tokens, int inicio, int fim) {
    int resultado = 0;
    string operador = "+";

    for (int i = inicio; i <= fim; ++i) {
        if (tokens[i] == "+" || tokens[i] == "-") {
            operador = tokens[i];
        } else {
            int valor = 0;
            // Verifica se o token é uma variável ou um número literal
            if (memoriaVars.find(tokens[i]) != memoriaVars.end()) {
                valor = memoriaVars[tokens[i]];
            } else {
                valor = stoi(tokens[i]);
            }

            // Aplica a operação matemática
            if (operador == "+") {
                resultado += valor;
            } else if (operador == "-") {
                resultado -= valor;
            }
        }
    }

    return resultado;
}

// Função para processar um único comando BASIC
bool executarComando(const string& comando) {
    cout << "Executando comando: " << comando << endl;

    vector<string> tokens = dividirString(comando, ' ');

    // Comando de comentário
    if (tokens[0] == "rem") {
        return false; // Ignora linhas de comentário
    }

    // Comando para imprimir na tela
    if (tokens[0] == "print") {
        if (tokens[1][0] == '"') {
            string mensagem = comando.substr(comando.find("\"") + 1);
            mensagem = mensagem.substr(0, mensagem.rfind("\""));
            cout << mensagem << endl;
        } else {
            // Imprime variável
            if (memoriaVars.find(tokens[1]) != memoriaVars.end()) {
                cout << memoriaVars[tokens[1]] << endl;
            } else {
                cout << "Erro: Variável não encontrada -> " << tokens[1] << endl;
            }
        }
        return false;
    }

    // Entrada de dados do usuário
    else if (tokens[0] == "input") {
        int valor;
        cout << "Informe " << tokens[1] << ": ";
        cin >> valor;
        memoriaVars[tokens[1]] = valor;
        cout << "Variável " << tokens[1] << " definida com valor: " << valor << endl;
        return false;
    }

    // Atribuição de valor a uma variável
    else if (tokens[1] == "=") {
        // Verifica se a expressão envolve operações matemáticas
        if (tokens.size() > 3) {
            int resultado = avaliarExpressao(tokens, 2, tokens.size() - 1);
            memoriaVars[tokens[0]] = resultado;
        } else {
            // Atribuição simples de valor
            try {
                memoriaVars[tokens[0]] = stoi(tokens[2]);
            } catch (...) {
                if (memoriaVars.find(tokens[2]) != memoriaVars.end()) {
                    memoriaVars[tokens[0]] = memoriaVars[tokens[2]]; // Copia valor de outra variável
                } else {
                    cout << "Erro: Atribuição inválida para -> " << tokens[0] << endl;
                }
            }
        }
        cout << "Variável " << tokens[0] << " definida com valor: " << memoriaVars[tokens[0]] << endl;
        return false;
    }

    // Comando para saltar para outra linha
    else if (tokens[0] == "goto") {
        if (codigoFonte.find(stoi(tokens[1])) != codigoFonte.end()) {
            linhaAtual = stoi(tokens[1]);
            cout << "Indo para a linha: " << linhaAtual << endl;
        } else {
            cout << "Erro: Linha " << tokens[1] << " não encontrada!" << endl;
            programaParado = true; // Para a execução caso a linha não exista
        }
        return true;
    }

    // Estrutura condicional 'if'
    else if (tokens[0] == "if") {
        if (memoriaVars.find(tokens[1]) != memoriaVars.end()) {
            int valorVar = memoriaVars[tokens[1]];
            int valorComparacao = stoi(tokens[3]);

            cout << "Comparando " << tokens[1] << " (" << valorVar << ") com " << valorComparacao << endl;

            if (avaliarCondicao(valorVar, tokens[2], valorComparacao)) {
                if (codigoFonte.find(stoi(tokens[5])) != codigoFonte.end()) {
                    linhaAtual = stoi(tokens[5]);
                    cout << "Condição verdadeira, indo para a linha: " << linhaAtual << endl;
                    return true;
                } else {
                    cout << "Erro: Linha " << tokens[5] << " não encontrada!" << endl;
                    programaParado = true;
                }
            }
        } else {
            cout << "Erro: Variável " << tokens[1] << " não encontrada para comparação." << endl;
        }
    }

    // Comando para parar o programa
    else if (tokens[0] == "halt") {
        programaParado = true;
        cout << "Execução interrompida com comando HALT." << endl;
        return true;
    }

    return false;
}

// Executa o programa carregado
void executarPrograma() {
    while (codigoFonte.find(linhaAtual) != codigoFonte.end() && !programaParado) {
        cout << "------------------------------------------\n";
        cout << "Linha atual: " << linhaAtual << endl;
        
        string linhaComando = codigoFonte[linhaAtual];
        vector<string> comandos = dividirPorDoisPontos(linhaComando);
        bool mudouLinha = false;

        for (const string& cmd : comandos) {
            if (executarComando(cmd)) {
                mudouLinha = true;
                break; // Sai do loop quando encontrar 'goto' ou 'halt'
            }
        }

        // Avança para a próxima linha se não houver salto
        if (!mudouLinha) {
            linhaAtual += 10; // Avança para a próxima linha
        }

        // Log dos valores das variáveis importantes
        if (memoriaVars.find("A") != memoriaVars.end()) {
            cout << "Valor de A: " << memoriaVars["A"] << endl;
        }
        if (memoriaVars.find("S") != memoriaVars.end()) {
            cout << "Valor de S: " << memoriaVars["S"] << endl;
        }
    }
}

int main() {
    // Carrega o código do programa BASIC a partir do arquivo
    carregarPrograma("interpretador.txt");

    // Executa o programa carregado
    executarPrograma();,
}
