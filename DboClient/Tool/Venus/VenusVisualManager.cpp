//***********************************************************************************
//	File		:	CVenusVisualManager.h
//	Desc		:	
//	Begin		:	2005. 7.28
//	Copyright	:	?? 2005 by agebreak CO., Ltd
//	Author		:	agebreak
//	Update		:	
//***********************************************************************************

#include "StdAfx.h"
#include ".\venusvisualmanager.h"

#include "VenusPropertyContainer.h"

#include "NtlPLEntityFactory.h"

#include ".\venusconfig.h"

#include "Manipulator.h"
#include "VenusConfig.h"
#include "VenusCamera.h"

#include "VenusAPI.h"
#include "NtlPlApi.h"

#include "InstanceEffectDialog.h"

#include "EditDialog.h"


#include "NtlInstanceParticleSystem.h"
#include "NtlInstanceHurricaneSystem.h"
#include "NtlInstanceLightningSystem.h"
#include "NtlInstanceDecalSystem.h"
#include "NtlInstancePostEffectSystem.h"
#include "NtlInstanceTraceSystem.h"
#include "NtlInstanceLineSystem.h"

CVenusVisualManager::CVenusVisualManager(void)
{
	m_pResourceEffect = NULL;
	m_pResourceSystem = NULL;
	m_nSystemDataFlag = 0;
	m_lpDevice	= NULL;	

	m_fDeltaTime	= 0.f;

	for (RwInt32 i = 0; i < EMITTER_VERTEX_COUNT; ++ i)
	{
		RwIm3DVertexSetPos(&m_EmitterVertex[i], 0.f, 0.f, 0.f);
		RwIm3DVertexSetRGBA(&m_EmitterVertex[i], 255, 255, 0, 255);
	}

	RwImVertexIndex index[]  = {0, 1, 1, 5, 5, 4, 4, 0,
								2, 3, 3, 7, 7, 6, 6, 2,
								0, 4, 4, 6, 6, 2, 2, 0,
								1, 3, 3, 7, 7, 5, 5, 1,
								0, 1, 1, 3, 3, 2, 2, 0,
								4, 5, 5, 7, 7, 6, 6, 4};

	memcpy(m_EmitterIndex, index, sizeof(RwImVertexIndex) * EMITTER_INDEX_COUNT);

    m_pBSCurrEffect = NULL;
    m_nBSIndex = 0;
    m_bGenerateBS = FALSE;
    m_fEffectLifeTime = 0.0f;
    m_nEventEffectUsedMemory = 0;
}

CVenusVisualManager::~CVenusVisualManager(void)
{
}

//------------------------------------------------------------------
//	FuncName	: GetInstance
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
CVenusVisualManager& CVenusVisualManager::GetInstance()
{
	static CVenusVisualManager manager;
	return manager;
}

//------------------------------------------------------------------
//	FuncName	: Create
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
RwBool CVenusVisualManager::Create(void)
{
	m_lpDevice = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
	MarkAutoDeleteFlag(PLENTITY_EFFECT);		// Effect?? Auto Delete ?ɶ? ?????Լ??? ȣ???ϱ? ???ؼ? ?????Ѵ?.
	return CNtlPLVisualManager::Create();
}

//------------------------------------------------------------------
//	FuncName	: Destroy
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::Destroy(void)
{
	m_svSelectInstanceEffect.clear();
	m_pResourceEffect = NULL;
	m_pResourceSystem = NULL;
	m_nSystemDataFlag = 0;

	CNtlPLVisualManager::Destroy();
}

