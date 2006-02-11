// MemReader.cpp: implementation of the CMemReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemReader.h"
#include "MemUtil.h"

#include "TibiaItem.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemReader::CMemReader()
{
	CMemConstData();
}

CMemReader::~CMemReader()
{

}

CTibiaContainer *CMemReader::readContainer(int containerNr)
{	
	int i;

	CTibiaContainer *container = new CTibiaContainer();
	long containerOffset=m_memAddressFirstContainer+containerNr*m_memLengthContainer;		
	container->flagOnOff=CMemUtil::GetMemIntValue(containerOffset+0);	
	container->objectId=CMemUtil::GetMemIntValue(containerOffset+4);
	container->size=CMemUtil::GetMemIntValue(containerOffset+48);
	container->number=containerNr;
	container->itemsInside=CMemUtil::GetMemIntValue(containerOffset+56);

	for (i=0;i<container->itemsInside;i++)
	{
		CTibiaItem *item = new CTibiaItem();
		item->objectId = CMemUtil::GetMemIntValue(containerOffset+60+i*m_memLengthItem+0);
		item->quantity = CMemUtil::GetMemIntValue(containerOffset+60+i*m_memLengthItem+4);		
		item->pos = i;
		container->items.Add(item);
	};

	return container;
}


CTibiaCharacter *CMemReader::readSelfCharacter()
{
	
	CTibiaCharacter *ch = new CTibiaCharacter();

	ch->hp = CMemUtil::GetMemIntValue(m_memAddressHP);
	ch->mana = CMemUtil::GetMemIntValue(m_memAddressMana);	
	ch->maxHp = CMemUtil::GetMemIntValue(m_memAddressHPMax);
	ch->maxMana = CMemUtil::GetMemIntValue(m_memAddressManaMax);
	ch->cap = CMemUtil::GetMemIntValue(m_memAddressCap);
	ch->exp = CMemUtil::GetMemIntValue(m_memAddressExp);
	ch->lvl = CMemUtil::GetMemIntValue(m_memAddressLvl);
	ch->mlvl = CMemUtil::GetMemIntValue(m_memAddressMlvl);
	ch->soulPoints = CMemUtil::GetMemIntValue(m_memAddressSoulPoints);

	ch->skillAxe = CMemUtil::GetMemIntValue(m_memAddressSkillAxe);
	ch->skillClub = CMemUtil::GetMemIntValue(m_memAddressSkillClub);
	ch->skillSword = CMemUtil::GetMemIntValue(m_memAddressSkillSword);
	ch->skillDist = CMemUtil::GetMemIntValue(m_memAddressSkillDist);
	ch->skillFish = CMemUtil::GetMemIntValue(m_memAddressSkillFish);
	ch->skillShield = CMemUtil::GetMemIntValue(m_memAddressSkillShield);
	ch->skillAxePercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillAxePercLeft);
	ch->skillClubPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillClubPercLeft);
	ch->skillSwordPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillSwordPercLeft);
	ch->skillDistPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillDistPercLeft);
	ch->skillFishPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillFishPercLeft);
	ch->skillShieldPercLeft = 100-CMemUtil::GetMemIntValue(m_memAddressSkillShieldPercLeft);			
	
	ch->visible=1;

	int loggedCharNr=getLoggedCharNr();
	if (loggedCharNr>=0)
	{
		CTibiaCharacter *monCh = readVisibleCreature(loggedCharNr);
		ch->tibiaId=monCh->tibiaId;
		ch->x=monCh->x;
		ch->y=monCh->y;
		ch->z=monCh->z;


		delete monCh;
	}

	return ch;
}

CTibiaItem * CMemReader::readItem(int locationAddress)
{
	CTibiaItem *item = new CTibiaItem();

	item->objectId = CMemUtil::GetMemIntValue(locationAddress + 0);
	item->quantity = CMemUtil::GetMemIntValue(locationAddress + 4);

	if (item->objectId==0)
		item->quantity=0;

	return item;
}

