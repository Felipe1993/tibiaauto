// Alarm.cpp: implementation of the Alarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_autogo.h"
#include "Alarm.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include "IPCBackPipeProxy.h"
#include "TibiaContainer.cpp" //What am I missing???  Why do I have to include a .cpp to get this to compile???? Other modules call the .h and work fine!
#include "ModuleUtil.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Alarm::Alarm() {
	alarmDescriptor = "<none>";
	spell = "";
	spellDelay = 0;
	manaCost = 0;
	sound = "";
	logEvents = false;
	windowAction = -1;
	shutdown = false;
	killTibia = false;
	logout = false;
	depot = false;
	start = false;
	runaway = true;
	screenshot = 0;
	intTrigger = -1;
	strTrigger = "";
	condition = 0;
	attribute = 0;
	alarmType = 0;
	stopWalking = false;

// execution markers only, no need to save orecord here
	onScreenAt = 0;
	spellCast = 0;
	screenshotsTaken = 0;
	timeLastSS = 0;
	modulesStarted = false;
	modulesSuspended = false;
	eventLogged = false;
	windowActed = false;
	flashed = false;
	halfSleep = false;
	stopWalk = false;
	currentLoc=point(0,0,0);
	movedTime=time(NULL);
}



Alarm::Alarm(int type, int attr, int cond, int trig, CString strTrig, bool run, bool sta, bool dep, CString spe, int cost, int delay, int scr, bool stopwalk, bool log, bool kill, bool shut, int winAct, CString audio, bool event, list<CString> beginModules, list<CString> endModules) {
	alarmDescriptor = "";
	startModules = beginModules;
	stopModules = endModules;
	spell = spe;
	spellDelay = delay;
	manaCost = cost;
	sound = audio;
	logEvents = event;
	windowAction = winAct;
	shutdown = shut;
	killTibia = kill;
	logout = log;
	depot = dep;
	start = sta;
	runaway = run;
	screenshot = scr;
	intTrigger = trig;
	strTrigger=strTrig;
	condition = cond;
	attribute = attr;
	alarmType = type;
	stopWalking = stopwalk;
	
	onScreenAt = 0;
	spellCast = 0;
	screenshotsTaken = 0;
	timeLastSS = 0;
	modulesStarted = false;
	modulesSuspended = false;
	eventLogged = false;
	windowActed = false;
	flashed = false;
	halfSleep = false;
	stopWalk = false;
	currentLoc=point(0,0,0);
	movedTime=time(NULL);
}

Alarm::~Alarm(){

}

//////////////////////////////////////////////////////////////////////
// Setters
//////////////////////////////////////////////////////////////////////

void Alarm::setType(int type) {
	alarmType = type;
}

void Alarm::setAttribute(int attrib) {
	attribute = attrib;
}

void Alarm::setCondition(int cond) {
	condition = cond;
}

void Alarm::setTrigger(int trig) {
	intTrigger = trig;
}

void Alarm::setTrigger(CString trig) {
	strTrigger = trig;
}
void Alarm::setStartModules(list<CString> beginModules) {
	startModules = beginModules;
}

void Alarm::setStopModules(list<CString> endModules) {
	stopModules = endModules;
}

void Alarm::setGoToRunaway(bool onOff) {
	runaway = onOff;
}

void Alarm::setGoToStart(bool onOff) {
	start = onOff;
}

void Alarm::setGoToDepot(bool onOff) {
	depot = onOff;
}

void Alarm::setCastSpell(CString spellToCast) {
	spell = spellToCast;
}

void Alarm::setTakeScreenshot(int screenshotOption) {
	screenshot = screenshotOption;
}

void Alarm::setStopWalking(bool onOff) {
	stopWalking = onOff;
}

void Alarm::setLogout(bool onOff) {
	logout = onOff;
}

void Alarm::setKillClient(bool onOff) {
	killTibia = onOff;
}

void Alarm::setShutdownComputer(bool onOff) {
	shutdown = onOff;
}