void CVenusVisualManager::Clear() 
{
	m_svSelectInstanceEffect.clear();
	m_pResourceEffect = NULL;
	m_pResourceSystem = NULL;
	m_nSystemDataFlag = 0;

	CNtlPLRenderGroup *pGroup;
	MapRenderGroup::iterator it;

	std::list<CNtlPLEntity*>::iterator the;
	std::list<CNtlPLEntity*> listEntities;

	// update group
	for(it = m_mapUpdateGroup.begin(); it != m_mapUpdateGroup.end(); ++it)
	{
		pGroup = (*it).second;
		if(pGroup->GetLayer() == PLENTITY_LAYER_EFFECT)
		{
			pGroup->Gets(listEntities);
			for(the = listEntities.begin(); the != listEntities.end(); the++)
			{
				DeleteEntity(*the);
			}
			//pGroup->Destroy();
			//NTL_DELETE(pGroup);
			listEntities.clear();
		}
	}	

	// rendering group
	for(it = m_mapRenderGroup.begin(); it != m_mapRenderGroup.end(); ++it)
	{
		pGroup = (*it).second;
		if(pGroup->GetLayer() == PLENTITY_LAYER_EFFECT)
		{
			pGroup->Gets(listEntities);
			for(the = listEntities.begin(); the != listEntities.end(); the++)
			{
				DeleteEntity(*the);
			}
			//pGroup->Destroy();
			//NTL_DELETE(pGroup);
			listEntities.clear();
		}
	}	

	// instance group
	for(it = m_mapInstanceGroup.begin(); it != m_mapInstanceGroup.end(); ++it)
	{
		pGroup = (*it).second;
		if(pGroup->GetLayer() == PLENTITY_LAYER_EFFECT)
		{
			pGroup->Gets(listEntities);
			for(the = listEntities.begin(); the != listEntities.end(); the++)
			{
				DeleteEntity(*the);
			}
			//pGroup->Destroy();
			//NTL_DELETE(pGroup);
			listEntities.clear();
		}
	}	
}

//------------------------------------------------------------------
//	FuncName	: Update
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::Update(RwReal fElapsed)
{
    // Generate Effect Bounding Sphere
    if(m_bGenerateBS)
    {
        m_fEffectLifeTime +=fElapsed;

        if(!m_pBSCurrEffect || m_fEffectLifeTime >= 2.0f)
        {
            if(m_pBSCurrEffect)
                DeleteEntity(m_pBSCurrEffect);

            ++m_nBSIndex;
            m_fEffectLifeTime = 0.0f;
            m_pBSCurrEffect = CreateEffect(m_nBSIndex);            
            if(!m_pBSCurrEffect)
                m_bGenerateBS = FALSE;
        }        
    }

	CNtlPLVisualManager::Update(fElapsed);
	return;
}

void CVenusVisualManager::Render(void)
{
	m_lpDevice->SetRenderState(D3DRS_FILLMODE, CVenusConfig::GetInstance().m_D3DFillMode);

	CNtlPLVisualManager::Render();

    // ?ٿ??? ???Ǿ? ?ڵ? ?????ÿ? ??????
    if(m_bGenerateBS && m_pBSCurrEffect)
    {
        m_pBSCurrEffect->RenderBoundingSphere();
    }

	if (CVenusConfig::GetInstance().m_nCurrentLayer != EDIT_LAYER_PATH) 
	{
		if (!m_svSelectInstanceEffect.empty())
		{
			RwV3d vTotalPosition = { 0, 0, 0 };
			
            for each(CNtlInstanceEffect* pInstanceEffect in m_svSelectInstanceEffect)
			{
				RwV3d vPosition = *RwMatrixGetPos(&pInstanceEffect->m_matWorld);				
                vTotalPosition += vPosition;

                // ?ٿ??? ???Ǿ? ??????
                if(CVenusConfig::GetInstance().m_bRenderBoundingSphere)
                {
                    pInstanceEffect->RenderBoundingSphere();
                }                
			}
			vTotalPosition.x = vTotalPosition.x / (RwReal)m_svSelectInstanceEffect.size();
			vTotalPosition.y = vTotalPosition.y / (RwReal)m_svSelectInstanceEffect.size();
			vTotalPosition.z = vTotalPosition.z / (RwReal)m_svSelectInstanceEffect.size();

			RwMatrix vViewMatrix = *CVenusCamera::GetInstance().GetViewMatrix();
			CManipulator::GetInstance().SetPosition(vTotalPosition.x, vTotalPosition.y, vTotalPosition.z);
			CManipulator::GetInstance().Render(&vViewMatrix);
		}
	}
}

//------------------------------------------------------------------
//	FuncName	: GetRenderingEffectCount
//	Desc		: ???? ?????????? Effect?? ?????? ??ȯ?Ѵ?.
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
RwInt32 CVenusVisualManager::GetRenderingEffectCount()
{

    CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
    if(!pGroup)
        return 0;
    
    return (RwInt32)pGroup->GetEntities()->size();

	return 0;
}