CTibiaCharacter * CMemReader::readVisibleCreature(int nr)
{
	CTibiaCharacter *ch = new CTibiaCharacter();

	int offset = m_memAddressFirstCreature+nr*m_memLengthCreature;

	ch->hp=-1;
	ch->mana=-1;
	ch->tibiaId=CMemUtil::GetMemIntValue(offset+0);		
	ch->x=CMemUtil::GetMemIntValue(offset+36);
	ch->y=CMemUtil::GetMemIntValue(offset+40);
	ch->z=CMemUtil::GetMemIntValue(offset+44);
	ch->outfitId=CMemUtil::GetMemIntValue(offset+72);
	ch->monsterType=CMemUtil::GetMemIntValue(offset+96);
	ch->hpPercLeft=CMemUtil::GetMemIntValue(offset+132);
	ch->visible=CMemUtil::GetMemIntValue(offset+140);	
	ch->lastAttackTm=CMemUtil::GetMemIntValue(offset+128);
	ch->nr=nr;
	
	
	CMemUtil::GetMemRange(offset+4,offset+4+31,ch->name);
	
	
	return ch;
}

int CMemReader::readBattleListMin()
{
	return CMemUtil::GetMemIntValue(m_memAddressBattleMin);
}

int CMemReader::readBattleListMax()
{
	return CMemUtil::GetMemIntValue(m_memAddressBattleMax);
}

char * CMemReader::GetLoggedChar(int processId)
{
	char *ret=(char *)malloc(MAX_STRING_LEN);
	long selfId;
	int i;
	
	memset(ret,0x00,MAX_STRING_LEN);
	sprintf(ret,"unknown");
		
	CMemUtil::GetMemIntValue(processId,m_memAddressSelfId,&selfId);

	for (i=0;i<m_memMaxCreatures;i++)
	{
		long creatureId,visible;
		long offset = m_memAddressFirstCreature+i*m_memLengthCreature;
		CMemUtil::GetMemIntValue(processId,offset+0,&creatureId);		
		CMemUtil::GetMemIntValue(processId,offset+140,&visible);
		if (selfId==creatureId&&visible)
		{
			CMemUtil::GetMemRange(processId,offset+4,offset+4+31,ret);
		};
	};
		
	return ret;
}

int CMemReader::getAttackedCreature()
{
	return CMemUtil::GetMemIntValue(m_memAddressAttackedCreature);
}

CTibiaCharacter *CMemReader::getCharacterByTibiaId(int tibiaId)
{			
	int i;
	for (i=0;i<m_memMaxCreatures;i++)
	{
		CTibiaCharacter *ch = readVisibleCreature(i);

		if (ch->tibiaId==tibiaId)
			return ch;

		delete ch;
	}
	return NULL;
}

int CMemReader::getTradeCountSelf()
{
	return CMemUtil::GetMemIntValue(m_memAddressTradeCountSelf);
}

int CMemReader::getTradeCountPartner()
{
	return CMemUtil::GetMemIntValue(m_memAddressTradeCountPartner);
}

CTibiaItem * CMemReader::getTradeItemSelf(int nr)
{
	CTibiaItem *item = new CTibiaItem();
	item->objectId=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemSelf+nr*m_memLengthItem);
	item->quantity=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemSelf+nr*m_memLengthItem+4);
	item->pos=nr;
	return item;
}

CTibiaItem * CMemReader::getTradeItemPartner(int nr)
{
	CTibiaItem *item = new CTibiaItem();
	item->objectId=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemPartner+nr*m_memLengthItem);
	item->quantity=CMemUtil::GetMemIntValue(m_memAddressTradeFirstItemPartner+nr*m_memLengthItem+4);
	item->pos=nr;
	return item;
}

void CMemReader::writeVisibleCreatureName(int chNr, char *name)
{		
	int offset = m_memAddressFirstCreature+chNr*m_memLengthCreature;	
	if (strlen(name)>31)
	{
		AfxMessageBox("writeVisibleCreatureName: too long name");
		return;
	}
	CMemUtil::SetMemRange(offset+4,offset+4+(strlen(name)+1),name);		
}

int CMemReader::getSelfEventFlags()
{
	return CMemUtil::GetMemIntValue(m_memAddressSelfFlags);
}

