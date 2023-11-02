//Example from: https://github.com/VolosR/PokerS3/tree/main

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "NotoSansBold15.h"
#include "cards.h"
#include "OneButton.h"
#define up 14
#define down 0

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
OneButton button(down, true);

bool deb = 0;
bool deb2 = 0;

int fase = 0;
float win = 0;
int tmpHand[5];
int deck[52];
int hand[5];
bool held[5] = {0, 1, 1, 0, 0};
int handRank = 0;
int chosen = 0;

float score = 20;
int bet = 2;
int deal = 0;

int x[8] = {83, 83, 83, 83, 203, 203, 203, 203};
int y[8] = {0, 18, 36, 54, 0, 18, 36, 54};
String txt[8] = {"HIGH CARD", "PAIR", "TWO PAIRS", "3 OF A KIND", "STRAIGHT", "FLUSH", "FULL HOUSE", "POKER"};
float multi[8] = {0.5, 1, 3, 4, 8, 10, 15, 30};

unsigned long timePassed = 0;
unsigned long tempPassed = 0;
unsigned long timePassed2 = 0;
unsigned long tempPassed2 = 0;
int period = 250;
bool started = 0;
bool started2 = 0;


void shuffle(int *array, int size)
{
    for (int i = size - 1; i > 0; i--) {
        int j = random(i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int flush(int *hand)
{
    int suit = hand[0] / 13;
    for (int i = 1; i < 5; i++) {
        if (hand[i] / 13 != suit) {
            return 0; // Not a flush
        }
    }
    return 1; // Flush
}

int straight(int *hand)
{
    // Sort the hand
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (hand[j] % 13 > hand[j + 1] % 13) {
                int temp = hand[j];
                hand[j] = hand[j + 1];
                hand[j + 1] = temp;
            }
        }
    }

    // Check for a straight
    for (int i = 0; i < 4; i++) {
        if (hand[i] % 13 + 1 != hand[i + 1] % 13) {
            return 0; // Not a straight
        }
    }
    return 1; // Straight
}
// Function to evaluate a poker hand and determine its rank
int evaluateHand(int *hand)
{
    // Define arrays for card ranks and suits
    int ranks[13] = {0}; // Initialize rank counters to 0

    // Count the number of each rank in the hand
    for (int i = 0; i < 5; i++) {
        int rank = hand[i] % 13;
        ranks[rank]++;
    }

    int pairs = 0;
    int threeOfAKind = 0;
    int fourOfAKind = 0;

    // Check for pairs, three of a kind, and four of a kind
    for (int i = 0; i < 13; i++) {
        if (ranks[i] == 2) {
            pairs++;
        } else if (ranks[i] == 3) {
            threeOfAKind++;
        } else if (ranks[i] == 4) {
            fourOfAKind++;
        }
    }

    if (fourOfAKind == 1) {
        return 8; // Four of a kind
    } else if (threeOfAKind == 1 && pairs == 1) {
        return 7; // Full House
    } else if (flush(hand)) {
        return 6; // Flush
    } else if (straight(hand)) {
        return 5; // Straight
    } else if (threeOfAKind == 1) {
        return 4; // Three of a kind
    } else if (pairs == 2) {
        return 3; // Two pairs
    } else if (pairs == 1) {
        return 2; // One pair
    }

    return 1; // High card (default)
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw()
{
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextDatum(0);

    sprite.fillRect(4, 0, 75, 20, 0x122F);
    sprite.setTextColor(TFT_RED, 0x122F);

    sprite.loadFont(NotoSansBold15);
    sprite.drawString("SCORE:", 8, 3);
    sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
    sprite.drawString(String((int)score), 8, 24);

    sprite.setTextColor(TFT_MAGENTA, TFT_BLACK);
    sprite.drawString("BET: " + String(bet), 9, 42, 2);
    sprite.unloadFont();
    sprite.setTextColor(TFT_WHITE, TFT_BLACK);

    sprite.drawString("WIN:" + String((int)win), 10, 60);

    sprite.setTextColor(TFT_WHITE, 0x1C54);

    for (int i = 0; i < 8; i++) {
        sprite.fillRect(x[i], y[i], 116, 15, 0x1C54);
        sprite.drawString(txt[i] + " " + String((int)bet * multi[i]), x[i] + 8, y[i] + 4);
    }

    if (fase == 5 && handRank > 0) {
        sprite.drawRect(x[handRank - 1] + 1, y[handRank - 1] + 1, 114, 13, TFT_ORANGE);
        sprite.drawRect(x[handRank - 1], y[handRank - 1], 116, 15, TFT_ORANGE);
    }

    for (int i = 0; i < 5; i++) {
        sprite.pushImage(4 + (i * 64), 75, 60, 84, card[hand[i]]);
        // sprite.drawRoundRect(9+(i*104),84,96,134,8,TFT_BLACK);

        if (fase == 2) {

            sprite.drawString(String(held[i]), 9 + 48 + (i * 104), 230);
            if (held[i] == 1) sprite.fillRect(4 + (i * 64), 166, 60, 4, TFT_ORANGE);
            else sprite.fillRect(4 + (i * 64), 166, 60, 4, 0x1281);
            sprite.drawRect(4 + (chosen * 64) - 2, 73, 64, 88, TFT_ORANGE);
            sprite.drawRect(4 + (chosen * 64) - 3, 72, 66, 90, TFT_ORANGE);
        }
    }
    sprite.pushSprite(0, 0);
}

void doubleClick()
{
    if (fase == 2) {
        held[chosen] = !held[chosen];
        draw();
    }
} // doubleClick

void Click()
{
    if (fase == 2) {
        chosen++; if (chosen == 5) chosen = 0;
        draw();
    }

    if (fase == 5) {
        bet = bet * 2; if (bet > score) bet = 2;
        draw();
    }
}


void setup()
{
    pinMode(up, INPUT_PULLUP);
    pinMode(down, INPUT_PULLUP);
    pinMode(15, OUTPUT);
    digitalWrite(15, 1);

    tft.init();  // amoled lcd initialization
    tft.setRotation(1);
    sprite.createSprite(320, 170);
    sprite.setSwapBytes(1);
    for (int i = 0; i < 52; i++)
        deck[i] = i;

    randomSeed(analogRead(0));
    button.attachDoubleClick(doubleClick);
    button.attachClick(Click);

    ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, 160);
}



void readButtons()
{
    button.tick();
    if (digitalRead(up) == 0) {
        if (deb == 0) {
            deb = 1;
            if (fase == 2) fase = 3; else fase = 0;
        }
    } else deb = 0;
}

void loop()
{

    if (fase == 0) {
        if (started == 0) {
            timePassed = millis();
            started = 1;
            for (int i = 0; i < 5; i++)
                hand[i] = 52;
            draw();
        }

        if (millis() > timePassed + period) {
            started = 0;
            fase = 1;
        }

    }

    if (fase == 1) {
        deal++;
        score = score - bet;
        win = 0;
        chosen = 0;
        shuffle(deck, 52);
        // Deal 5 cards to the player
        for (int i = 0; i < 5; i++) {
            hand[i] = deck[i];
            held[i] = 0;
        }
        fase = 2;
        draw();
    }

    if (fase == 2) {
        readButtons();
    }

    if (fase == 3) {
        if (started2 == 0) {
            timePassed2 = millis();
            started2 = 1;
            for (int i = 0; i < 5; i++)
                if (!held[i])
                    hand[i] = 52;
            draw();
        }

        if (millis() > timePassed2 + period) {
            started2 = 0;
            fase = 4;
        }

    }

    if (fase == 4) {
        // Replace cards that are not held
        for (int i = 0; i < 5; i++) {
            if (!held[i]) {
                hand[i] = deck[i + 5];
            }
        }

        for (int i = 0; i < 5; i++)
            tmpHand[i] = hand[i];

        handRank = evaluateHand(hand);
        if (handRank > 0) {
            win = bet * multi[handRank - 1];
            score = score + win;
        }

        fase = 5;

        for (int i = 0; i < 5; i++)
            hand[i] = tmpHand[i];
        draw();
    }


    if (fase == 5)
        readButtons();

}