//------------------------------------------------------------------
//	FuncName	: GetRenderingPolygonCount
//	Desc		: ???? ?????????? ???????? ????(??ƼŬ ????)?? ??ȯ?Ѵ?.
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
RwInt32 CVenusVisualManager::GetRenderingPolygonCount()
{
	// active particle count 
	RwInt32 nRenderPolygonCount = 0;
	CNtlPLRenderGroup::MapEntity::iterator it;

    CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
    if(!pGroup)
        return 0;

	CNtlPLRenderGroup::MapEntity *pEntities = pGroup->GetEntities();
	for(it = pEntities->begin(); it != pEntities->end(); ++it)
	{
		CNtlInstanceEffect* pInstanceEffect = (CNtlInstanceEffect*) (*it).second;

		CNtlInstanceEffect::svdef_InstanceComponentSystem::iterator jt;
		for (jt = pInstanceEffect->m_svSystem.begin(); jt != pInstanceEffect->m_svSystem.end(); ++ jt)
		{
			if (!(*jt)->m_bReady) continue;

			switch((*jt)->GetSystemType())
			{
			case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_PARTICLE:
				{
					CNtlInstanceParticleSystem* pSystem = (CNtlInstanceParticleSystem*)(*jt);
					if (pSystem->m_pEmitter != NULL)
					{
						nRenderPolygonCount += pSystem->m_pEmitter->prtActive;
					}
				}
				break;
			case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_MESH:
				{
//					CNtlInstanceMeshSystem* pSystem = (*jt);
				}
				break;
			case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_BEAM:
				{
//					CNtlInstanceBeamSystem* pSystem = (*jt);
					nRenderPolygonCount += 6;
				}
				break;
			case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_HURRICANE:
				{
					CNtlInstanceHurricaneSystem* pSystem = (CNtlInstanceHurricaneSystem*)(*jt);
					nRenderPolygonCount += (pSystem->m_nCurrentPoint - pSystem->m_nStartPoint) * 2;
				}
				break;
			case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_LIGHTNING:
				{
					CNtlInstanceLightningSystem* pSystem = (CNtlInstanceLightningSystem*)(*jt);
					nRenderPolygonCount += pSystem->m_nLevelCount * 2;
				}
				break;
			case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_DECAL:
				{
					CNtlInstanceDecalSystem* pSystem = (CNtlInstanceDecalSystem*)(*jt);
					nRenderPolygonCount += pSystem->GetPolyCount();
				}
				break;
			case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_POST_EFFECT:
				{
					CNtlInstancePostEffectSystem* pSystem = (CNtlInstancePostEffectSystem*)(*jt);
				}
				break;            
            case CNtlResourceComponentSystem::RESOURCE_SYSTEMTYPE_LINE:
                {
                    CNtlInstanceLineSystem* pSystem = (CNtlInstanceLineSystem*)(*jt);
                    nRenderPolygonCount += pSystem->GetPolyCount();
                }
                break;
			default:
				NTL_ASSERTE(!"default");
				break;
			}
		}
	}

	return nRenderPolygonCount;
}

//------------------------------------------------------------------
//	FuncName	: ResetInstanceEffect
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
RwBool CVenusVisualManager::ResetInstanceEffectFromSplinePath(const RwChar* strName)
{
	/*
	CNtlPLRenderGroup::MapEntity *pEntities = m_Effect.GetEntities();
	CNtlPLRenderGroup::MapEntity::iterator it;
	for(it = pEntities->begin(); it != pEntities->end(); ++it)
	{
		CNtlInstanceEffect* pInstanceEffect = (CNtlInstanceEffect*)(*it).second;

		CNtlResourceEffect::svdef_ResourceComponentSystem::iterator jt;
		for (jt = pInstanceEffect->m_pResourceEffect->m_svResourceSystem.begin(); jt != pInstanceEffect->m_pResourceEffect->m_svResourceSystem.end(); ++ jt)
		{
			CNtlResourceComponentSystem* pResourceSystem = (*jt);
			if (pResourceSystem->m_nPathEmitterDataFlag == rpPRTADVEMITTERDATAFLAGSPLINEPATH)
			{
				if (_stricmp(pResourceSystem->m_EmitterPrtSplinePath.strName, strName) == 0)
				{
					ResetInstanceEffect(pInstanceEffect->m_pResourceEffect->GetName());
					break;
				}
			}
		}
	}
	*/

	return TRUE;
}

