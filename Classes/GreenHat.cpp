#pragma execution_character_set("utf-8")
#include "GreenHat.h"
#define database UserDefault::getInstance()
using namespace CocosDenshion;
USING_NS_CC;
bool GreenHat::twoP = false;
void GreenHat::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* GreenHat::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setAutoStep(false);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = GreenHat::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

//if player isn't moving, set arm and shooting hat visible so player can shoot hat
void GreenHat::update(float dt) {
	this->getScene()->getPhysicsWorld()->step(1 / 100.0f);
	if (dtime == 0) GameOver();
}

// add backgrounds, scores, time counter
void GreenHat::addSprite() {
	// add background
	auto bgSprite = Sprite::create("Grassland.jpg");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);

	// scores 
	//if (database->getStringForKey("higheststr") != "") {
	//	scorePlayer1 = Label::createWithTTF("Highest score: " + database->getStringForKey("higheststr"), "fonts/arial.TTF", 30);
	//	scorePlayer1->setColor(Color3B(0, 0, 0));
	//	scorePlayer1->setPosition(visibleSize.width/2, visibleSize.height - 100);
	//	this->addChild(scorePlayer1, 3);
	//}

	scorePlayer2 = Label::createWithTTF("Player2 score: " + score1, "fonts/arial.TTF", 30);
	scorePlayer2->setColor(Color3B(0, 0, 0));
	scorePlayer2->setPosition(visibleSize.width/2 + 300, visibleSize.height - 50);
	this->addChild(scorePlayer2, 3);

	scorePlayer1 = Label::createWithTTF("Player1 score: " + score2, "fonts/arial.TTF", 30);
	scorePlayer1->setColor(Color3B(0, 0, 0));
	scorePlayer1->setPosition(visibleSize.width/2 - 400, visibleSize.height - 50);
	this->addChild(scorePlayer1, 3);

	// time counter
	nowtime = Label::createWithTTF("60S", "fonts/arial.TTF", 50);
	nowtime->setColor(Color3B(0, 0, 0));
	nowtime->setPosition(visibleSize.width / 2, visibleSize.height - nowtime->getContentSize().height);
	this->addChild(nowtime, 3);

	// worldline
	Vec2* points = new Vec2[4];
	points[0] = Vec2(-visibleSize.width / 2, visibleSize.height * 0.6);
	points[1] = Vec2(visibleSize.width / 2, visibleSize.height * 0.6);
	points[2] = Vec2(visibleSize.width / 2, -visibleSize.height / 2 + 70);
	points[3] = Vec2(-visibleSize.width / 2, -visibleSize.height / 2 + 70);
	auto body = PhysicsBody::createEdgePolygon(points, 4, PhysicsMaterial(1.0f, 0.0f, 1.0f));
	body->setCategoryBitmask(0xFFFFFFFF);
	body->setCollisionBitmask(0xFFFFFFFF);
	body->setContactTestBitmask(0xFFFFFFFF);
	edgeNode = Node::create();
	edgeNode->setPosition(visibleSize / 2);
	edgeNode->setPhysicsBody(body);
	this->addChild(edgeNode);
}

