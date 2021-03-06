/*****************************************************************************
 *
 * File			: NTLCamera.h
 * Author		: agebreak
 * Copyright	: (??)NTL
 * Date			: 2005. 7. 14	
 * Abstract		: NTl Camera
 *****************************************************************************
 * Desc         : 
 *
 *****************************************************************************/
#ifndef __NTL_GAME_CAMERA__
#define __NTL_GAME_CAMERA__

#include "NtlCamera.h"
#include "InputHandler.h"
#include "NtlPLCharacter.h"
#include "NtlPostEffectCamera.h"

#define MIN_DELTA_EPLISION	0.00000001f

class CNtlGameCamera : public CNtlPostEffectCamera
{
public:
    enum ECameraMove
    {
        MOVE_UP,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,        
        MOVE_FORWARD,
        MOVE_BACKWARD,
    };

public:
	CNtlGameCamera(void);
	~CNtlGameCamera(void);

	void	HandleMouse(RwReal fAngleX, RwReal fAngleY);
	void	HandleMouse(RwReal fDeltaZ);

	void	Update(RwReal fElapsed);

	int		MouseDownHandler(unsigned int uiMouseData);
	int		MouseUpHandler(unsigned int uiMouseData);
	int		MouseMoveHandler(unsigned int uiMouseData);
    void    OnPanning(ECameraMove eMove);
	int		MouseWheelHandler(unsigned int uiMouseData);
	int		KeyboardDownHandler(unsigned int uiKeyData);

    void    SetLeftView();                          ///< ?????? ?????? ??????.
    void    SetFrontView();                         ///< ?????? ?????? ??????.
    void    SetTopView();                           ///< ?????? ?????? ??????.
    void    SetRightView();                         ///< ?????? ???????? ??????.
    void    SetRearView();                          ///< ?????? ?????? ??????.
    void    SetBottomView();                        ///< ?????? ???? ??????.
    void    SetEnableFreeCamera(RwBool bEnable);    ///< ???? ī?޶? On/Off
    RwBool  GetEnableFreeCamera() {return m_bFreeCamera;}

    void    SetDistance(RwReal fDistance);          ///< ?Ÿ??? ?????Ѵ?.
    RwReal  GetDistance() {return m_fDistance;}

    RwReal  GetRatioDistance() {return m_fRatioDistance;} ///< ?Ÿ? ?????? ??ȯ?Ѵ?.
    void    SetRatioDistance(RwReal fRatioDistance) {m_fRatioDistance = fRatioDistance;} ///< ?Ÿ? ?????? ?????Ѵ?

    void    SetCameraOffset(const RwV3d& vOffset); 
    void    SetCameraLookAtOffset(const RwV3d& vOffset); 

private:
    RwReal	m_fDistance;		//Camera?? Target???? ?Ÿ?
    RwBool  m_bFreeCamera;      // ???? ī?޶? ?÷???

    RwReal	m_fMaxDistance;
    RwReal	m_fMinDistance;
    RwReal	m_fSmoothDistance;
    RwReal	m_fPrefDistance;
    RwReal  m_fRatioDistance;

    RwReal	m_fXAngle;
    RwReal	m_fMaxXAngle;
    RwReal	m_fMinXAngle;
    RwReal	m_fSmoothXAngle;
    RwReal	m_fPrefXAngle;

    RwReal	m_fYAngle;
    RwReal	m_fMaxYAngle;
    RwReal	m_fMinYAngle;
    RwReal	m_fSmoothYAngle;
    RwReal	m_fOldSmoothYAngle;
    RwReal	m_fPrefYAngle;

    RwMatrix *m_pCachedMatrix;

    RwV3d	m_vLookAt; 
    RwV3d   m_vLookAtDelta;
    RwReal	m_fLookAtHeight;
    // ī?޶? ????ũ ????
    RwV3d   m_vCamOffset;
    RwV3d   m_vLookAtOffset;

    INPUT_HANDLE	m_handleMouseWheel;
    INPUT_HANDLE	m_handleMouseMove;
    INPUT_HANDLE	m_handleMouseDown;
    INPUT_HANDLE	m_handleMouseUp;
    INPUT_HANDLE	m_handleKeyDown;

    RwV2d	m_OldMouseMove;
    RwBool	m_bMouseMoveValidFlag;
    RwBool  m_bMouseMoveMButtonFlag;
    CNtlPLCharacter *m_pTarget;

};

#endif