/**
 * EffectSystem?? Property Data?? ?????? ?Ǿ??? ???? ȣ???? ?ȴ?.
 * \param strName: Effect Entity?? ?̸?
 * \return ???? ????
 */
RwBool CVenusVisualManager::ResetInstanceEffect(const RwChar* strName)
{
	CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
    if(!pGroup)
        return TRUE;

	CNtlPLRenderGroup::MapEntity::iterator it;
	CNtlPLRenderGroup::MapEntity *pmapEntity = pGroup->GetEntities();
	for(it = pmapEntity->begin(); it != pmapEntity->end(); )
	{
		CNtlInstanceEffect* pInstanceEffect = (CNtlInstanceEffect*)(*it).second;

		if (_stricmp(pInstanceEffect->m_pResourceEffect->GetName(), strName) == 0)
		{
			RwMatrix matWorld = pInstanceEffect->m_matWorld;
			CNtlResourceEffect* pResourceEffect = pInstanceEffect->m_pResourceEffect;

			pInstanceEffect->Destroy();
			pInstanceEffect->SetProperty(pResourceEffect);
			pInstanceEffect->SetMatrix(matWorld);
			pInstanceEffect->SetScale(pResourceEffect->m_fScaleForTool);	// VENUS???? Scale?? ?׽?Ʈ?ϱ? ???? ????
		}
		++it;
	}
	
	return TRUE;
}

/**
 * EffectEntity?? ?????? ?Ǿ??? ???? ȣ???? ?ȴ?.
 * \param strName: EffectEntity?? ?̸?
 * \return ???? ????
 */
RwBool CVenusVisualManager::DeleteInstanceEffect(const RwChar* strName)
{
	//Name???? Entity?? ã?Ƽ? Visual Manager???? ?????? ?Ѵ?.
	CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
    if(pGroup)
    {
        CNtlPLRenderGroup::MapEntity::iterator it;
        CNtlPLRenderGroup::MapEntity *pmapEntity = pGroup->GetEntities();
        if(pmapEntity)
        {
            for(it = pmapEntity->begin(); it != pmapEntity->end(); ++it)
            {
                CNtlInstanceEffect* pInstanceEffect = (CNtlInstanceEffect*)(*it).second;

                if (_stricmp(pInstanceEffect->m_pResourceEffect->GetName(), strName) == 0)
                {
                    DeleteEntity(pInstanceEffect);
                    break;
                }                
            }
        }        
    }
	
	//Instance Effect List?? Effect?? ?????? ?Ѵ?.
	CInstanceEffectDialog::GetInstance().BuildInstanceEffect();
	return TRUE;
}


//------------------------------------------------------------------
//	FuncName	: EditInstanceEffect
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::EditInstanceEffect(RwInt32 nAxisType, POINT gap)
{
	RwMatrix matResult,matTemp, matWorld;
	CManipulator::GetInstance().GetEditMatrix(matResult, CVenusConfig::GetInstance().m_nEditMode, nAxisType, gap);

	svdef_InstanceEffect::iterator it;
	for (it = m_svSelectInstanceEffect.begin(); it != m_svSelectInstanceEffect.end(); ++ it)
	{
		CNtlInstanceEffect* pInstanceEffect = (*it);

		matTemp = pInstanceEffect->m_matWorld;
        
		RwMatrixMultiply(&matWorld, &matTemp, &matResult);

		pInstanceEffect->SetMatrix(matWorld);
	}
}

//------------------------------------------------------------------
//	FuncName	: EditInstanceEffect
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::EditInstanceEffect(RwMatrix* pEditMatrix)
{
	svdef_InstanceEffect::iterator it;
	for (it = m_svSelectInstanceEffect.begin(); it != m_svSelectInstanceEffect.end(); ++ it)
	{
		CNtlInstanceEffect* pInstanceEffect = (*it);
        if(!pInstanceEffect)
            continue;

		// ȸ???̳? ???????? ???? ???? ??ǥ?? ???ϹǷ? ???? ???? ?????ߴٰ? ???߿? ?????ش?.
		RwV3d vPosition = *RwMatrixGetPos(&pInstanceEffect->m_matWorld);
		RwMatrixGetPos(&pInstanceEffect->m_matWorld)->x = RwMatrixGetPos(&pInstanceEffect->m_matWorld)->y = RwMatrixGetPos(&pInstanceEffect->m_matWorld)->z = 0.f;

		RwMatrix matResult;
		RwMatrixMultiply(&matResult, &pInstanceEffect->m_matWorld, pEditMatrix);

		RwMatrixTranslate(&matResult, &vPosition, rwCOMBINEPOSTCONCAT);

		pInstanceEffect->SetMatrix(matResult);

        //for(size_t i = 0; i < m_vCreatedEffectInfo.size(); ++i)
        //{
        //    if(m_vCreatedEffectInfo[i]->m_strEffectName == pInstanceEffect->GetName())
        //    {
        //        m_vCreatedEffectInfo[i]->m_mat = matResult;
        //        break;
        //    }
        //}
	}
	CEditDialog::GetInstance().Reset();
}


