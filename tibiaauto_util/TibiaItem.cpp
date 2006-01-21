// TibiaItem.cpp: implementation of the CTibiaItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TibiaItem.h"
#include "Util.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>


XERCES_CPP_NAMESPACE_USE

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "TibiaItem_codes.cpp"


CUIntArray *CTibiaItem::foodList=NULL;
int CTibiaItem::itemListsFresh=0;
int CTibiaItem::xmlInitialised=0;

char CTibiaItem::itemsItems[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsItemsId[MAX_ITEMS];
int CTibiaItem::itemsItemsCount=0;
char CTibiaItem::itemsFood[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsFoodId[MAX_ITEMS];
int CTibiaItem::itemsFoodCount=0;
char CTibiaItem::itemsCorpses[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsCorpsesId[MAX_ITEMS];
int CTibiaItem::itemsCorpsesCount=0;
char CTibiaItem::itemsLooted[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsLootedId[MAX_ITEMS];
int CTibiaItem::itemsLootedCount=0;



CTibiaItem::CTibiaItem()
{
	objectId=0;
	quantity=0;
	pos=0;
}


char * CTibiaItem::getName(int objectId)
{	
	int i;
	
	refreshItemLists();

	for (i=0;i<itemsItemsCount;i++)
	{
		if (itemsItemsId[i]==objectId)
			return itemsItems[i];
	}		
	
	
	return "unknown";
}


int CTibiaItem::getCorpseIdByCreatureName(char *name)
{		
	int i;
	refreshItemLists();

	for (i=0;i<itemsCorpsesCount;i++)
	{
		if (!strcmp(itemsCorpses[i],name))
			return itemsCorpsesId[i];
	}

	// 'human' is the default one
	return 0;
}

CUIntArray * CTibiaItem::getItemsFood()
{	
	refreshItemLists();
	if (!foodList)
	{
		foodList = new CUIntArray();
		int i;
		for (i=0;i<itemsFoodCount;i++)
		{
			foodList->Add(itemsFoodId[i]);			
		}
	}
	return foodList;
}

void CTibiaItem::refreshItemLists()
{	
	if (itemListsFresh) return;
	itemListsFresh=1;
	if (!xmlInitialised)
	{
		XMLPlatformUtils::Initialize();
		xmlInitialised=1;
	}

	XercesDOMParser *parser = new XercesDOMParser();
	try
	{	
		int listNr,itemNr,rootNr;		

		itemsItemsCount=0;
		itemsLootedCount=0;
		itemsCorpsesCount=0;
		itemsFoodCount=0;
				
		parser->parse("mods\\tibiaauto-items.xml");							
		DOMNode  *doc = parser->getDocument();		
		for (rootNr=0;rootNr<doc->getChildNodes()->getLength();rootNr++)
		{			
			DOMNode *root = doc->getChildNodes()->item(rootNr);
				
			if (wcscmp(root->getNodeName(),_L("item-definitions")))
				continue;			
			for (listNr=0;listNr<root->getChildNodes()->getLength();listNr++)
			{
				DOMNode *listNode = root->getChildNodes()->item(listNr);			
				if (!wcscmp(listNode->getNodeName(),_L("items"))) {
					
					for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
					{
						int attrNr;
						DOMNode *item = listNode->getChildNodes()->item(itemNr);
						if (wcscmp(item->getNodeName(),_L("item")))
							continue;
						
						int objectId=0;
						char *objectName=NULL;						
						
						for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
						{
							DOMNode *attrNode = item->getAttributes()->item(attrNr);
							if (!wcscmp(attrNode->getNodeName(),_L("name")))							
								objectName=CUtil::wc2c(attrNode->getNodeValue());																							
							if (!wcscmp(attrNode->getNodeName(),_L("id")))
							{
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"0x%x",&objectId);
								free(idTmp);
							}
						}			 			
						if (!objectId||!objectName||!strlen(objectName))
							continue;												
						memcpy(itemsItems[itemsItemsCount],objectName,strlen(objectName)+1);
						itemsItemsId[itemsItemsCount]=objectId;
						itemsItemsCount++;
					}
				}
				if (!wcscmp(listNode->getNodeName(),_L("foods"))) {
					
					for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
					{
						int attrNr;
						DOMNode *item = listNode->getChildNodes()->item(itemNr);
						if (wcscmp(item->getNodeName(),_L("item")))
							continue;
						
						int objectId=0;
						char *objectName=NULL;						
						
						for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
						{
							DOMNode *attrNode = item->getAttributes()->item(attrNr);
							if (!wcscmp(attrNode->getNodeName(),_L("name")))							
								objectName=CUtil::wc2c(attrNode->getNodeValue());																							
							if (!wcscmp(attrNode->getNodeName(),_L("id")))
							{
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"0x%x",&objectId);
								free(idTmp);
							}
						}			 			
						if (!objectId||!objectName||!strlen(objectName))
							continue;
						
						memcpy(itemsFood[itemsFoodCount],objectName,strlen(objectName)+1);
						itemsFoodId[itemsFoodCount]=objectId;
						itemsFoodCount++;
					}
				}

				if (!wcscmp(listNode->getNodeName(),_L("corpses"))) {
					
					for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
					{
						int attrNr;
						DOMNode *item = listNode->getChildNodes()->item(itemNr);
						if (wcscmp(item->getNodeName(),_L("item")))
							continue;
						
						int objectId=0;
						char *objectName=NULL;						
						
						for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
						{
							DOMNode *attrNode = item->getAttributes()->item(attrNr);
							if (!wcscmp(attrNode->getNodeName(),_L("name")))							
								objectName=CUtil::wc2c(attrNode->getNodeValue());																							
							if (!wcscmp(attrNode->getNodeName(),_L("id")))
							{
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"0x%x",&objectId);
								free(idTmp);
							}
						}			 			
						if (!objectId||!objectName||!strlen(objectName))
							continue;
						
						memcpy(itemsCorpses[itemsCorpsesCount],objectName,strlen(objectName)+1);
						itemsCorpsesId[itemsCorpsesCount]=objectId;
						itemsCorpsesCount++;
					}
				}

				if (!wcscmp(listNode->getNodeName(),_L("looted"))) {
					
					for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
					{
						int attrNr;
						DOMNode *item = listNode->getChildNodes()->item(itemNr);
						if (wcscmp(item->getNodeName(),_L("item")))
							continue;
						
						int objectId=0;
						char *objectName=NULL;						
						
						for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
						{
							DOMNode *attrNode = item->getAttributes()->item(attrNr);
							if (!wcscmp(attrNode->getNodeName(),_L("name")))							
								objectName=CUtil::wc2c(attrNode->getNodeValue());																							
							if (!wcscmp(attrNode->getNodeName(),_L("id")))
							{
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"0x%x",&objectId);
								free(idTmp);
							}
						}			 			
						if (!objectId||!objectName||!strlen(objectName))
							continue;
						
						memcpy(itemsLooted[itemsLootedCount],objectName,strlen(objectName)+1);
						itemsLootedId[itemsLootedCount]=objectId;
						itemsLootedCount++;
					}
				}
				
			}
		}
		
	} catch (...)
	{
		AfxMessageBox("Unable to load item definitions!");
	}

	delete parser;		


	// remove cached food list
	delete foodList;
	foodList=NULL;
}
 