void Alarm::setWindowAction(int windowActionOpt) {
	windowAction = windowActionOpt;
}

void Alarm::setAudioAlarm(CString waveToPlay) {
	sound = waveToPlay;
}

void Alarm::setLogEvents(bool onOff) {
	logEvents = onOff;
}

//////////////////////////////////////////////////////////////////////
// Actions/Accessors
//////////////////////////////////////////////////////////////////////

list<CString> Alarm::doStopModules() {
	return stopModules;
}

list<CString> Alarm::doStartModules() {
	return startModules;
}

bool Alarm::doLogout() {
	return logout;
}

bool Alarm::doKillClient() {
	return killTibia;
}

bool Alarm::doShutdownComputer() {
	return shutdown;
}

int Alarm::doWindowAction() {
	return windowAction;
}

CString Alarm::doAlarm() {
	return sound;
}

bool Alarm::doLogEvents() {
	return logEvents;
}

int Alarm::doTakeScreenshot() {
	return screenshot;
}

bool Alarm::doGoToRunaway() {
	return runaway;
}

bool Alarm::doGoToStart() {
	return start;
}

bool Alarm::doGoToDepot() {
	return depot;
}

CString Alarm::doCastSpell() {
	return spell;
}

bool Alarm::doStopWalking() {
	return stopWalking;
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

void Alarm::initializeCharacter() {
	//Initialize the character's starting info 
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	lvlInit = self->lvl;
	mlvlInit = self->mlvl;
	skillFishInit = self->skillFish;
	skillFistInit = self->skillFist;
	skillClubInit = self->skillClub;
	skillSwordInit = self->skillSword;
	skillAxeInit = self->skillAxe;
	skillDistInit = self->skillDist;
	skillShieldInit = self->skillShield;
	positionXInit = self->x;
	positionYInit = self->y;
	positionZInit = self->z;
	healthInit = self->hp;
	delete self;
}

bool Alarm::checkAlarm(char whiteList[100][32], int options) {
	bool retval = false;
	CMemReaderProxy reader;
	CTibiaCharacter *self = reader.readSelfCharacter();
	if (currentLoc.x!=self->x || currentLoc.y!=self->y || currentLoc.z!=self->z){
		currentLoc=point(self->x,self->y,self->z);
		movedTime=time(NULL);
	}

	switch (alarmType) {
	case SKILL:
		switch (condition) {
		case LEVELUP:
			switch (attribute) {
			case ALL:
				retval |= (self->lvl > lvlInit);
				retval |= (self->mlvl > mlvlInit);
				retval |= (self->skillFish > skillFishInit);
				retval |= (self->skillFist > skillFistInit);
				retval |= (self->skillClub > skillClubInit);
				retval |= (self->skillSword > skillSwordInit);
				retval |= (self->skillAxe > skillAxeInit);
				retval |= (self->skillDist > skillDistInit);
				retval |= (self->skillShield > skillShieldInit);
				lvlInit=self->lvl;
				mlvlInit=self->mlvl;
				skillFishInit=self->skillFish;
				skillFistInit=self->skillFist;
				skillClubInit=self->skillClub;
				skillSwordInit=self->skillSword;
				skillAxeInit=self->skillAxe;
				skillDistInit=self->skillDist;
				skillShieldInit=self->skillShield;
				break;
			case LEVEL:
				retval = self->lvl > lvlInit;
				lvlInit=self->lvl;
				break;
			case MAGICLVL:
				retval = self->mlvl > mlvlInit;
				mlvlInit=self->mlvl;
				break;
			case FISHING:
				retval = self->skillFish > skillFishInit;
				skillFishInit=self->skillFish;
				break;
			case FIST:
				retval = self->skillFist > skillFistInit;
				skillFistInit=self->skillFist;
				break;
			case CLUB:
				retval = self->skillClub > skillClubInit;
				skillClubInit=self->skillClub;
				break;
			case SWORDSKILL:
				retval=self->skillSword > skillSwordInit;
				skillSwordInit=self->skillSword;
				break;
			case AXE:
				retval = self->skillAxe > skillAxeInit;
				skillAxeInit=self->skillAxe;
				break;
			case DISTANCE:
				retval = self->skillDist > skillDistInit;
				skillDistInit=self->skillDist;
				break;
			case SHIELD:
				retval = self->skillShield > skillShieldInit;
				skillShieldInit=self->skillShield;
				break;
			}
			break;
		case PERCLVLACHIEVED: 
			{
			int lvlExp=(50*pow(self->lvl-1,3)-150*pow(self->lvl-1,2)+400*(self->lvl-1))/3;
			switch (attribute) {
			case ALL:
				retval |= (100 - (self->exp-lvlExp)/((self->lvl-1)*(self->lvl-2)*50+100) >= intTrigger);
				retval |= (100 - self->mlvlPercLeft >= intTrigger);
				retval |= (100 - self->skillFishPercLeft >= intTrigger);
				retval |= (100 - self->skillFistPercLeft >= intTrigger);
				retval |= (100 - self->skillClubPercLeft >= intTrigger);
				retval |= (100 - self->skillSwordPercLeft >= intTrigger);
				retval |= (100 - self->skillAxePercLeft >= intTrigger);
				retval |= (100 - self->skillDistPercLeft >= intTrigger);
				retval |= (100 - self->skillShieldPercLeft >= intTrigger);
				break;
			case LEVEL:
				retval=100 - (self->exp-lvlExp)/((self->lvl-1)*(self->lvl-2)*50+100) >= intTrigger;
				break;
			case MAGICLVL:
				retval=100 - self->mlvlPercLeft >= intTrigger;
				break;
			case FISHING:
				retval=100 - self->skillFishPercLeft >= intTrigger;
				break;
			case FIST:
				retval=100 - self->skillFistPercLeft >= intTrigger;
				break;
			case CLUB:
				retval=100 - self->skillClubPercLeft >= intTrigger;
				break;
			case SWORDSKILL:
				retval=100 - self->skillSwordPercLeft >= intTrigger;
				break;
			case AXE:
				retval=100 - self->skillAxePercLeft >= intTrigger;
				break;
			case DISTANCE:
				retval=100 - self->skillDistPercLeft >= intTrigger;
				break;
			case SHIELD:
				retval=100 - self->skillShieldPercLeft >= intTrigger;
				break;
			}
			break;
			}
		case PERCLVLREMAINING:
			{
			int lvlExp=(50 * pow(self->lvl-1,3)-150 * pow(self->lvl-1,2)+400*(self->lvl-1))/3;
			switch (attribute) {
			case ALL:
				retval |= ((self->exp-lvlExp)/((self->lvl-1)*(self->lvl-2)*50+100) <= intTrigger);
				retval |= (self->mlvlPercLeft <= intTrigger);
				retval |= (self->skillFishPercLeft <= intTrigger);
				retval |= (self->skillFistPercLeft <= intTrigger);
				retval |= (self->skillClubPercLeft <= intTrigger);
				retval |= (self->skillSwordPercLeft <= intTrigger);
				retval |= (self->skillAxePercLeft <= intTrigger);
				retval |= (self->skillDistPercLeft <= intTrigger);
				retval |= (self->skillShieldPercLeft <= intTrigger);
				break;
			case LEVEL:
				retval=(self->exp-lvlExp)/((self->lvl-1)*(self->lvl-2)*50+100) <= intTrigger;
				break;
			case MAGICLVL:
				retval=self->mlvlPercLeft <= intTrigger;
				break;
			case FISHING:
				retval=self->skillFishPercLeft <= intTrigger;
				break;
			case FIST:
				retval=self->skillFistPercLeft <= intTrigger;
				break;
			case CLUB:
				retval=self->skillClubPercLeft <= intTrigger;
				break;
			case SWORDSKILL:
				retval=self->skillSwordPercLeft <= intTrigger;
				break;
			case AXE:
				retval=self->skillAxePercLeft <= intTrigger;
				break;
			case DISTANCE:
				retval=self->skillDistPercLeft <= intTrigger;
				break;
			case SHIELD:
				retval=self->skillShieldPercLeft <= intTrigger;
				break;
			}
			break;
			}
		}
		break;
	case RESOURCE:
		switch (condition) {
		case EQUAL:
			switch(attribute) {
			case HP:
				retval=self->hp == intTrigger;
				break;
			case MP:
				retval=self->mana == intTrigger;
				break;
			case XP:
				retval=self->exp == intTrigger;
				break;
			case SP:
				retval=self->soulPoints == intTrigger;
				break;
			case STAMINA:
				retval=self->stamina == intTrigger;
				break;
			case CAPACITY:
				retval=self->cap == intTrigger;
				break;
			case SPACE:
				retval=spaceAvailable() == intTrigger;
				break;
			} 
			break;
		case LESS:
			switch(attribute) {
			case HP:
				retval= self->hp < intTrigger;
				break;
			case MP:
				retval= self->mana < intTrigger;
				break;
			case XP:
				retval= self->exp < intTrigger;
				break;
			case SP:
				retval= self->soulPoints < intTrigger;
				break;
			case STAMINA:
				retval= self->stamina < intTrigger;
				break;
			case CAPACITY:
				retval= self->cap < intTrigger;
				break;
			case SPACE:
				retval= spaceAvailable() < intTrigger;
				break;
			}
			break;
		case MORE:
			switch(attribute) {
			case HP:
				retval= self->hp > intTrigger;
				break;
			case MP:
				retval= self->mana > intTrigger;
				break;
			case XP:
				retval= self->exp > intTrigger;
				break;
			case SP:
				retval=self->soulPoints > intTrigger;
				break;
			case STAMINA:
				retval= self->stamina > intTrigger;
				break;
			case CAPACITY:
				retval= self->cap > intTrigger;
				break;
			case SPACE:
				retval= spaceAvailable() > intTrigger;
				break;
			}
			break;
		}
		break;
	case ITEMS:
		switch (condition) {
		case EQUAL:
			switch(attribute) {
			case 0:
				retval= countAllFood() == intTrigger;
				break;
			default:
				retval= countAllItemsOfType(attribute) == intTrigger;
				break;
			}
			break;
		case LESS:
			switch(attribute) {
			case 0:
				retval= countAllFood() < intTrigger;
				break;
			default:
				retval= countAllItemsOfType(attribute) < intTrigger;
				break;
			}
			break;
		case MORE:
			switch(attribute) {
			case 0:
				retval= countAllFood() > intTrigger;
				break;
			default:
				retval= countAllItemsOfType(attribute) > intTrigger;
				break;
			}
			break;
		}
		break;
	case PROXIMITY:
		switch (condition) {
		case NEARBY:
			switch(attribute) {
			case PLAYER:
				retval= playerOnScreen(whiteList, options);
				break;
			case MONSTER:
				retval= monsterOnScreen(whiteList, options);
				break;
			case GM:
				retval= gmOnScreen(whiteList, options);
				break;
			case BATTLELIST:
				retval= battleListActive();
				break;
			case BLACKSKULL:
				retval= skullOnScreen(BLACK_SKULL);
				break;
			case REDSKULL:
				retval= skullOnScreen(RED_SKULL);
				break;
			case GREENSKULL:
				retval= skullOnScreen(GREEN_SKULL);
				break;
			case YELLOWSKULL:
				retval= skullOnScreen(YELLOW_SKULL);
				break;
			case WHITESKULL:
				retval= skullOnScreen(WHITE_SKULL);
				break;
			}
			break;
		case DISAPPEARS:
			switch(attribute) {
			case PLAYER:
				retval= !playerOnScreen(whiteList, options);
				break;
			case MONSTER:
				retval= !monsterOnScreen(whiteList, options);
				break;
			case GM:
				retval= !gmOnScreen(whiteList, options);
				break;
			case BATTLELIST:
				retval= !battleListActive();
				break;
			case BLACKSKULL:
				retval= !skullOnScreen(BLACK_SKULL);
				break;
			case REDSKULL:
				retval= !skullOnScreen(RED_SKULL);
				break;
			case GREENSKULL:
				retval= !skullOnScreen(GREEN_SKULL);
				break;
			case YELLOWSKULL:
				retval= !skullOnScreen(YELLOW_SKULL);
				break;
			case WHITESKULL:
				retval= !skullOnScreen(WHITE_SKULL);
				break;
			}
			break;
		case ISONSCREENFOR: {
			bool onscreen;
			switch(attribute) {
			case PLAYER:
				onscreen = playerOnScreen(whiteList, options);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case MONSTER:
				onscreen = monsterOnScreen(whiteList, options);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case GM:
				onscreen = gmOnScreen(whiteList, options);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case BATTLELIST:
				onscreen = battleListActive();
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case BLACKSKULL:
				onscreen = skullOnScreen(BLACK_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case REDSKULL:
				onscreen = skullOnScreen(RED_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case GREENSKULL:
				onscreen = skullOnScreen(GREEN_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case YELLOWSKULL:
				onscreen = skullOnScreen(YELLOW_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			case WHITESKULL:
				onscreen = skullOnScreen(WHITE_SKULL);
				if(onscreen && onScreenAt != 0 && time(NULL) - onScreenAt >= intTrigger)
					retval=true;
				else if (onscreen && onScreenAt == 0)
					onScreenAt = time(NULL);
				else if (!onscreen)
					onScreenAt = 0;
				retval=false;
				break;
			}
			}
			break;
		}	
		break;
	case ONLINE:
		switch (condition) {
		case LOGON:
			switch (attribute) {
			case CURRENTPLAYERONLINE:
				retval=reader.getConnectionState() == 8;
				break;
			default:
				retval=vipOnline(attribute - 1);
				break;
			}
			break;
		case LOGOFF:
			switch (attribute) {
			case CURRENTPLAYERONLINE:
				retval=reader.getConnectionState() != 8;
				break;
			default:
				retval=!vipOnline(attribute - 1);
				break;
			}
			break;
		}
		break;
	case CHARACTER_STATUS:
		retval=reader.getSelfEventFlags() & (int)pow(2,attribute);
		break;
	case EVENT:
		struct tibiaMessage *msg = triggerMessage();
		int isSpell = msg && (options & OPTIONS_IGNORE_SPELLS) && isSpellMessage(msg->msg);
		switch (attribute) {
		case ALLMESSAGES:
			retval = !isSpell && msg && (msg->type == 1 || msg->type == 2 || msg->type == 6);
			break;
		case PUBLICMESSAGES:
			retval =  !isSpell && msg && (msg->type == 1 || msg->type == 2);
			break;
		case PRIVATEMESSAGES:
			retval = !isSpell && msg && msg->type == 6;
			break;
		case CHARACTERMOVED:
			retval = positionXInit != self->x || positionYInit != self->y || positionZInit != self->z;
			break;
		case CHARACTERHIT:
			retval = self->hp < healthInit;
			healthInit = self->hp;
			break;
		case CHARACTERNOTMOVED:
			switch (condition) {
			case EQUAL:
				retval=time(NULL)-movedTime == getIntTrigger();
				break;
			case LESS:
				retval=time(NULL)-movedTime < getIntTrigger();
				break;
			case MORE:
				retval=time(NULL)-movedTime > getIntTrigger();
				break;
			}
			break;
		}
		if (msg) delete msg;
		break;
	}
	delete self;
	return retval;
}

//////////////////////////////////////////////////////////////////////
// Triggers
//////////////////////////////////////////////////////////////////////

int Alarm::spaceAvailable() {
	CTibiaItemProxy itemProxy;	
	CMemReaderProxy reader;
	int space = 0;
	
	for (int contNr = 0; contNr < itemProxy.getValueForConst("maxContainers"); contNr++) {		
		CTibiaContainer *cont = reader.readContainer(contNr);
		if (cont->flagOnOff)
			space += cont->size - cont->itemsInside;
		delete cont;
	}	
	return space;
}

bool Alarm::playerOnScreen(char whiteList[100][32], int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	
	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);		
		if (ch->visible) {		
			if (ch->tibiaId != self->tibiaId && (!OnList(whiteList,(char*)ch->name) ^ ((options & OPTIONS_MAKE_BLACKLIST)!=0))) {				
				if (ch->z == self->z || (options & OPTIONS_BATTLE_PARANOIA) || ((options & OPTIONS_BATTLE_ANXIETY) && abs(ch->z - self->z) <= 1)) {					
					if (ch->tibiaId < 0x40000000) {
						delete ch;
						delete self;							
						return true;
					}					
				}
			}			
		}
		delete ch;
	}
	delete self;
	return false;
}

bool Alarm::gmOnScreen(char whiteList[100][32],int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();

	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++){
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);		
		if (ch->visible) {		
			if (ch->tibiaId != self->tibiaId && (!OnList(whiteList, (char*)ch->name) ^ ((options & OPTIONS_MAKE_BLACKLIST)!=0))) {				
				if (ch->z == self->z || (options & OPTIONS_BATTLE_PARANOIA) || (options & OPTIONS_BATTLE_ANXIETY && abs(ch->z - self->z) <= 1)) {					
					if (ch->name[0] == 'G' && ch->name[1] == 'M') { // this is GM						
						delete ch;
						delete self;						
						return true;
					}
					if (ch->name[0] == 'C' && ch->name[1] == 'M') { // this is CM				
						delete ch;
						delete self;						
						return true;
					}					
				}
			}		
		}
		delete ch;
	};
	delete self;
	return false;
}

bool Alarm::monsterOnScreen(char whiteList[100][32], int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	
	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);
		if (ch->visible) {
			if (ch->tibiaId != self->tibiaId && (!OnList(whiteList, (char*)ch->name) ^ ((options & OPTIONS_MAKE_BLACKLIST)!=0))) {
				if (ch->z == self->z || (options & OPTIONS_BATTLE_PARANOIA) || (options & OPTIONS_BATTLE_ANXIETY && abs(ch->z - self->z) <= 1)) {					
					if (ch->tibiaId >= 0x40000000) { // this is monster/npc
						delete ch;
						delete self;							
						return true;
					}					
				}
			}			
		}
		delete ch;
	};
	delete self;
	return false;
}