//------------------------------------------------------------------
//	FuncName	: IntersectionEffect
//	Desc		: Effect?? ???ÿ????ȿ? ???ԵǾ????? üũ?Ѵ?.
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::IntersectionEffect(POINT point, RwBool bShiftButton, RwBool bCtrlButton)
{
	
	// ?????ؼ? ???????? ?ܰ谡 ?ƴϸ? ????Ʈ?? ???? ?????? ?Ѵ?.
	if (!bShiftButton && !bCtrlButton)
	{
		m_svSelectInstanceEffect.clear();
	}

	RwV3d vRayPos, vRayDir;
	CVenusCamera::GetInstance().GetMouseRay(point, vRayPos, vRayDir);

	CNtlInstanceEffect* pIntersectionEffect = NULL;
	RwReal fDist = RwRealMAXVAL, fPickDist;

    CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
    if(!pGroup)
        return;

    CNtlPLRenderGroup::MapEntity::iterator it;
    CNtlPLRenderGroup::MapEntity *pmapEntity = pGroup->GetEntities();

	for(it = pmapEntity->begin(); it != pmapEntity->end(); ++it)
	{
		CNtlInstanceEffect* pInstanceEffect = (CNtlInstanceEffect*)(*it).second;
		RwV3d vPosition = *RwMatrixGetPos(&pInstanceEffect->m_matWorld);
		if (API_IntersectionSphereFromRay(&vRayPos, &vRayDir, &vPosition, 0.5f, &fPickDist))
		{
			if (fDist > fPickDist)
			{
				pIntersectionEffect = pInstanceEffect;
				fDist = fPickDist;
			}
		}
	}

	if (pIntersectionEffect != NULL)
	{
		InsertSelectEffect(pIntersectionEffect, bShiftButton, bCtrlButton);
	}
	CEditDialog::GetInstance().Reset();
	
}

//------------------------------------------------------------------
//	FuncName	: IntersectionEffect
//	Desc		: Effect?? ???ÿ????ȿ? ???ԵǾ??ִ??? ?˻??Ѵ?.
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::IntersectionEffect(POINT OldPoint, POINT CurPoint, RwBool bShiftButton, RwBool bCtrlButton)
{
	
	// ?????ؼ? ???????? ?ܰ谡 ?ƴϸ? ????Ʈ?? ???? ?????? ?Ѵ?.
	if (!bShiftButton && !bCtrlButton)
	{
		m_svSelectInstanceEffect.clear();
	}

	POINT MinPoint, MaxPoint;
	if (OldPoint.x < CurPoint.x) MinPoint.x = OldPoint.x;	else MinPoint.x = CurPoint.x;
	if (OldPoint.y < CurPoint.y) MinPoint.y = OldPoint.y;	else MinPoint.y = CurPoint.y;
	
	if (OldPoint.x > CurPoint.x) MaxPoint.x = OldPoint.x;	else MaxPoint.x = CurPoint.x;
	if (OldPoint.y > CurPoint.y) MaxPoint.y = OldPoint.y;	else MaxPoint.y = CurPoint.y;

    RwInt32 nWidth = RwRasterGetWidth(RwCameraGetRaster(CVenusCamera::GetInstance().GetCamera()));
    RwInt32 nHeight = RwRasterGetHeight(RwCameraGetRaster(CVenusCamera::GetInstance().GetCamera()));

    CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
    if(!pGroup)
        return;

    CNtlPLRenderGroup::MapEntity::iterator it;
    CNtlPLRenderGroup::MapEntity *pmapEntity = pGroup->GetEntities();

	for(it = pmapEntity->begin(); it != pmapEntity->end(); ++it)
	{
		CNtlInstanceEffect* pInstanceEffect = (CNtlInstanceEffect*)(*it).second;

		RwV3d vPosition = *RwMatrixGetPos(&pInstanceEffect->m_matWorld);
        RwV2d v2dPos = API_PL_Calc3DPosTo2D(&vPosition, nWidth, nHeight);

		if (v2dPos.x >= MinPoint.x && v2dPos.y >= MinPoint.y && v2dPos.x < MaxPoint.x && v2dPos.y < MaxPoint.y)
		{
			InsertSelectEffect(pInstanceEffect, bShiftButton, bCtrlButton);
		}
	}
	CEditDialog::GetInstance().Reset();
	
}