int CMemReader::getLoggedCharNr()
{	
	
	int i;		
	
	long selfId=CMemUtil::GetMemIntValue(m_memAddressSelfId);
	

	for (i=0;i<m_memMaxCreatures;i++)
	{
		long creatureId,visible;
		long offset = m_memAddressFirstCreature+i*m_memLengthCreature;
		creatureId=CMemUtil::GetMemIntValue(offset+0);		
		visible=CMemUtil::GetMemIntValue(offset+140);
		if (selfId==creatureId&&visible)
		{
			return i;
		};
	};
		
	return -1;
}

void CMemReader::writeGotoCoords(int x, int y, int z)
{
	int chNr=getLoggedCharNr();
	CMemUtil::SetMemIntValue(m_memAddressGoX,x);
	CMemUtil::SetMemIntValue(m_memAddressGoY,y);
	CMemUtil::SetMemIntValue(m_memAddressGoZ,z);
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+76+chNr*m_memLengthCreature,1);
}	

void CMemReader::cancelAttackCoords()
{
	int chNr=getLoggedCharNr();
	CMemUtil::SetMemIntValue(m_memAddressFirstCreature+76+chNr*m_memLengthCreature,0);
	CMemUtil::SetMemIntValue(m_memAddressGoX,0);
	CMemUtil::SetMemIntValue(m_memAddressGoY,0);
	CMemUtil::SetMemIntValue(m_memAddressGoZ,0);	
}

int CMemReader::readCreatureLightPower(int creatureNr)
{	
	CMemReader reader;
	return CMemUtil::GetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+116);
}

int CMemReader::readCreatureLightColor(int creatureNr)
{	
	CMemReader reader;
	return CMemUtil::GetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+116+4);
}

void CMemReader::writeCreatureLightPower(int creatureNr,int value)
{	
	CMemReader reader;
	CMemUtil::SetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+116,value);		
}

void CMemReader::writeCreatureLightColor(int creatureNr,int value)
{
	CMemReader reader;
	CMemUtil::SetMemIntValue(reader.m_memAddressFirstCreature+creatureNr*reader.m_memLengthCreature+116+4,value);		
}	



int CMemReader::readSelfLightPower()
{		
	CMemReader reader;			
	int loggedCharNr=reader.getLoggedCharNr();
	return readCreatureLightPower(loggedCharNr);	
}

int CMemReader::readSelfLightColor()
{
	CMemReader reader;			
	int loggedCharNr=reader.getLoggedCharNr();
	return readCreatureLightColor(loggedCharNr);	
}

void CMemReader::writeSelfLightPower(int value)
{	
	CMemReader reader;			
	int loggedCharNr=reader.getLoggedCharNr();
	writeCreatureLightPower(loggedCharNr,value);
}

void CMemReader::writeSelfLightColor(int value)
{
	CMemReader reader;			
	int loggedCharNr=reader.getLoggedCharNr();
	writeCreatureLightColor(loggedCharNr,value);
}	



int CMemReader::mapGetSelfCellNr()
{				
	static int prevSelfTileNr=0;
	static int prevSelfTilePos=0;
	int wouldReturn=0;
	CTibiaCharacter *self = readSelfCharacter();

	// check self tile cache
	int c=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+prevSelfTileNr*m_memLengthMapTile);
	int tileId=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+prevSelfTileNr*m_memLengthMapTile+prevSelfTilePos*12+4);
	if (prevSelfTilePos<c&&tileId==99)
	{				
		int tileCharId = CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+prevSelfTileNr*168+prevSelfTilePos*12+4+4);
		if (tileCharId==self->tibiaId)
		{
			delete self;
			return prevSelfTileNr;
			
		} 
	}

	int tileNr;
	for (tileNr=0;tileNr<m_memMaxMapTiles;tileNr++)
	{
		int pos;
		int count=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile);
		for (pos=0;pos<count;pos++)
		{
			int tileId=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*m_memLengthMapTile+pos*12+4);
			if (tileId==99)
			{				
				int tileCharId = CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+tileNr*168+pos*12+4+4);
				if (tileCharId==self->tibiaId)
				{
					delete self;
					prevSelfTileNr=tileNr;
					prevSelfTilePos=pos;					
					return tileNr;
				}				
			}
		}				
	}
	delete self;
	return 0;
}

