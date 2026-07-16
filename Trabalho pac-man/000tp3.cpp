#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string> // Incluído para gerenciar o texto do placar
using namespace std;

// Enum que representa as direções possíveis de movimento no jogo (Nenhuma = parado, que é só no início do jogo)
enum Direcao { Nenhuma, Cima, Baixo, Esquerda, Direita };

//Estrutura que representa um fantasma no jogo
struct Fantasma{
    float x, y;              //Posição atual do fantasma no mapa
    Direcao direcao;         //Direção atual de movimento
    sf::Sprite sprite;       //Imagem do fantasma que será desenhada na tela
    int tipo;                // 0 = Aleatorio, 1 = Perseguidor
    float velocidadeBase;    //Velocidade de movimento do fantasma (Cada fantasma tem sua velocidade)
};
//Função auxiliar que retorna o módulo de um número
float modulo(float valor){
    if (valor < 0) return -valor; // transforma negativo em positivo
    return valor;
}

unsigned int sementeSorteio = 0;
int numeroAleatorio(int limite){
    sementeSorteio = (sementeSorteio * 1103515245 + 12345);
    return (sementeSorteio / 65536) % limite;
}
//Implementando restrições aos fantasmas
bool fantasmaPodeMover(int l, int c, Direcao d, int mapa[24][40]){
    if (d == Cima) 
        l--;
    else if (d == Baixo) 
        l++;
    else if (d == Esquerda)
        c--;
    else if (d == Direita)
        c++;

    // Impede que os fantasmas entrem no túnel
    if (l < 0 || l >= 24 || c < 0 || c >= 40) 
        return false;
    
    // Proíbe que fantasmas entrem de volta na caixa (Parede Invisível)
    if (l == 11 && (c == 19 || c == 20) && d == Baixo) 
        return false;

    return mapa[l][c] != 1; // Retorna true se não for parede
}

// Retorna todas as direções válidas que o fantasma pode seguir a partir da posição atual
int pegarDirecoesValidas(int l, int c, Direcao atual, int mapa[24][40], Direcao opcoes[4]) {
    int qtd = 0;
    
    // Arrays auxiliares com as 4 direções e seus respectivos opostos
    Direcao dirs[4] = { Cima, Baixo, Esquerda, Direita };
    Direcao opostos[4] = { Baixo, Cima, Direita, Esquerda };

    for (int i = 0; i < 4; i++) {
        // Se o fantasma está parado (Nenhuma), ele pode ir para qualquer lado livre.
        // Se já está andando, ele só não pode ir para a direção oposta (voltar para trás).
        bool direcaoValida = (atual == Nenhuma) || (atual != opostos[i]);

        if (direcaoValida && fantasmaPodeMover(l, c, dirs[i], mapa)) {
            opcoes[qtd++] = dirs[i];
        }
    }

    // Retorna quantas direções válidas foram encontradas
    return qtd; 
}
// ======================================================================================================================================== //