//------------------------------------------------------------------
//	FuncName	: InsertSelectEffect
//	Desc		: ???õ? Effect ????Ʈ?? ?߰??Ѵ?.
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::InsertSelectEffect(CNtlInstanceEffect* pInstanceEffect, RwBool bShiftButton, RwBool bCtrlButton)
{
	svdef_InstanceEffect::iterator it = std::find(m_svSelectInstanceEffect.begin(), m_svSelectInstanceEffect.end(), pInstanceEffect);
	if (bCtrlButton)
	{
		if (it != m_svSelectInstanceEffect.end())
		{
			m_svSelectInstanceEffect.erase(it);
		}
	}
	else if (bShiftButton)
	{
		if (it == m_svSelectInstanceEffect.end())
		{
			m_svSelectInstanceEffect.push_back(pInstanceEffect);
		}
	}
	else
	{
		m_svSelectInstanceEffect.push_back(pInstanceEffect);
	}
}

//------------------------------------------------------------------
//	FuncName	: AddSelectInstanceEffectPoint
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::AddSelectInstanceEffectPoint(RwV3d* pEditVector)
{
	if (m_svSelectInstanceEffect.empty()) return;

	svdef_InstanceEffect::iterator it;
	for (it = m_svSelectInstanceEffect.begin(); it != m_svSelectInstanceEffect.end(); ++ it)
	{
		CNtlInstanceEffect* pInstanceEffect = (*it);

		switch(CVenusConfig::GetInstance().m_nEditMode)
		{
		case CVenusConfig::EDIT_MODE_MOVE:
			{
				RwV3d vPosition = pInstanceEffect->GetPosition();
				RwV3dAdd(&vPosition, &vPosition, pEditVector);
				pInstanceEffect->SetPosition(&vPosition);
			}
			break;
		case CVenusConfig::EDIT_MODE_ROTATE:
		case CVenusConfig::EDIT_MODE_SCALE:
			break;
		}
	}
	CEditDialog::GetInstance().Reset();
}

//------------------------------------------------------------------
//	FuncName	: SetSelectInstanceEffectPoint
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::SetSelectInstanceEffectPoint(RwV3d* pEditVector)
{
	if (m_svSelectInstanceEffect.empty()) return;

	svdef_InstanceEffect::iterator it;
	for (it = m_svSelectInstanceEffect.begin(); it != m_svSelectInstanceEffect.end(); ++ it)
	{
		CNtlInstanceEffect* pInstanceEffect = (*it);

		switch(CVenusConfig::GetInstance().m_nEditMode)
		{
		case CVenusConfig::EDIT_MODE_MOVE:
			pInstanceEffect->SetPosition(pEditVector);
			break;
		case CVenusConfig::EDIT_MODE_ROTATE:
		case CVenusConfig::EDIT_MODE_SCALE:
			break;
		}
	}
	CEditDialog::GetInstance().Reset();
}

//------------------------------------------------------------------
//	FuncName	: SetSelectInstanceEffectPoint_X
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::SetSelectInstanceEffectPoint_X(float x)
{
	if (m_svSelectInstanceEffect.empty()) return;

	svdef_InstanceEffect::iterator it;
	for (it = m_svSelectInstanceEffect.begin(); it != m_svSelectInstanceEffect.end(); ++ it)
	{
		CNtlInstanceEffect* pInstanceEffect = (*it);

		switch(CVenusConfig::GetInstance().m_nEditMode)
		{
		case CVenusConfig::EDIT_MODE_MOVE:
			{
				RwV3d vPosition = pInstanceEffect->GetPosition();
				vPosition.x = x;
				pInstanceEffect->SetPosition(&vPosition);
			}
			break;
		case CVenusConfig::EDIT_MODE_ROTATE:
		case CVenusConfig::EDIT_MODE_SCALE:
			break;
		}
	}
	CEditDialog::GetInstance().Reset();
}