bool Alarm::battleListActive() {
	CMemReaderProxy reader;

	return (reader.readBattleListMax() >= 0 || reader.readBattleListMin() >= 0);	
}

bool Alarm::vipOnline(int iconIndex) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaVIPEntry *vip;

	for (int vipNr = 0; strcmp((vip = reader.readVIPEntry(vipNr))->name, ""); vipNr++) {
		if (vip->icon == iconIndex && vip->status == 1) {
			delete vip;
			return true;
		}
		delete vip;
	}
	return false;
}

bool Alarm::skullOnScreen(int options) {
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	for (int creatureNr = 0; creatureNr < memConstData.m_memMaxCreatures; creatureNr++) {
		CTibiaCharacter *ch = reader.readVisibleCreature(creatureNr);		
		if (ch->visible) {
			if (ch->tibiaId != self->tibiaId) {				
				if (ch->z == self->z) {					
					if (ch->skulls == options) {
						delete ch;
						delete self;						
						return true;
					}
				}
			}			
		}
		delete ch;
	}
	delete self;
	return false;
}

//////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////

int Alarm::countAllItemsOfType(int objectId) {
	CTibiaItemProxy itemProxy;	
	CMemReaderProxy reader;
	int itemCount = 0;

	for (int contNr = 0; contNr < itemProxy.getValueForConst("maxContainers"); contNr++) {
		CTibiaContainer *cont = reader.readContainer(contNr);		
		if (cont->flagOnOff)
			itemCount += cont->countItemsOfType(objectId);
		delete cont;
	}
	return itemCount;
}