// add players, arms and shooting hats
void GreenHat::addPlayer() {
	IsPlayer1Jump = false;
	IsPlayer1Left = false;
	IsPlayer1Right = false;
	LastPlayer1Press = 'D';

	//create player1
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("GreenHatA.png");
	frame1 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 125, 130)));
	player1 = Sprite::createWithSpriteFrame(frame1);
	player1->setPhysicsBody(PhysicsBody::createBox(player1->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.001f)));
	int xpos = twoP ? 100 : visibleSize.width / 2;
	player1->setPosition(Vec2(xpos, 185));
	player1->getPhysicsBody()->setCategoryBitmask(1);//0001
	player1->getPhysicsBody()->setCollisionBitmask(8);//1000
	player1->getPhysicsBody()->setContactTestBitmask(8);//1000
	player1->getPhysicsBody()->setAngularVelocityLimit(0);
	player1->getPhysicsBody()->setRotationEnable(false);
	player1->setName("player1");
	this->addChild(player1, 2);
	IdleWithBox1 = frame1;

	//把帽子直接P在手上，放开的时候创建新的粗来，不然太不稳定了

	IsPlayer2Jump = false;
	IsPlayer2Left = false;
	IsPlayer2Right = false;
	LastPlayer2Press = 'A';

	//create player2
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("GreenHatB.png");
	frame2 = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 125, 130)));
	player2 = Sprite::createWithSpriteFrame(frame2);
	player2->setPhysicsBody(PhysicsBody::createBox(player2->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.001f)));
	xpos = twoP ? visibleSize.width - 100 : visibleSize.width / 2;
	player2->setPosition(Vec2(xpos, 185));
	player2->getPhysicsBody()->setCategoryBitmask(2);//0010
	player2->getPhysicsBody()->setCollisionBitmask(8);//1000
	player2->getPhysicsBody()->setContactTestBitmask(8);//1000
	player2->getPhysicsBody()->setAngularVelocityLimit(0);
	player2->getPhysicsBody()->setRotationEnable(false);
	player2->setName("player2");
	player2->setFlippedX(true);
	this->addChild(player2, 2);
	IdleWithBox2 = frame2;

	if (!twoP) {
		player2->setVisible(false);
	}
	loadAnimation();
}

// add Listenre about collapse and movement
void GreenHat::addListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GreenHat::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(GreenHat::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GreenHat::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// on "init" you need to initialize your instance
// getHat -> hat hit player || score -> game is coming to over
bool GreenHat::init() {
	//////////////////////////////
	// 1. super init first
	if (!Layer::init()) {
		return false;
	}
	auto scene = Scene::createWithPhysics();
	visibleSize = Director::getInstance()->getVisibleSize();
	twoP = database->getBoolForKey("twoP");
	dtime = 60;
	score1 = score2 = 0;
	addSprite();
	addListener();
	addPlayer();
	schedule(schedule_selector(GreenHat::update), 0.01, kRepeatForever, 0.1);
	schedule(schedule_selector(GreenHat::updateTime), 1.0, kRepeatForever, 0);
	detaH = height = 0;
	SimpleAudioEngine::getInstance()->preloadEffect("getHat.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("score.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("drop.mp3");
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("bgm.mp3");
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("gameover.mp3");
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("gameover.mp3");
	SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", true);
	return true;
}

// time counter
void GreenHat::updateTime(float dt) {
	string t = "";
	char second[5];
	if (dtime != 60) {
		_itoa(dtime % 60, second, 10);
		if (dtime % 60 < 10)
			t += '0';
	}
	else _itoa(dtime, second, 10);
	t += second;
	t += "S";
	nowtime->setString(t);
	--dtime;
}

// control of players
void GreenHat::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	Action* animation;
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("run1")));
		animation->setTag(11);
		player1->runAction(animation);
		if (code == EventKeyboard::KeyCode::KEY_A) {
			IsPlayer1Left = true;
			IsPlayer1Right = false;
			if (LastPlayer1Press == 'D') {
				player1->setFlipX(true);
			}
			LastPlayer1Press = 'A';
			player1->getPhysicsBody()->setVelocity(Vec2(-300, player1->getPhysicsBody()->getVelocity().y));
		}
		else {
			IsPlayer1Right = true;
			IsPlayer1Left = false;
			if (LastPlayer1Press == 'A') {
				player1->setFlipX(false);
			}
			LastPlayer1Press = 'D';
			player1->getPhysicsBody()->setVelocity(Vec2(300, player1->getPhysicsBody()->getVelocity().y));
		}
		if (IsPlayer1Jump == true) {
			player1->stopAllActions();
		}
		break;

	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		if (IsPlayer1Jump) break;
		player1->stopAllActions();
		player1->getPhysicsBody()->setVelocity(Vec2(0, 300));
		IsPlayer1Jump = true;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		if (IsPlayer1Jump || IsPlayer1Left || IsPlayer1Right) break;
		//shot a hat out(speed is focused and so as triangle)
		throwHat("player1", LastPlayer1Press);
		break;
	}
	// player2
	if (twoP) {
		switch (code) {
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("run2")));
			animation->setTag(12);
			player2->runAction(animation);
			if (code == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
				IsPlayer2Left = true;
				IsPlayer2Right = false;
				if (LastPlayer2Press == 'D')
					player2->setFlipX(true);
				LastPlayer2Press = 'A';
				player2->getPhysicsBody()->setVelocity(Vec2(-300, player2->getPhysicsBody()->getVelocity().y));
			}
			else {
				IsPlayer2Right = true;
				IsPlayer2Left = false;
				if (LastPlayer2Press == 'A')
					player2->setFlipX(false);
				LastPlayer2Press = 'D';
				player2->getPhysicsBody()->setVelocity(Vec2(300, player2->getPhysicsBody()->getVelocity().y));
			}
			if (IsPlayer2Jump == true) {
				player2->stopAllActions();
			}
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
			if (IsPlayer2Jump) break;
			player2->stopAllActions();
			player2->getPhysicsBody()->setVelocity(Vec2(0, 300));
			IsPlayer2Jump = true;
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			if (IsPlayer2Jump || IsPlayer2Left || IsPlayer2Right) break;
			throwHat("player2", LastPlayer2Press);
			break;
		}
	}
}