//------------------------------------------------------------------
//	FuncName	: SetSelectInstanceEffectPoint_X
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::SetSelectInstanceEffectPoint_Y(float y)
{
	if (m_svSelectInstanceEffect.empty()) return;

    for each(CNtlInstanceEffect* pInstanceEffect in m_svSelectInstanceEffect)
	{
		switch(CVenusConfig::GetInstance().m_nEditMode)
		{
		case CVenusConfig::EDIT_MODE_MOVE:
			{
				RwV3d vPosition = pInstanceEffect->GetPosition();
				vPosition.y = y;
				pInstanceEffect->SetPosition(&vPosition);
			}
			break;
		case CVenusConfig::EDIT_MODE_ROTATE:
		case CVenusConfig::EDIT_MODE_SCALE:
			break;
		}
	}
	CEditDialog::GetInstance().Reset();
}

//------------------------------------------------------------------
//	FuncName	: SetSelectInstanceEffectPoint_X
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::SetSelectInstanceEffectPoint_Z(float z)
{
	if (m_svSelectInstanceEffect.empty()) return;

	svdef_InstanceEffect::iterator it;
	for (it = m_svSelectInstanceEffect.begin(); it != m_svSelectInstanceEffect.end(); ++ it)
	{
		CNtlInstanceEffect* pInstanceEffect = (*it);

		switch(CVenusConfig::GetInstance().m_nEditMode)
		{
		case CVenusConfig::EDIT_MODE_MOVE:
			{
				RwV3d vPosition = pInstanceEffect->GetPosition();
				vPosition.z = z;
				pInstanceEffect->SetPosition(&vPosition);
			}
			break;
		case CVenusConfig::EDIT_MODE_ROTATE:
		case CVenusConfig::EDIT_MODE_SCALE:
			break;
		}
	}
	CEditDialog::GetInstance().Reset();
}

//------------------------------------------------------------------
//	FuncName	: CreateEventEffect
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
CNtlPLEntity* CVenusVisualManager::CreateEventEffect(const RwChar *pKey)
{
	NTL_FUNCTION("CVenusVisualManager::CreateEventEffect");

	CNtlPLProperty *pProperty = CNtlPLPropertyContainer::GetInstance()->GetProperty(PLENTITY_EFFECT, pKey);
	if (pProperty == NULL)
	{
		return NULL;
	}

	CNtlPLEntity *pEntity = CNtlPLEntityFactory::CreateEntity(PLENTITY_EFFECT, NULL);
	if(pEntity)
	{
		pEntity->SetProperty(pProperty);
		pEntity->Create();
	}

    CNtlInstanceEffect* pEffect = (CNtlInstanceEffect*)pEntity;
    m_nEventEffectUsedMemory += pEffect->GetMemoryUseSize();

	NTL_RETURN(pEntity);
}

//------------------------------------------------------------------
//	FuncName	: DeleteEventEffect
//	Desc		: 
//	Parameter	: 
//	Return		: 
//------------------------------------------------------------------
void CVenusVisualManager::DeleteEventEffect(CNtlPLEntity *pEntity)
{
	NTL_FUNCTION("CNtlPLVisualManager::DeleteEntity");

    CNtlInstanceEffect* pEffect = (CNtlInstanceEffect*)pEntity;
    m_nEventEffectUsedMemory -= pEffect->GetMemoryUseSize();
    m_nEventEffectUsedMemory = max(0, m_nEventEffectUsedMemory);

	pEntity->Destroy();

	CNtlPLEntityFactory::DeleteEntity(pEntity); 
	
	NTL_RETURNVOID();
}