int Alarm::countAllFood() {
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	int foodCount = 0;
	
	for (int contNr = 0; contNr < itemProxy.getValueForConst("maxContainers"); contNr++) {
		CTibiaItem *item = CModuleUtil::lookupItem(contNr, itemProxy.getFoodIdArrayPtr());
		if (item->objectId)
			foodCount += countAllItemsOfType(item->objectId);
		delete item;
	}	
	return foodCount;
}

bool Alarm::OnList(char whiteList[100][32],char name[]) {
	int i=0;
	while (IsCharAlphaNumeric(whiteList[i][0])) {
		if (!strcmpi(whiteList[i],name))
			return true;
		i++;
	}
	return false;
}

bool Alarm::isSpellMessage(char *msg) {
	const char *spellPre[] = {"ex",	"ad", "ut", "al", NULL};
	const char *spellSuf[] = {"ana", "eta", "evo", "ito", "ori", "ura", "ani", "iva", "amo", NULL};
	char newmsg[128] = {'\0'};

	//get 5 characters from msg discarding all spaces
	int count=0;
	for (int i = 0; count < 5 && msg[i]; i++) {
		if (msg[i] != ' ') {
			newmsg[count] = msg[i];
			count++;
		}
	}
	newmsg[count] = '\0';

	if (strlen(newmsg) != 5) return false;

	//if string starts with prefix and is = prefix+suffix, return 1
	for (int pos = 0; spellPre[pos]; pos++) {
		if (strnicmp(newmsg, spellPre[pos], 2) == 0) {
			for (int pos2 = 0; spellSuf[pos2]; pos2++) {
				char tmp[10];
				tmp[0]=0;
				strcat(tmp,spellPre[pos]);
				strcat(tmp,spellSuf[pos2]);
				tmp[5]='\0';
				if (strnicmp(newmsg, tmp, 5) == 0) {
					return true;
				}
			}
		}
	}
	return false;
}

