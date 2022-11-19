#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//global common variables
int lives = -1, score = -1;
enum game_states {MENU, GAME, END_GAME};
int state = MENU;
std::vector<int> vhiscores;

#include "Global.h"
#include "CSprite.h"
#include "Background.h"
#include "GameEngine.h"

//class variables
GameEngine *pGame;
ScrollingBackground *scrollingBackground;
CSprite *player;

//functions
void NewGame();
void AddMeteor();

bool GameInitialize()
{
    pGame = new GameEngine("Space Shooter v2",640,180);
    if(pGame == nullptr) return false;

    pGame->SetFrameRate(30);

    return true;
}

void GameStart()
{
    pGame->loadAssets();
    pGame->playMusic("music",true);
    scrollingBackground = new ScrollingBackground("background", pGame->width, pGame->height, 1);
    sf::FloatRect rcBounds(0,0,pGame->width,pGame->height);
    player = new CSprite("player",sf::Vector2f(0,pGame->height/2), sf::Vector2f(0,0),4,rcBounds,BA_STOP);
    player->setNumFrames(2);
    player->SetName("player");
    pGame->AddSprite(player);

    ReadHiScores(vhiscores);
    NewGame();
}

void GameEnd()
{
    WriteHiScores(vhiscores);
    pGame->stopMusic("music");
    delete scrollingBackground;

    pGame->CleanupAll();
    pGame->window.close();
    delete pGame;
}

void GameActivate()
{
    pGame->continueMusic("music");
}

void GameDeactivate()
{
    pGame->pauseMusic("music");
}

void GamePaint(sf::RenderWindow &window)
{
    scrollingBackground->Draw(window);

    switch(state)
    {
    case MENU:
        {
            //show hi scores
            std::string histr="SPACE SHOOTER\n  HI-SCORES\n";
            for(int i=0;i<5;i++)
            {
                histr = histr + "     " + std::to_string(vhiscores[i]) + "\n";
            }
            histr += "PRESS S TO START";
            pGame->Text(histr, 280, 20, sf::Color::Cyan, 24, "font", window);
            break;
        }
    case GAME:
        {
            //draw the score
            std::string sc = "LIVES: " + std::to_string(lives) + "    SCORE:  " + std::to_string(score);
            pGame->Text(sc,450,0,sf::Color::Cyan, 24, "font", window);

            pGame->DrawSprites(window);
            break;
        }
    case END_GAME:
        {
            pGame->Text("GAME OVER", 220,30, sf::Color::Cyan, 50, "font", window);
            pGame->Text("PRESS M", 250,100, sf::Color::Cyan, 25, "font", window);
            break;
        }
    default:
        break;
    }
    window.display();
}

void GameCycle(sf::Time delta)
{
    if( state == GAME )
    {
        scrollingBackground->Update();

        if( rnd.getRndInt(0,100) <= 1 )
        {
            sf::FloatRect rcBounds(0,0,pGame->width, pGame->height);
            CSprite* asteroid = new CSprite("asteroid",sf::Vector2f(pGame->width,rnd.getRndInt(0,pGame->height)), sf::Vector2f(-80,0), 3, rcBounds, BA_DIE);
            asteroid->SetName("asteroid");
            asteroid->setNumFrames(2);
            asteroid->setFrameDelay(10);
            pGame->AddSprite(asteroid);
        }

        pGame->UpdateSprites(delta);
    }
}

void HandleKeys()
{
    switch(state)
    {
    case MENU:
        {
            if( pGame->KeyPressed(sf::Keyboard::S) )
            {
                state = GAME;
                NewGame();
            }
            break;
        }
    case GAME:
        {
        //space is the fire key
        if( pGame->KeyPressed(sf::Keyboard::Space) )
        {
            sf::FloatRect rcBounds(0,0,pGame->width,pGame->height);
            float shotx = player->GetPosition().left + player->GetWidth();
            float shoty = player->GetPosition().top;
            CSprite* bullet = new CSprite("shot",sf::Vector2f(shotx,shoty),sf::Vector2f(360,0),2,rcBounds,BA_DIE);
            bullet->setNumFrames(2);
            bullet->SetName("bullet");
            pGame->AddSprite(bullet);
            pGame->playSound("laser");
        }

        sf::Vector2f vel = player->GetVelocity();
        if( pGame->KeyPressed(sf::Keyboard::Right) || pGame->KeyHeld(sf::Keyboard::Right) ) vel.x == 150 ? vel.x = 150 : vel.x += 5;
        if( pGame->KeyPressed(sf::Keyboard::Left) || pGame->KeyHeld(sf::Keyboard::Left) ) vel.x == -150 ? vel.x = -150 : vel.x -= 5;
        if( pGame->KeyPressed(sf::Keyboard::Up) || pGame->KeyHeld(sf::Keyboard::Up) ) vel.y == -150 ? vel.y = -150 : vel.y -= 5;
        if( pGame->KeyPressed(sf::Keyboard::Down) || pGame->KeyHeld(sf::Keyboard::Down) ) vel.y == 150 ? vel.y = 150 : vel.y += 5;
        player->SetVelocity(vel);
        break;
        }
    case END_GAME:
        {
            if( pGame->KeyPressed(sf::Keyboard::M) ) state = MENU;
            break;
        }
    default:
        break;
    }
}

void MouseButtonDown(int x, int y, bool bLeft)
{
}

void MouseButtonUp(int x, int y, bool bLeft)
{

}

void MouseMove(int x, int y)
{

}

bool SpriteCollision(CSprite* pSpriteHitter, CSprite* pSpriteHittee)
{
    std::string Hitter = pSpriteHitter->GetName();
    std::string Hittee = pSpriteHittee->GetName();

    if( Hitter == "asteroid" && Hittee == "bullet" )
    {
        pSpriteHittee->SetHidden(true);
        pSpriteHitter->SetHidden(true);
        pSpriteHittee->Kill();
        pSpriteHitter->Kill();

        //explosion
        sf::FloatRect rcBounds(0,0,pGame->width, pGame->height);
        CSprite* expl = new CSprite("explosion",rcBounds, BA_DIE);
        expl->SetPosition(pSpriteHitter->GetPosition());
        expl->OffsetPosition(-8,-8);
        expl->setFrameDelay(30);
        expl->setNumFrames(1,true);
        expl->SetZOrder(0);
        pGame->AddSprite(expl);
        pGame->playSound("explosion+3");
        score += 10;
    }

    if( Hitter == "asteroid" && Hittee == "player" )
    {
        pSpriteHitter->SetHidden(true);
        pSpriteHitter->Kill();

        //ship explosion
        //explosion
        sf::FloatRect rcBounds(0,0,pGame->width, pGame->height);
        CSprite* expl = new CSprite("explosion",rcBounds, BA_DIE);
        expl->SetPosition(pSpriteHitter->GetPosition());
        expl->OffsetPosition(-8,-8);
        expl->setFrameDelay(30);
        expl->setNumFrames(1,true);
        expl->SetZOrder(0);
        pGame->AddSprite(expl);

        if( lives == 1 )
            pGame->playSound("explosion+6");
        else
            pGame->playSound("explosion+3");

        lives--;
        if(lives<=0)
        {
            //endgame
            UpdateHiScores(vhiscores, score);
            state = END_GAME;
        }

        //relocate the ship
        player->SetPosition(0,pGame->height/2);
        player->SetVelocity(0,0);
    }
    return false;
}

void SpriteDying(CSprite* pSprite)
{

}

void NewGame()
{
    lives = 3;
    score = 0;
}

void AddMeteor()
{
}
