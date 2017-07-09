#ifndef __GREEN_HAT_H__
#define __GREEN_HAT_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
using namespace std;
USING_NS_CC;

class GreenHat : public Layer {
public:
	static PhysicsWorld* world;
	static bool twoP;
	static cocos2d::Scene* createScene();
	void setPhysicsWorld(PhysicsWorld * world);
	virtual bool init();
	void addSprite();
	void addPlayer();
	void addListener();
	void update(float dt);
	void updateTime(float dt);
	void onKeyPressed(EventKeyboard::KeyCode code, Event * event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event * event);
	bool onConcactBegin(PhysicsContact & contact);
	void loadAnimation();
	void GameOver();
	void clickCallback(Ref * pSender);
	void exitCallback(Ref * pSender);
	bool throwHat(string player, char dir);
	// implement the "static create()" method manually
	CREATE_FUNC(GreenHat);
private:
	int dtime;
	int score1;
	int score2;
	Label* nowtime;
	Label* scorePlayer1;
	Label* scorePlayer2;
	PhysicsWorld* m_world;
	Size visibleSize;
	Node* edgeNode;
	Sprite* player1;
	Sprite* player2;
	Sprite* hat1;
	Sprite* hat2;
	SpriteFrame* frame1;
	SpriteFrame* frame2;
	SpriteFrame* frame3;
	SpriteFrame* IdleWithBox1;
	SpriteFrame* IdleWithBox2;
	list<PhysicsBody*> player1Hat;
	list<PhysicsBody*> player2Hat;
	char LastPlayer1Press;
	char LastPlayer2Press;
	bool IsPlayer1Jump;
	bool IsPlayer2Jump;
	bool IsPlayer1Left;
	bool IsPlayer2Left;
	bool IsPlayer1Right;
	bool IsPlayer2Right;
	double detaH;
	double height;
};
#endif // __GREEN_HAT_H__