struct tibiaMessage * Alarm::triggerMessage() {
	CMemReaderProxy reader;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	if (backPipe.readFromPipe(&mess,1003)) {
		int infoType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];
		
		memset(nickBuf,0,16384);
		memset(msgBuf,0,16384);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&nickLen,mess.payload+4,sizeof(int));
		memcpy(&msgLen,mess.payload+8,sizeof(int));
		memcpy(nickBuf,mess.payload+12,nickLen);
		memcpy(msgBuf,mess.payload+12+nickLen,msgLen);
		
		CTibiaCharacter *self = reader.readSelfCharacter();

		if (strcmpi(nickBuf, self->name) != 0 && strcmpi(nickBuf, "Tibia Auto") != 0) {
			delete self;
			struct tibiaMessage *newMsg = new tibiaMessage();
			newMsg->type = infoType;
			strcpy(newMsg->msg, msgBuf);
			return newMsg;
		}
		delete self;
	}
	return NULL;
}




int Alarm::getAlarmType() {
	return alarmType;
}

int Alarm::getAttribute() {
	return attribute;
}

int Alarm::getCondition() {
	return condition;
}

CString Alarm::getStrTrigger() {
	return strTrigger;
}

int Alarm::getIntTrigger() {
	return intTrigger;
}

int Alarm::getManaCost() {
	return manaCost;
}

void Alarm::setManaCost(int cost) {
	manaCost = cost;
}

CString Alarm::getDescriptor() {
	return alarmDescriptor;
}

void Alarm::setDescriptor(CString descriptor) {
	alarmDescriptor = descriptor;
}

void Alarm::setSpellDelay(int delay) {
	spellDelay = delay;
}

int Alarm::getSpellDelay() {
	return spellDelay;
}