int main()
{
    sf::RenderWindow window(sf::VideoMode({1600, 960}), "Trabalho 3");
    window.setFramerateLimit(60);

    // Posição inicial do cavaleiro (Personagem principal)
    float posx = 760, posy = 280;   
    
    //Inicia parado
    Direcao direcaoAtual = Nenhuma;
    //Guarda a intenção do jogador
    Direcao direcaoDesejada = Nenhuma; 

    sf::Clock relogioMovimento;
    float tempoPasso = 0.15f; //Tempo entre cada movimento do personagem principal

    //Array de relógios, um para cada inimigo
    sf::Clock relogiosFantasmas[4];

    //Variáveis pro poder de congelamento
    bool fantasmasCongelados = false;
    sf::Clock relogioCongelamento;

    //Variáveis pro poder de parar o tempo (relógio)
    bool fantasmasLentos = false;
    sf::Clock relogioLentidao;

    //Placar
    int score = 0;
    sf::Font font;
    if (!font.openFromFile("ari-w9500-bold.ttf")) { 
        cout << "Erro ao carregar a fonte ari-w9500-bold.ttf\n";
        return 0;
    }
    sf::Text textoPlacar(font, "SCORE: 0", 40);
    textoPlacar.setFillColor(sf::Color::White);
    textoPlacar.setPosition({680.f, 340.f}); //Posição do texto 

    //Variáveis e texto de vitória e derrota
    bool venceu = false;
    bool perdeu = false;
    
    sf::Text textoVitoria(font, "VOCE VENCEU!", 100);
    textoVitoria.setFillColor(sf::Color::Green);
    textoVitoria.setPosition({420.f, 400.f});
    
    sf::Text textoDerrota(font, "VOCE PERDEU!", 100);
    textoDerrota.setFillColor(sf::Color::Red);
    textoDerrota.setPosition({420.f, 400.f});
    //Matriz do labirinto
    int mapa[24][40] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        // O '3' na linha debaixo é o floco de neve no canto superior esquerdo do mapa
        {1,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
        {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1,2,1},
        {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1,2,1},
        {1,2,2,2,1,1,2,1,1,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,1,1,2,1,1,2,2,2,2,2,1},
        {1,1,1,2,1,1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,2,1},
        {1,1,1,2,1,1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,2,1},
        {1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1,1,1,1,0,0,1,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1,1,2,1}, // '0,0' é a porta da casa dos inimigos
        {2,2,2,2,2,2,2,2,2,2,1,1,1,1,2,1,0,0,0,0,0,0,0,0,1,2,1,1,1,1,2,2,2,2,2,2,2,2,2,2}, // Interior vazio (0)
        {1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1,0,0,0,0,0,0,0,0,1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1}, // Interior vazio (0)
        {1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1}, 
        {1,2,1,1,1,1,2,1,1,1,1,1,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
        {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
        {1,2,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1,2,1,1,1,1,2,1},
        {1,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,1},
        {1,2,1,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,1,2,1},
        // O '4' na linha debaixo é o relogio no canto inferior direito do mapa
        {1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,4,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    //Contagem de quantas pílulas e poderes existem no início
    int itensRestantes = 0;
    for(int i = 0; i < 24; i++){
        for(int j = 0; j < 40; j++){
            if(mapa[i][j] == 2 || mapa[i][j] == 3 || mapa[i][j] == 4)
                itensRestantes++;
        }
    }

    //Colocando a textura das paredes
    sf::Texture texturaParede;
    if(!texturaParede.loadFromFile("tijologrande.png")){
        cout << "Erro lendo imagem textura_parede.png\n";
        return 0;
    }
    sf::Sprite paredeSprite(texturaParede);
    
    //Colocando as pilulas no mapa
    sf::CircleShape pilula(5.f); 
    pilula.setFillColor(sf::Color::Yellow);
    pilula.setOrigin({-16.f, -16.f}); 

    //Colocando a foto do floco de neve (3 na matriz)
    sf::Texture texturaFloco;
    if(!texturaFloco.loadFromFile("floco.png")) cout << "Erro lendo floco.png\n";
        sf::Sprite spriteFloco(texturaFloco);

    //Colocando a foto do relogio (4 na matriz)
    sf::Texture texturaRelogio;
    if(!texturaRelogio.loadFromFile("relogio.png")) cout << "Erro lendo relogio.png\n";
        sf::Sprite spriteRelogio(texturaRelogio);

    //Colocando a foto do personagem principal
    sf::Texture texture;
    if (!texture.loadFromFile("personagem.png")) {
        cout << "Erro lendo imagem personagem.png\n";
        return 0;
    }
    sf::Sprite principal(texture);
    
    //Define a origem do sprite (personagem) no centro dele para espelhar a foto sem teleportar
    principal.setOrigin({20.f, 20.f});

    //Inicialização dos fantasmas
        sf::Texture tF1, tF2, tF3, tF4;
    if (!tF1.loadFromFile("fantasma1.png")) 
        cout << "Erro lendo fantasma1.png\n";
    if (!tF2.loadFromFile("fantasma2.png")) 
        cout << "Erro lendo fantasma2.png\n";
    if (!tF3.loadFromFile("fantasma3.png")) 
        cout << "Erro lendo fantasma3.png\n";
    if (!tF4.loadFromFile("fantasma4.png")) 
        cout << "Erro lendo fantasma4.png\n"; 
    
    sf::Sprite sF1(tF1), sF2(tF2), sF3(tF3), sF4(tF4);

    //Define a origem do sprite (inimigos) no centro de suas respectivas texturas, para espelhar a foto sem teleportar
    sF1.setOrigin({20.f, 20.f});
    sF2.setOrigin({20.f, 20.f});
    sF3.setOrigin({20.f, 20.f});
    sF4.setOrigin({20.f, 20.f});

    //Inicializando os fantasmas com posição, direção, sprite, tipo de IA e velocidade)
    Fantasma fantasmas[4] = {
        {18.f * 40.f, 12.f * 40.f, Nenhuma, sF1, 0, 0.18f}, // inimigo do tipo aleatório (Velocidade Média)
        {19.f * 40.f, 12.f * 40.f, Nenhuma, sF2, 0, 0.17f}, // inimigo do tipo aleatório (Velocidade Média)
        {20.f * 40.f, 12.f * 40.f, Nenhuma, sF3, 1, 0.20f}, // inimigo do tipo perseguidor (Velocidade Lenta)
        {21.f * 40.f, 12.f * 40.f, Nenhuma, sF4, 1, 0.16f}  // inimigo do tipo perseguidor (Velocidade Alta)
    };

    while (window.isOpen()){
        sementeSorteio++; //Muda a semente para evitar padrões repetitivos

        while (const optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();

                // O teclado agora altera a DIREÇÃO DESEJADA (Buffer)
                if (keyPressed->scancode == sf::Keyboard::Scancode::W)
                    direcaoDesejada = Cima;   
                else if (keyPressed->scancode == sf::Keyboard::Scancode::S)
                    direcaoDesejada = Baixo;   
                else if (keyPressed->scancode == sf::Keyboard::Scancode::A)
                    direcaoDesejada = Esquerda;   
                else if (keyPressed->scancode == sf::Keyboard::Scancode::D)
                    direcaoDesejada = Direita;   
            }
        }

        //O movimento do personagem principal só acontece se ele não perdeu e não venceu
        if (!perdeu && !venceu && relogioMovimento.getElapsedTime().asSeconds() >= tempoPasso){
            //Movimento contínuo
            //Verificar se é possível virar para a direção desejada
            float testeX = posx;
            float testeY = posy;

            if (direcaoDesejada == Cima)
                testeY -= 40;
            if (direcaoDesejada == Baixo)   
                testeY += 40;
            if (direcaoDesejada == Esquerda) 
                testeX -= 40;
            if (direcaoDesejada == Direita)  
                testeX += 40;

            int lDesejada = (int)(testeY / 40);
            int cDesejada = (int)(testeX / 40);

            //Se a casa desejada estiver livre, a direção atual muda para ela imediatamente
            if (lDesejada >= 0 && lDesejada < 24 && cDesejada >= 0 && cDesejada < 40){
                if (mapa[lDesejada][cDesejada] != 1)
                    direcaoAtual = direcaoDesejada;
            }

            //Executa o movimento baseado na direção atual decidida
            float proximoX = posx;
            float proximoY = posy;

            if (direcaoAtual == Cima)
                proximoY -= 40;
            if (direcaoAtual == Baixo)   
                proximoY += 40;
            if (direcaoAtual == Esquerda) 
                proximoX -= 40;
            if (direcaoAtual == Direita)  
                proximoX += 40;

            int linha = (int)(proximoY / 40);
            int coluna = (int)(proximoX / 40);

            if (linha >= 0 && linha < 24 && coluna >= 0 && coluna < 40){
                if(mapa[linha][coluna] != 1){
                    posx = proximoX;
                    posy = proximoY;

                    //Aplica o espelhamento vertical ao personagem principal, baseado na direção
                    if (direcaoAtual == Esquerda)
                        principal.setScale({-1.f, 1.f});          //Inverte X
                    else if (direcaoAtual == Direita)
                        principal.setScale({1.f, 1.f});           //Normal X
                }
                else 
                  direcaoAtual = Nenhuma;  // Interrompe o movimento ao colidir com uma parede
            }

            //Lógica do túnel
            if (proximoX < 0 && linha == 12)
                posx = 1560;
            else if (proximoX >= 1600 && linha == 12)
                posx = 0;

            //Sistema de comer pílulas e usar os poderes do floco de neve e do relógio.
            int mtualLinha = (int)(posy / 40);
            int atualColuna = (int)(posx / 40);

            //Sistema de comer pílulas amarelas
            if(mapa[mtualLinha][atualColuna] == 2){
                mapa[mtualLinha][atualColuna] = 0; 
                score += 10;
                itensRestantes--;
                textoPlacar.setString("SCORE: " + to_string(score));
            }
            //Sistema de usar o poder floco de neve (Número 3)
            else if (mapa[mtualLinha][atualColuna] == 3){
                mapa[mtualLinha][atualColuna] = 0; 
                score += 80; 
                itensRestantes--; 
                textoPlacar.setString("SCORE: " + to_string(score));
                
                fantasmasCongelados = true;
                relogioCongelamento.restart(); 
            }
            //Usar o poder do relógio (Número 4)
            else if (mapa[mtualLinha][atualColuna] == 4){
                mapa[mtualLinha][atualColuna] = 0; 
                score += 80; 
                itensRestantes--;
                textoPlacar.setString("SCORE: " + to_string(score));
                fantasmasLentos = true;
                relogioLentidao.restart(); 
            }

            //Verifica se comeu tudo pra ganhar o jogo
            if(itensRestantes <= 0){
                venceu = true;
                textoVitoria.setString("VOCE VENCEU!\nPONTOS: " + to_string(score));
            }

            relogioMovimento.restart(); 
        }

        //Lógica pra desativar a lentidão do relógio depois de 5 segundos
        if (fantasmasLentos && relogioLentidao.getElapsedTime().asSeconds() >= 5.0f)
            fantasmasLentos = false;

        //Lógica pra descongelar os inimigos depois de 4 segundos
        if(fantasmasCongelados && relogioCongelamento.getElapsedTime().asSeconds() >= 4.0f) 
            fantasmasCongelados = false;

        //Tratamento dos fantasmas(inimigos), eles só pensam e se movem se não estiverem congelados 
        if (!fantasmasCongelados){
            //Laço que cuida de um fantasma por vez
            for (int i = 0; i < 4; i++) {
                
                //Define a velocidade dos fantasmas (reduzida quando o efeito de lentidão está ativo) 
                float velocidadeAtualFantasmas;
                if(fantasmasLentos)
                    velocidadeAtualFantasmas = 0.80f;
                else
                    velocidadeAtualFantasmas = fantasmas[i].velocidadeBase;

                //Checa o relógio individual de cada fantasma
                //Fantasmas também param de andar se o jogador vencer ou perder
                if (!perdeu && !venceu && relogiosFantasmas[i].getElapsedTime().asSeconds() >= velocidadeAtualFantasmas){
                    
                    int lF = (int)(fantasmas[i].y / 40);
                    int cF = (int)(fantasmas[i].x / 40);

                    //Verifica se está na caixa para obrigá-lo a sair
                    bool dentroDaCaixa = (lF >= 11 && lF <= 13) && (cF >= 16 && cF <= 23);   

                    
                    if(dentroDaCaixa){
                        //Se estiver na linha de saída, sobe
                        if (lF == 11){
                            fantasmas[i].direcao = Cima;
                        } else {
                            //Direciona o fantasma para sair da caixa (esquerda/direita até alinhar, depois sobe)
                            if (cF < 19) fantasmas[i].direcao = Direita;
                            else if (cF > 20) fantasmas[i].direcao = Esquerda;
                            else fantasmas[i].direcao = Cima;
                        }
                    } 
                    else {
                        //Verifica se o fantasma pode continuar na direção atual (não bateu em parede)
                        bool naParede = !fantasmaPodeMover(lF, cF, fantasmas[i].direcao, mapa);
                        //Gera lista de direções possíveis a partir da posição atual
                        Direcao opcoesDeCaminho[4]; 
                        int qtdOpcoes = pegarDirecoesValidas(lF, cF, fantasmas[i].direcao, mapa, opcoesDeCaminho);

                        //IA dos inimigos aletórios (Tipo 0)
                        if(fantasmas[i].tipo == 0){  
                            //Se bateu na parede ou não tem direção definida, escolhe aleatoriamente
                            if(naParede || fantasmas[i].direcao == Nenhuma || qtdOpcoes > 1){
                                if(qtdOpcoes > 0){
                                    fantasmas[i].direcao = opcoesDeCaminho[numeroAleatorio(qtdOpcoes)];
                                }
                                 // Se não houver opções, fica parado e dps ele recalculará a rota
                                else 
                                    fantasmas[i].direcao = Nenhuma;
                            }
                        } 
                        //IA dos inimigos perseguidores (Tipo 1)
                        else if(fantasmas[i].tipo == 1){   
                             //Recalcula direção em casos necessários, que seriam colisão na parede, inatividade ou múltiplas opções(encruzilhadas)
                             if (naParede || fantasmas[i].direcao == Nenhuma || qtdOpcoes > 1) {
                                float menorDistancia = 999999.f;
                                Direcao melhorDirecao = fantasmas[i].direcao;
                                // Se não houver opções, fica parado
                                if (qtdOpcoes == 0) 
                                    melhorDirecao = Nenhuma; 

                                //Testa todas as direções possíveis e escolhe a mais próxima do jogador
                                for(int k = 0; k < qtdOpcoes; k++){
                                    Direcao testarDir = opcoesDeCaminho[k];
                                    int nextL = lF, nextC = cF;
                                    //Simula a posição futura (1 bloco à frente) para avaliar cada direção possível
                                    if (testarDir == Cima)
                                        nextL--;
                                    else if (testarDir == Baixo) 
                                        nextL++;
                                    else if (testarDir == Esquerda) 
                                        nextC--;
                                    else if (testarDir == Direita) 
                                        nextC++;

                                    //Calcula distância até o jogador
                                    float dist = modulo(nextC * 40.f - posx) + modulo(nextL * 40.f - posy);
                                    //Guarda a direção que deixa o fantasma mais próximo do jogador
                                    if (dist < menorDistancia) {
                                        menorDistancia = dist;
                                        melhorDirecao = testarDir;
                                    }
                                }
                                fantasmas[i].direcao = melhorDirecao; //Aplica a melhor direção encontrada, ou seja, a menor
                            }
                        }
                    } 
                    //Atualiza a posição do fantasma conforme sua direção atual
                    if (fantasmas[i].direcao == Cima) 
                        fantasmas[i].y -= 40;
                    else if (fantasmas[i].direcao == Baixo) 
                        fantasmas[i].y += 40;
                    else if (fantasmas[i].direcao == Esquerda) 
                        fantasmas[i].x -= 40;
                    else if (fantasmas[i].direcao == Direita) 
                        fantasmas[i].x += 40;

                    //Aplicar o espelhamento individual para cada fantasma
                    if (fantasmas[i].direcao == Esquerda) {
                        fantasmas[i].sprite.setScale({-1.f, 1.f});
                    } else if (fantasmas[i].direcao == Direita) {
                        fantasmas[i].sprite.setScale({1.f, 1.f});
                    }
                    
                    //Reseta o relógio do fantasma
                    relogiosFantasmas[i].restart();
                } //Fim da checagem do relógio individual
            }
        } //Fim do IF (!fantasmasCongelados)

        //Tratando o GAME OVER
        //Se os fantasmas estiverem congelados, o PacMan não morre ao encostar neles
        if(!fantasmasCongelados && !venceu && !perdeu){
            for(int i = 0; i < 4; i++){
                if (modulo(fantasmas[i].x - posx) < 20.f && modulo(fantasmas[i].y - posy) < 20.f) {
                    perdeu = true;
                }
            }
        }

        // Define o fundo da janela como cinza e remove o que foi desenhado no frame anterior
        window.clear(sf::Color(80, 80, 80));

        //Criação do mapa
        //Percorre toda a matriz do mapa (24 linhas x 40 colunas) 
        for(int i = 0; i < 24; i++){
            for (int j = 0; j < 40; j++){
                    //Se for parede (1), desenha o sprite da parede naquela posição
                    if(mapa[i][j] == 1){
                        paredeSprite.setPosition({ (float)j * 40.f, (float)i * 40.f });
                        window.draw(paredeSprite);
                    }
                    //Se for pílula comum (2), desenha a pílula
                    else if(mapa[i][j] == 2){
                        pilula.setPosition({ (float)j * 40.f, (float)i * 40.f });
                        window.draw(pilula);
                    }
                    //Se for o poder do floco de neve (3)
                    else if(mapa[i][j] == 3){
                        spriteFloco.setPosition({ (float)j * 40.f, (float)i * 40.f });
                        window.draw(spriteFloco);
                    }
                    //Se for o poder do relógio (4)
                    else if (mapa[i][j] == 4){
                        spriteRelogio.setPosition({ (float)j * 40.f, (float)i * 40.f });
                        window.draw(spriteRelogio);
                    }
           }
        }

        //Ajusta a posição para centralizar o sprite na célula do mapa (que é 40x40)
        principal.setPosition({posx + 20.f, posy + 20.f});
        window.draw(principal);

        //Desenha os fantasmas somando + 20.f para centralizar os sprites deles na célula do mapa.
        for(int i = 0; i < 4; i++){
            fantasmas[i].sprite.setPosition({fantasmas[i].x + 20.f, fantasmas[i].y + 20.f});
            window.draw(fantasmas[i].sprite);
        }

        window.draw(textoPlacar);   //Escrever o placar

        //Escrever as mensagens finais
        if(venceu) 
           window.draw(textoVitoria);
        if(perdeu) 
           window.draw(textoDerrota);

        window.display();
    }
    return 0;
}