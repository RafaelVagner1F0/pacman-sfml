# PacMan Medieval
Jogo inspirado no clássico **Pac-Man**, desenvolvido em **C++** utilizando a biblioteca **SFML**.

Nesta versão, o jogador controla um cavaleiro que deve coletar todas as moedas do mapa enquanto evita monstros controlados por diferentes tipos de inteligência artificial.

O jogo foi desenvolvido como parte do **Trabalho Prático 3** da disciplina **INF110 – Programação I**, da **Universidade Federal de Viçosa (UFV)**.

O projeto foi planejado e desenvolvido pelos alunos:

- Rafael Vagner Pinto da Fonseca Souza
- Matheus Borges Nery De Mingo
- Daniel Martins Teixeira
  
---

## Sobre o jogo
A partida inicia imediatamente após a execução do jogo. Os monstros passam a se movimentar desde o início. O jogo termina quando todas as moedas são coletadas (vitória) ou quando o personagem é capturado por um monstro (derrota). 

Cada moeda vale **10 pontos**, enquanto cada poder especial concede **80 pontos**.

O projeto foi desenvolvido buscando reproduzir a jogabilidade clássica do Pac-Man. Para isso, diversos trechos do código trabalham em conjunto para controlar aspectos como movimentação, temporização, comportamento dos monstros e estrutura do mapa.

No [Arquivo de apresentação do jogo](projeto_pacman.pdf) existe um manual de utilização do jogo e explicações acerca de algumas partes do código e características adicionais.

---
## Funcionalidades

- Movimentação em quatro direções (WASD).
- Sistema de pontuação.
- Coleta de moedas distribuídas pelo labirinto.
- Condições de vitória e derrota.
- Túnel lateral de teletransporte.
- Monstros com comportamentos distintos.
- Poder de congelamento dos monstros (**Floco de neve**).
- Poder de desaceleração temporária dos monstros (**Relógio**).
---
## Características da implementação

- Dois tipos de inteligência artificial para os monstros (aleatória e perseguidora).
- Perseguição baseada na distância de Manhattan.
- Buffer de direção para movimentação mais fluida, semelhante ao Pac-Man clássico.
- Controle independente da movimentação dos monstros utilizando `sf::Clock`.
- Sistema de poderes temporizados.
- Velocidades individuais para cada monstro.
- Espelhamento dos sprites por código, evitando imagens duplicadas.
- Sistema de colisão e movimentação baseado em uma grade de 40×40 pixels.

---
## Ambiente de desenvolvimento

O projeto foi desenvolvido e testado utilizando:

- Sistema Operacional: Windows 11
- Compilador: MinGW-w64 g++ 14.2.0
- Linguagem: C++17
- Biblioteca gráfica: SFML 3.1.0

---
## Requisitos

- Compilador compatível com **C++17**
- **SFML 3.1.0**

---

## Compilação e execução
Os comandos abaixo são para ambiente Windows com MinGW.
Compilar:
```bash
g++ 000tp3.cpp -o 000tp3.exe -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system
```

Executar:
```bash
./000tp3.exe
```
---

## Gameplay

### Vitória
![Vitória](gif_imagens/Vitória.png)

### Derrota
![Derrota](gif_imagens/Derrota.png)

### Poder de câmera lenta (Relógio)
![Câmeralenta](gif_imagens/cameralenta.gif)

### Poder de congelamento (Floco de neve)
![Congelamento](gif_imagens/congelamento.gif)

---

Projeto desenvolvido para o **Trabalho Prático 3** da disciplina **INF110 – Programação I**, da **Universidade Federal de Viçosa (UFV)**.