struct point CMemReader::mapConvertCellToPoint(int cellNr)
{
	int z=cellNr/(14*18);
	int y=(cellNr-z*14*18)/18;
	int x=(cellNr-z*14*18-y*18);

	return point(x,y,z);
}

struct point CMemReader::mapConvertPointToRelPoint(point p)
{	
	return point((p.x<=9)?p.x:(p.x-18),(p.y<=7)?p.y:(p.y-14),p.z);
}

struct point CMemReader::mapConvertRelPointToPoint(point p)
{
	return point((p.x>=0)?p.x:(p.x+18),(p.y>=0)?p.y:(p.y+14),p.z);
}

int CMemReader::mapConvertPointToCell(point p)
{
	return p.z*14*18+p.y*18+p.x;
}

int CMemReader::mapGetPointItemsCount(point p)
{	
	int selfCell=mapGetSelfCellNr();
	struct point selfPoint = mapConvertCellToPoint(selfCell);
	struct point selfRelPoint = mapConvertPointToRelPoint(selfPoint);

	if (p.x<-8||p.x>9||p.y<-6||p.y>7||p.z!=0)
	{
		p.x=p.y=p.z=0;
	}
	selfRelPoint.x+=p.x;
	selfRelPoint.y+=p.y;
	selfRelPoint.z+=p.z;
	if (selfRelPoint.x<-8) selfRelPoint.x+=18;
	if (selfRelPoint.x>9) selfRelPoint.x-=18;
	if (selfRelPoint.y<-6) selfRelPoint.y+=14;
	if (selfRelPoint.y>7) selfRelPoint.y-=14;

	struct point itemPoint = mapConvertRelPointToPoint(selfRelPoint);
	int itemCell = mapConvertPointToCell(itemPoint);

	int count=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile);	
	return count;
}

int CMemReader::mapGetPointItemId(point p, int stackNr)
{
	return mapGetPointItemExtraInfo(p,stackNr,0);	
}

void CMemReader::mapSetPointItemId(point p, int stackNr, int tileId)
{
	int selfCell=mapGetSelfCellNr();
	struct point selfPoint = mapConvertCellToPoint(selfCell);
	struct point selfRelPoint = mapConvertPointToRelPoint(selfPoint);

	if (p.x<-8||p.x>9||p.y<-6||p.y>7||p.z!=0)
	{
		p.x=p.y=p.z=0;
	}
	selfRelPoint.x+=p.x;
	selfRelPoint.y+=p.y;
	selfRelPoint.z+=p.z;
	if (selfRelPoint.x<-8) selfRelPoint.x+=18;
	if (selfRelPoint.x>9) selfRelPoint.x-=18;
	if (selfRelPoint.y<-6) selfRelPoint.y+=14;
	if (selfRelPoint.y>7) selfRelPoint.y-=14;

	struct point itemPoint = mapConvertRelPointToPoint(selfRelPoint);
	int itemCell = mapConvertPointToCell(itemPoint);

	CMemUtil::SetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile+4+stackNr*12,tileId);	
}

void CMemReader::mapSetPointItemsCount(point p, int count)
{
	int selfCell=mapGetSelfCellNr();
	struct point selfPoint = mapConvertCellToPoint(selfCell);
	struct point selfRelPoint = mapConvertPointToRelPoint(selfPoint);

	if (p.x<-8||p.x>9||p.y<-6||p.y>7||p.z!=0)
	{
		p.x=p.y=p.z=0;
	}
	selfRelPoint.x+=p.x;
	selfRelPoint.y+=p.y;
	selfRelPoint.z+=p.z;
	if (selfRelPoint.x<-8) selfRelPoint.x+=18;
	if (selfRelPoint.x>9) selfRelPoint.x-=18;
	if (selfRelPoint.y<-6) selfRelPoint.y+=14;
	if (selfRelPoint.y>7) selfRelPoint.y-=14;

	struct point itemPoint = mapConvertRelPointToPoint(selfRelPoint);
	int itemCell = mapConvertPointToCell(itemPoint);

	CMemUtil::SetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile,count);	
}