// Effect?? Update ?????ؼ? ???? ?ɶ? ȣ???Ǵ? ?Լ?
// Repeat ???ɿ? ???? ó???? ???⼭ ?Ѵ?.
void CVenusVisualManager::AlarmAutoDelete( CNtlPLEntity *pEntity ) 
{
	CNtlInstanceEffect* pInstanceEffect = (CNtlInstanceEffect*)pEntity;		// ?? ?Լ??? ???????? Entity?? Effect ???̴?.

    if(m_bGenerateBS && m_pBSCurrEffect == pEntity)
    {
        m_pBSCurrEffect = NULL;
        return;
    }

	if(CVenusConfig::GetInstance().m_bTimeRepeat)
	{
		// ?????? ?????? ???ο? ????Ʈ?? ?????Ѵ?.
		// Instance?? ?????? Resource?? ?̸??? ?????ɼ??? ?ֱ? ??????, Resource???? ?̸??? ?????´?.
		std::string strName = pInstanceEffect->m_pResourceEffect->GetName();
		RwMatrix matWorld = pInstanceEffect->m_matWorld;

		CNtlInstanceEffect* pNewInstanceEffect = (CNtlInstanceEffect*)CreateEntity(PLENTITY_EFFECT, strName.c_str());
		if(!pNewInstanceEffect)
		{
			return;
		}
		pNewInstanceEffect->SetMatrix(matWorld);

		// ???? ????Ʈ?? ?????? ?????? ???? ???? ???ο? ???? ?ִ´?.
		svdef_InstanceEffect::iterator selete_it = std::find(m_svSelectInstanceEffect.begin(), m_svSelectInstanceEffect.end(), pInstanceEffect);
		if (selete_it != m_svSelectInstanceEffect.end())
		{
			InsertSelectEffect((*selete_it), FALSE, TRUE);
			InsertSelectEffect(pNewInstanceEffect, TRUE, FALSE);
		}
	}
	else
	{
		// ?????? ????Ʈ????Ʈ?߿? ???? ?ִٸ? ??????.
		svdef_InstanceEffect::iterator selete_it = std::find(m_svSelectInstanceEffect.begin(), m_svSelectInstanceEffect.end(), pInstanceEffect);
		if (selete_it != m_svSelectInstanceEffect.end())
		{
			InsertSelectEffect((*selete_it), FALSE, TRUE);
		}
	}
}

void CVenusVisualManager::OnGenerateBoundingBoxAll()
{
    m_bGenerateBS = TRUE;
    m_nBSIndex = 0;

    m_pBSCurrEffect = CreateEffect(m_nBSIndex);
}

CNtlInstanceEffect* CVenusVisualManager::CreateEffect( RwUInt32 nIndex ) 
{
    if(nIndex < CVenusPropertyContainer::GetInstance().m_svEffectProperty.size())
    {
        CNtlPLProperty* pResourceEffect = CVenusPropertyContainer::GetInstance().m_svEffectProperty[m_nBSIndex];
        return (CNtlInstanceEffect*)CreateEntity(PLENTITY_EFFECT, pResourceEffect->GetName());        
    } 

    return NULL;
}

// ?̺?Ʈ ?????߿??? ?̺?Ʈ ???? ????Ʈ?? ???????ϱ? ???? ?Լ?
// ?̺?Ʈ???? ?????? ????Ʈ???? ?????? ??ġ?? ?ʴ´?.
void CVenusVisualManager::RenderEffect( RwBool bRender ) 
{
    CNtlPLRenderGroup::MapEntity::iterator it;

    CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
    if(!pGroup)
        return;

    CNtlPLRenderGroup::MapEntity *pEntities = pGroup->GetEntities();
    for(it = pEntities->begin(); it != pEntities->end(); ++it)
    {
        CNtlInstanceEffect* pEffect = (CNtlInstanceEffect*)(*it).second;
        if(pEffect)
        {
            pEffect->SetVisible(bRender);
        }
    }
}

RwUInt32 CVenusVisualManager::GetRenderingEffectUsedMemory() 
{
    RwUInt32 uiMemoryUseSize = 0;

    if (CVenusConfig::GetInstance().m_nCurrentLayer == EDIT_LAYER_EVENT)
    {
        uiMemoryUseSize = m_nEventEffectUsedMemory;
    }
    else
    {
        CNtlPLRenderGroup::MapEntity::iterator it;

        CNtlPLRenderGroup *pGroup = FindInstanceGroup(PLENTITY_EFFECT);
        if(!pGroup)
            return 0;

        CNtlPLRenderGroup::MapEntity *pEntities = pGroup->GetEntities();
        for(it = pEntities->begin(); it != pEntities->end(); ++it)
        {
            CNtlInstanceEffect* pEffect = (CNtlInstanceEffect*)(*it).second;
            if(pEffect)
            {
                uiMemoryUseSize += pEffect->GetMemoryUseSize();
            }
        }
    }

    return uiMemoryUseSize;
}