// control of players
void GreenHat::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		if (code == EventKeyboard::KeyCode::KEY_A)
			IsPlayer1Left = false;
		else IsPlayer1Right = false;
		player1->stopAllActionsByTag(11);
		player1->getPhysicsBody()->setVelocity(Vec2(0, player1->getPhysicsBody()->getVelocity().y));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		if (code == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
			IsPlayer2Left = false;
		else IsPlayer2Right = false;
		player2->stopAllActionsByTag(12);
		player2->getPhysicsBody()->setVelocity(Vec2(0, player2->getPhysicsBody()->getVelocity().y));
		break;
	}
}

//player throw hat and hat fly
bool GreenHat::throwHat(string player, char dir) {
	double d = dir == 'A' ? -1.0 : 1.0;
	SimpleAudioEngine::getInstance()->playEffect("drop.mp3", false);
	if (player == "player1") {
		hat1 = Sprite::createWithSpriteFrame(SpriteFrame::create("hat.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 274, 166))));
		hat1->setScale(0.2);
		hat1->setPosition(player1->getPosition() + Vec2(120 * d, 10));
		hat1->setPhysicsBody(PhysicsBody::createBox(hat1->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.3f)));
		hat1->getPhysicsBody()->setVelocity(Vec2(200 * d, 380));
		hat1->getPhysicsBody()->setCategoryBitmask(8);//1000
		hat1->getPhysicsBody()->setCollisionBitmask(3);//0011
		hat1->getPhysicsBody()->setContactTestBitmask(3);//0011
		hat1->getPhysicsBody()->setAngularVelocityLimit(0);
		hat1->getPhysicsBody()->setRotationEnable(true);
		hat1->setName("hat1");
		this->addChild(hat1, 4);
	}
	else {
		hat2 = Sprite::createWithSpriteFrame(SpriteFrame::create("hat.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 274, 166))));
		hat2->setScale(0.2);
		hat2->setPosition(player2->getPosition() + Vec2(120 * d, 10));
		hat2->setPhysicsBody(PhysicsBody::createBox(hat2->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.3f)));
		hat2->getPhysicsBody()->setVelocity(Vec2(200 * d, 380));
		hat2->getPhysicsBody()->setCategoryBitmask(8);//1000
		hat2->getPhysicsBody()->setCollisionBitmask(3);//0011
		hat2->getPhysicsBody()->setContactTestBitmask(3);//0011
		hat2->getPhysicsBody()->setAngularVelocityLimit(0);
		hat2->getPhysicsBody()->setRotationEnable(true);
		hat2->setName("hat2");
		this->addChild(hat2, 4);
	}
	return false;
}


// contorl of collapse
bool GreenHat::onConcactBegin(PhysicsContact & contact) {
	auto shapeA = contact.getShapeA();
	auto shapeB = contact.getShapeB();

	//player1 without hat
	if (shapeA->getCategoryBitmask() == 1 || shapeB->getCategoryBitmask() == 1) {
		//player1 get contact with ground
		if (shapeB->getCategoryBitmask() == 0xFFFFFFFF || shapeA->getCategoryBitmask() == 0xFFFFFFFF) {
			IsPlayer1Jump = false;
		}
		//player1 get contact with simple hat
		if (shapeA->getCategoryBitmask() == 8) {//shapeA is hat shapeB is player1
			if (shapeA->getBody()->getPosition().y - 16.6 < player1->getPosition().y + 65) return false;
			SimpleAudioEngine::getInstance()->playEffect("getHat.mp3", false);
			shapeB->setCategoryBitmask(16);//10000
			shapeB->setCollisionBitmask(16);//10000
			shapeB->setContactTestBitmask(16);//10000
			shapeA->getBody()->setVelocity(player1->getPosition() - shapeA->getBody()->getPosition() + Vec2(0, 65));
			shapeA->setCategoryBitmask(9);//1001
			shapeA->setCollisionBitmask(8);//1000
			shapeA->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(player1->getPhysicsBody(), shapeA->getBody(), shapeA->getBody()->getPosition());
			m_world->addJoint(joint);
			score2++;
			CCString* s = CCString::createWithFormat("%d", score2);
			scorePlayer2->setString("Player2 score: " + s->_string);
		} else if(shapeB->getCategoryBitmask() == 8) {//shapeA is player1 shapeB is hat
			if (shapeB->getBody()->getPosition().y - 16.6 < player1->getPosition().y + 65) return false;
			SimpleAudioEngine::getInstance()->playEffect("getHat.mp3", false);
			shapeA->setCategoryBitmask(16);//10000
			shapeA->setCollisionBitmask(16);//10000
			shapeA->setContactTestBitmask(16);//10000
			shapeB->getBody()->setVelocity(player1->getPosition() - shapeB->getBody()->getPosition() + Vec2(0, 65));
			shapeB->setCategoryBitmask(9);//1001
			shapeB->setCollisionBitmask(8);//1000
			shapeB->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(player1->getPhysicsBody(), shapeB->getBody(), shapeB->getBody()->getPosition());
			m_world->addJoint(joint);
			score2++;
			CCString* s = CCString::createWithFormat("%d", score2);
			scorePlayer2->setString("Player2 score: " + s->_string);
		}
	}

	//player1 with hat on head get contact with ground
	if (shapeA->getCategoryBitmask() == 16 || shapeB->getCategoryBitmask() == 16) {
		if (shapeB->getCategoryBitmask() == 0xFFFFFFFF || shapeA->getCategoryBitmask() == 0xFFFFFFFF) {
			IsPlayer1Jump = false;
		}
	}

	//hat on player1's head get contact with hat
	if (shapeA->getCategoryBitmask() == 9 || shapeB->getCategoryBitmask() == 9) {
		if (shapeA->getCategoryBitmask() == 8) {//shapeA is hat shapeB is player1's highest hat
			if (shapeA->getBody()->getPosition().y - 16.6 < shapeB->getBody()->getPosition().y +16.6) return false;
			SimpleAudioEngine::getInstance()->playEffect("getHat.mp3", false);
			shapeB->setCategoryBitmask(4);//0100
			shapeB->setCollisionBitmask(64);//1000000
			shapeB->setContactTestBitmask(64);//10000000
			shapeA->getBody()->setVelocity(shapeB->getBody()->getPosition() - shapeA->getBody()->getPosition() + Vec2(0, 16.6));
			shapeA->setCategoryBitmask(9);//1001
			shapeA->setCollisionBitmask(8);//1000
			shapeA->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(shapeB->getBody(), shapeA->getBody(), shapeA->getBody()->getPosition());
			m_world->addJoint(joint);
			score2++;
			CCString* s = CCString::createWithFormat("%d", score2);
			scorePlayer2->setString("Player2 score: " + s->_string);
		}
		else if (shapeB->getCategoryBitmask() == 8) {//shapeA is player1's highest hat shapeB is hat
			if (shapeB->getBody()->getPosition().y - 16.6 < shapeA->getBody()->getPosition().y + 16.6) return false;
			SimpleAudioEngine::getInstance()->playEffect("getHat.mp3", false);
			shapeA->setCategoryBitmask(4);//0100
			shapeA->setCollisionBitmask(64);//1000000
			shapeA->setContactTestBitmask(64);//1000000
			shapeB->getBody()->setVelocity(shapeA->getBody()->getPosition() - shapeB->getBody()->getPosition() + Vec2(0, 16.6));
			shapeB->setCategoryBitmask(9);//1001
			shapeB->setCollisionBitmask(8);//1000
			shapeB->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(shapeA->getBody(), shapeB->getBody(), shapeB->getBody()->getPosition());
			m_world->addJoint(joint);
			score2++;
			CCString* s = CCString::createWithFormat("%d", score2);
			scorePlayer2->setString("Player2 score: " + s->_string);
		}
	}

	//player2 without hat
	if (shapeA->getCategoryBitmask() == 2 || shapeB->getCategoryBitmask() == 2) {
		if (shapeB->getCategoryBitmask() == 0xFFFFFFFF || shapeA->getCategoryBitmask() == 0xFFFFFFFF) {
			IsPlayer2Jump = false;
		}
		//player2 get contact with simple hat
		if (shapeA->getCategoryBitmask() == 8) {//shapeA is hat shapeB is player1
			SimpleAudioEngine::getInstance()->playEffect("getHat.mp3", false);
			shapeB->setCategoryBitmask(32);//100000
			shapeB->setCollisionBitmask(32);//100000
			shapeB->setContactTestBitmask(32);//100000
			shapeA->getBody()->setVelocity(player2->getPosition() - shapeA->getBody()->getPosition() + Vec2(0, 65));
			shapeA->setCategoryBitmask(10);//1010
			shapeA->setCollisionBitmask(8);//1000
			shapeA->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(player2->getPhysicsBody(), shapeA->getBody(), player2->getPosition() + Vec2(0, 65));
			m_world->addJoint(joint);
			score1++;
			CCString* s = CCString::createWithFormat("%d", score1);
			scorePlayer1->setString("Player1 score: " + s->_string);
		}
		else if (shapeB->getCategoryBitmask() == 8) {//shapeA is player1 shapeB is hat
			shapeA->setCategoryBitmask(32);//100000
			shapeA->setCollisionBitmask(32);//100000
			shapeA->setContactTestBitmask(32);//100000
			shapeB->getBody()->setVelocity(player2->getPosition() - shapeB->getBody()->getPosition() + Vec2(0, 65));
			shapeB->setCategoryBitmask(10);//1000
			shapeB->setCollisionBitmask(8);//1000
			shapeB->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(player2->getPhysicsBody(), shapeB->getBody(), player2->getPosition() + Vec2(0, 65));
			m_world->addJoint(joint);
			score1++;
			CCString* s = CCString::createWithFormat("%d", score1);
			scorePlayer1->setString("Player1 score: " + s->_string);
		}
	}

	//player2 with hat on head get contact with ground
	if (shapeA->getCategoryBitmask() == 32 || shapeB->getCategoryBitmask() == 32) {
		if (shapeB->getCategoryBitmask() == 0xFFFFFFFF || shapeA->getCategoryBitmask() == 0xFFFFFFFF) {
			IsPlayer2Jump = false;
		}
	}

	//hat on player2's head get contact with hat
	if (shapeA->getCategoryBitmask() == 10 || shapeB->getCategoryBitmask() == 10) {
		if (shapeA->getCategoryBitmask() == 8) {//shapeA is hat shapeB is player2's highest hat
			if (shapeA->getBody()->getPosition().y - 16.6 < shapeB->getBody()->getPosition().y + 16.6) return false;
			SimpleAudioEngine::getInstance()->playEffect("getHat.mp3", false);
			shapeB->setCategoryBitmask(4);//0100
			shapeB->setCollisionBitmask(64);//1000000
			shapeB->setContactTestBitmask(64);//10000000
			shapeA->getBody()->setVelocity(shapeB->getBody()->getPosition() - shapeA->getBody()->getPosition() + Vec2(0, 16.6));
			shapeA->setCategoryBitmask(10);//1010
			shapeA->setCollisionBitmask(8);//1000
			shapeA->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(shapeB->getBody(), shapeA->getBody(), shapeB->getBody()->getPosition() + Vec2(0, 16.6));
			m_world->addJoint(joint);
			score1++;
			CCString* s = CCString::createWithFormat("%d", score1);
			scorePlayer1->setString("Player1 score: " + s->_string);
		}
		else if (shapeB->getCategoryBitmask() == 8) {//shapeA is player2's highest hat shapeB is hat
			if (shapeB->getBody()->getPosition().y - 16.6 < shapeA->getBody()->getPosition().y + 16.6) return false;
			SimpleAudioEngine::getInstance()->playEffect("getHat.mp3", false);
			shapeA->setCategoryBitmask(4);//0100
			shapeA->setCollisionBitmask(64);//1000000
			shapeA->setContactTestBitmask(64);//1000000
			shapeB->getBody()->setVelocity(shapeA->getBody()->getPosition() - shapeB->getBody()->getPosition() + Vec2(0, 16.6));
			shapeB->setCategoryBitmask(10);//1010
			shapeB->setCollisionBitmask(8);//1000
			shapeB->setContactTestBitmask(8);//1000
			auto joint = PhysicsJointFixed::construct(shapeA->getBody(), shapeB->getBody(), shapeB->getBody()->getPosition() + Vec2(0, 16.6));
			m_world->addJoint(joint);
			score1++;
			CCString* s = CCString::createWithFormat("%d", score1);
			scorePlayer1->setString("Player1 score: " + s->_string);
		}
	}

	//hat get contact with ground and disapper
	if (shapeA->getCategoryBitmask() == 8 || shapeB->getCategoryBitmask() == 8) {
		if (shapeB->getCategoryBitmask() == 0xFFFFFFFF) return false;
		else  if (shapeA->getCategoryBitmask() == 0xFFFFFFFF) return false;
	}
	return true;
}

// animation
void GreenHat::loadAnimation() {
	cocos2d::Vector<SpriteFrame*> runplayer1;
	cocos2d::Vector<SpriteFrame*> runplayer2;
	// player1 running(frame：8，height：130，width：125）
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("GreenHatA.png");
	runplayer1.reserve(11);
	for (int i = 0; i < 11; i++) {
		auto frame = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(125 * i, 0, 125, 130)));
		runplayer1.pushBack(frame);
	}
	auto animationRun1 = Animation::createWithSpriteFrames(runplayer1, 0.05f);
	AnimationCache::getInstance()->addAnimation(animationRun1, "run1");

	//player2运动动画
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("GreenHatB.png");
	runplayer2.reserve(11);
	for (int i = 0; i < 11; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(125 * i, 0, 125, 130)));
		runplayer2.pushBack(frame);
	}
	auto animationRun2 = Animation::createWithSpriteFrames(runplayer2, 0.05f);
	AnimationCache::getInstance()->addAnimation(animationRun2, "run2");
}

// play again or exit choosing
void GreenHat::GameOver() {
	dtime = -1;
	unschedule(schedule_selector(GreenHat::updateTime));
	m_world->removeBody(edgeNode->getPhysicsBody());
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
	SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);
	if (database->getIntegerForKey("highestint") < score1 || database->getIntegerForKey("highestint") < score2) {
		database->setIntegerForKey("highestint", score1 > score2 ? score1 : score2);
		CCString* s = CCString::createWithFormat("%d", score1 > score2 ? score1 : score2);
		database->setStringForKey("higheststr", s->_string);
	}
	auto bgSprite = Sprite::create("speciallove.jpg");
	bgSprite->setPosition(visibleSize / 2);
	bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
	this->addChild(bgSprite, 0);
	auto label = Label::createWithTTF("Again", "fonts/arial.TTF", 40);
	label->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GreenHat::clickCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);
	label = Label::createWithTTF("Exit", "fonts/arial.TTF", 40);
	label->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GreenHat::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
}

void GreenHat::clickCallback(Ref * pSender) {
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("gameover.mp3");
	Director::getInstance()->replaceScene(GreenHat::createScene());
}

void GreenHat::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}