int CMemReader::dereference(int addr)
{
	static int lastAddrReq=0;
	static int lastAddrResp=0;
	if (lastAddrReq==addr)
		return lastAddrResp;	
	lastAddrResp=CMemUtil::GetMemIntValue(addr);	
	lastAddrReq=addr;		
	return lastAddrResp;
}

int CMemReader::mapGetPointItemExtraInfo(point p, int stackNr, int extraPos)
{
	int selfCell=mapGetSelfCellNr();
	struct point selfPoint = mapConvertCellToPoint(selfCell);
	struct point selfRelPoint = mapConvertPointToRelPoint(selfPoint);

	if (p.x<-8||p.x>9||p.y<-6||p.y>7||p.z!=0)
	{
		p.x=p.y=p.z=0;
	}
	selfRelPoint.x+=p.x;
	selfRelPoint.y+=p.y;
	selfRelPoint.z+=p.z;
	if (selfRelPoint.x<-8) selfRelPoint.x+=18;
	if (selfRelPoint.x>9) selfRelPoint.x-=18;
	if (selfRelPoint.y<-6) selfRelPoint.y+=14;
	if (selfRelPoint.y>7) selfRelPoint.y-=14;

	struct point itemPoint = mapConvertRelPointToPoint(selfRelPoint);
	int itemCell = mapConvertPointToCell(itemPoint);

	int data=CMemUtil::GetMemIntValue(dereference(m_memAddressMapStart)+itemCell*m_memLengthMapTile+4+stackNr*12+extraPos*4);
	return data;
}

long CMemReader::getCurrentTm()
{
	return CMemUtil::GetMemIntValue(m_memAddressCurrentTm);
}

void CMemReader::writeEnableRevealCName()
{
	unsigned char *buf=(unsigned char *)malloc(3);
	buf[0]=0x90;
	buf[1]=0x90;
	// replace jumps	

	// disable additionall 'z' check
	buf[0]=0x90;
	buf[1]=0x90;
	CMemUtil::SetMemRange(m_memAddressRevealCName1,m_memAddressRevealCName1+2,(char *)buf);	
	// disable main 'z' check
	buf[0]=0x47;
	CMemUtil::SetMemRange(m_memAddressRevealCName2,m_memAddressRevealCName2+1,(char *)buf);
	// extend reveal horizon
	buf[0]=8;
	CMemUtil::SetMemRange(m_memAddressRevealCName3,m_memAddressRevealCName3+1,(char *)buf);
	buf[0]=6;
	CMemUtil::SetMemRange(m_memAddressRevealCName4,m_memAddressRevealCName4+1,(char *)buf);
	

	/*
	// replace debug asserts when monster visible but out of range
	// 0xC3 is 'RET'
	buf[0]=0xC3;
	CMemUtil::SetMemRange(0x440260,0x440260+1,(char *)buf);
	CMemUtil::SetMemRange(0x440287,0x440287+1,(char *)buf);
	CMemUtil::SetMemRange(0x4402AE,0x4402AE+1,(char *)buf);
	*/
	free(buf);

	// INVISIBLE - 0x419450!!!
}

void CMemReader::writeDisableRevealCName()
{
	unsigned char *buf=(unsigned char *)malloc(2);	
	buf[0]=0x75;
	buf[1]=0x49;	
	CMemUtil::SetMemRange(m_memAddressRevealCName1,m_memAddressRevealCName1+2,(char *)buf);
	buf[0]=0x46;
	CMemUtil::SetMemRange(m_memAddressRevealCName2,m_memAddressRevealCName2+1,(char *)buf);
	// extend reveal horizon
	buf[0]=7;
	CMemUtil::SetMemRange(m_memAddressRevealCName3,m_memAddressRevealCName3+1,(char *)buf);
	buf[0]=5;
	CMemUtil::SetMemRange(m_memAddressRevealCName4,m_memAddressRevealCName4+1,(char *)buf);
	free(buf);	
}
