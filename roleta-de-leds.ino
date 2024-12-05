#include <LiquidCrystal.h>

// Conexões do LCD
const int rs = A0; // Register Select
const int en = A1; // Enable
const int d4 = A2; // Data 4
const int d5 = A3; // Data 5
const int d6 = A4; // Data 6
const int d7 = A5; // Data 7

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Configurações do jogo
int tempo = 400;
int pontuacao = 0;
const int tempoMinimo = 100;
int acertosConsecutivos = 0;
const int maxErros = 3;
const int maxAcertos = 10;
int erros = 0;

byte heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
};

unsigned long tempoInicio = 0;
unsigned long ultimoTempoLED = 0;
int tempoRestante = 30;
int ledAtual = 3;
bool indoDireita = true;

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    
    // Cria o caractere personalizado
    lcd.createChar(0, heart);
    
    pinMode(13, INPUT_PULLUP);
    for (int pin = 3; pin <= 11; pin++) {
        pinMode(pin, OUTPUT);
    }
    
    lcd.setCursor(0, 0);
    lcd.print("Roleta de LEDs"); 
    delay(500);
    lcd.clear();
    tempoInicio = millis();
    ultimoTempoLED = millis();
    updateDisplay();
}

void loop() {
    if (millis() - tempoInicio >= 1000) {
        tempoRestante--;
        tempoInicio = millis();

        if (tempoRestante <= 0) {
            gameOver();
            return;
        }

        updateDisplay();
    }

    if (millis() - ultimoTempoLED >= tempo) {
        ultimoTempoLED = millis();

        digitalWrite(ledAtual, LOW);

        if (indoDireita) {
            ledAtual++;
            if (ledAtual > 11) {
                indoDireita = false;
                ledAtual = 10;
            }
        } else {
            ledAtual--;
            if (ledAtual < 3) {
                indoDireita = true;
                ledAtual = 4;
            }
        }

        digitalWrite(ledAtual, HIGH);
    }

    if (digitalRead(13) == LOW) {
        delay(50);
        if (digitalRead(13) == LOW) {
            while (digitalRead(13));
            handlePress(ledAtual);
        }
    }
}

void handlePress(int x) {
    if (x == 7) {
        pontuacao += 20;
        acertosConsecutivos++;
        tempo = max(tempo - 50, tempoMinimo);
    } else if (x == 6 || x == 8) {
        pontuacao += 10;
        acertosConsecutivos++;
        tempo = max(tempo - 50, tempoMinimo);
    } else if (x == 5 || x == 9) {
        pontuacao += 5;
        acertosConsecutivos++;
        tempo = max(tempo - 50, tempoMinimo);
    } else {
        erros++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Errou!");
        delay(500);
        lcd.clear();
        if (erros >= maxErros) {
            gameOver();
            return;
        }
    }

    if (acertosConsecutivos >= maxAcertos) {
        winGame();
        return;
    }

    updateDisplay();
}

void updateDisplay() {
    lcd.setCursor(0, 0);
    lcd.print("P: ");
    lcd.print(pontuacao);
    lcd.setCursor(12, 0);
    lcd.print(acertosConsecutivos);
    lcd.print("/");
    lcd.print(maxAcertos);
    lcd.setCursor(0, 1);
    lcd.print("Tempo: ");
    if (tempoRestante < 10) {
        lcd.print("0");
        lcd.print(tempoRestante);
    } else {
        lcd.print(tempoRestante);
    }
    lcd.print("s");

    // Exibe os corações como vidas
    lcd.setCursor(13, 1);
    for (int i = 0; i < maxErros - erros; i++) {
        lcd.write(byte(0)); // Exibe o coração
    }
    for (int i = maxErros - erros; i < maxErros; i++) {
        lcd.print(" "); // Preenche com espaços vazios
    }
}

void winGame() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Voce venceu!");
    delay(2000);
    resetGame();
}

void gameOver() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game over :(");
    delay(2000);
    resetGame();
}

void resetGame() {
    for (int j = 1; j < 4; j++) {
        for (int x = 3; x <= 11; x++) { 
            digitalWrite(x, HIGH); 
            delay(50); 
            digitalWrite(x, LOW); 
        }
    }
    delay(500);

    pontuacao = 0;
    tempo = 400;
    acertosConsecutivos = 0;
    erros = 0;
    tempoRestante = 30;
    
    // Recria o coração ao reiniciar o jogo
    lcd.createChar(0, heart);
    
    lcd.clear();
    updateDisplay();
}
