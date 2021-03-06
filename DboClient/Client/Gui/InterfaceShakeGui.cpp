#include "precomp_dboclient.h"
#include "InterfaceShakeGui.h"

// core
#include "NtlDebug.h"
#include "NtlPLDef.h"

// gui
#include "gui_guimanager.h"
#include "mouse.h"

// simulation
#include "InputActionMap.h"
#include "NtlSLEvent.h"

// presentation
#include "NtlPLGuiManager.h"
#include "NtlPLEvent.h"

// Dbo
#include "DialogManager.h"
#include "DboGlobal.h"
 
/**
* \brief 持失切
*/
CInterfaceShakeGui::CInterfaceShakeGui( const RwChar* pName ) 
: CNtlPLGui(pName)
{
	m_nOldMouseX = 0;
	m_nMouseX = 0;
	m_nTrackValue = 0;
	m_nOffsetX = 0;
	m_nTrackMin = 0;
	m_nTrackMax = 0;
	m_dwLeftKey = 0;
	m_dwRightKey = 0;

	m_bMouseToggle = FALSE;
	m_bKeyToggle = FALSE;
	m_bComplete = FALSE;
	
	m_pPgbTrack = NULL;
	m_slotKeyDown = NULL;

	m_fElapsedTime = 0.0f;
	m_fShakeElapsedTime = 0.0f;
	m_fKeyDownElapsedTime = 0.0f;
}

/**
* \brief 社瑚切
*/
CInterfaceShakeGui::~CInterfaceShakeGui() 
{

}

/**
* \brief Create
*/
RwBool CInterfaceShakeGui::Create() 
{
	NTL_FUNCTION( "CInterfaceShakeGui::Create" );

	if( !CNtlPLGui::Create("", dSHAKE_FILE_SURFACE , dSHAKE_FILE_FRAME ) )
		NTL_RETURN( FALSE );

	CNtlPLGui::CreateComponents(CNtlPLGuiManager::GetInstance()->GetGuiManager());

	// Get Component
	m_pThis = (gui::CDialog*)GetComponent( "dlgMain" );
	m_pPgbTrack = (gui::CProgressBar*)GetComponent( "pgbTrack" );
	m_pPanTrack = (gui::CPanel*)GetComponent( "pnlTrackBack" );
	m_pPgbTrack->GetRange( m_nTrackMin, m_nTrackMax );

	// Priority
	m_pThis->SetPriority( dDIALOGPRIORITY_DEFAULT );

	//m_pFlashLeftArrow = (gui::CFlash*)GetComponent( "flaLeftKey" );
	//m_pFlashRightArrow = (gui::CFlash*)GetComponent( "flaRightKey" );
	m_pFlashMouseBack = (gui::CFlash*)GetComponent( "flaMouseBack" );

	//m_pBtnLeftKey = (gui::CButton*)GetComponent( "btnLeftKey" );
	//m_pBtnRightKey = (gui::CButton*)GetComponent( "btnRightKey" );
	m_pStbMouse = (gui::CStaticBox*)GetComponent( "stbMouse" );

	Show( false );

	// Link
	LinkMsg( g_EventKnockDownNfy, 0 );
	LinkMsg( g_EventKnockDownWakeUpNfy, 0 );

	NTL_RETURN( TRUE );
}

/**
* \brief Destroy
*/
VOID CInterfaceShakeGui::Destroy() 
{
	NTL_FUNCTION( "CInterfaceShakeGui::Destroy" );

	// Unlink
	UnLinkMsg( g_EventKnockDownWakeUpNfy );
	UnLinkMsg( g_EventKnockDownNfy );

	CNtlPLGui::DestroyComponents();
	CNtlPLGui::Destroy(); 

	NTL_RETURNVOID();
}

/**
* \brief HandleEvents
*/
VOID CInterfaceShakeGui::HandleEvents( RWS::CMsg &msg ) 
{
	NTL_FUNCTION("CInterfaceShakeGui::HandleEvents");
	
	// 核陥錘戚 鞠醸陥澗 Notify
	if( msg.Id == g_EventKnockDownNfy )
	{
		// 昔斗凪戚什研 句錘陥.
		GetDialogManager()->OpenDialog( DIALOG_INTERFACE_SHAKE );
	}
	// 核陥錘戚 熱携陥澗 Notify ( 昔斗凪戚什 UI亜 伸形 赤澗 雌伐拭辞幻 閤焼辞 坦軒 )
	else if ( msg.Id == g_EventKnockDownWakeUpNfy 
		&& GetDialogManager()->IsOpenDialog( DIALOG_INTERFACE_SHAKE ) )
	{
		// 昔斗凪戚什研 丸澗陥.
		GetDialogManager()->CloseDialog( DIALOG_INTERFACE_SHAKE );
	}

	NTL_RETURNVOID();
}

/**
* \brief SwitchDialog
* DialogManager拭辞 OpenDialog蟹 CloseDialog研 背早聖 井酔 叔楳 吉陥.
* \param bOpen Open = TRUE / Close = FALSE
* \return 失因食採
*/
RwInt32 CInterfaceShakeGui::SwitchDialog( bool bOpen ) 
{
	if( bOpen == TRUE )
		ShowInterface();
	else if( bOpen == FALSE )
		CloseInterface();	

	return 1;
}

/**
* \brief Update
* \param fElapsed 井引獣娃
*/
VOID CInterfaceShakeGui::Update( RwReal fElapsed ) 
{
	// 獣娃 装亜
	m_fElapsedTime += fElapsed;
	m_fShakeElapsedTime += fElapsed;
	m_fKeyDownElapsedTime += fElapsed;
	m_fElapsedShakeTime += fElapsed;

	// 巴掘習 穣汽戚闘
	m_pFlashMouseBack->Update( fElapsed );

	// 原酔什 是帖 飴重
	m_nMouseX = CMouse::GetX();					// 薄仙 原酔什
	m_nOffsetX = (m_nMouseX - m_nOldMouseX);		// 戚穿 原酔什拭辞 装亜葵 姥馬奄
	m_nOldMouseX = m_nMouseX;					// 戚穿 原酔什澗 薄仙税 原酔什亜 吉陥.

	// True 酔著, False 疎著
	if( m_bMouseToggle )
	{
		if( m_nOffsetX > dSHAKE_MOVE_DISTANCE )
		{
			ProgressUpdateFromMouse();
			m_bMouseToggle = FALSE;
			m_pStbMouse->SetPosition( dSHAKE_MOUSE_LEFT_X, dSHAKE_MOUSE_Y );
		}
	}
	else
	{
		if( m_nOffsetX < -dSHAKE_MOVE_DISTANCE )
		{
			ProgressUpdateFromMouse();
			m_bMouseToggle = TRUE;
			m_pStbMouse->SetPosition( dSHAKE_MOUSE_RIGHT_X, dSHAKE_MOUSE_Y );
		}
	}

	// ProgressBar Set
	m_pPgbTrack->SetPos( m_nTrackValue );

	// Todo : Time Over
	if( m_fElapsedTime > dSHAKE_MAX_WAKEUP )
	{
		GetDialogManager()->CloseDialog( DIALOG_INTERFACE_SHAKE );
	}

	// ProgressBar税 Max拭 陥陥牽檎 失因
	if( m_nTrackValue >= m_nTrackMax && m_bComplete == FALSE )
		m_bComplete = TRUE;

	// 失因 馬檎 鳶填聖 劾形爽壱 Dialog研 丸焼醤 廃陥.
	if( m_bComplete )
	{
		// 失因梅希虞亀 置社 獣娃戚 照鞠檎 奄陥鍵陥.
		if( m_fElapsedTime < dSHAKE_MIN_WAKEUP )
			return;
		
		// KnockDown税 噺差 鳶填
		GetDboGlobal()->GetGamePacketGenerator()->SendCharKnockDownReleaseNfy();
		
		// Dialog研 丸焼層陥.
		GetDialogManager()->CloseDialog( DIALOG_INTERFACE_SHAKE );
	}
}

/**
* \brief 原酔什稽 昔廃 ProgressBar 装亜
*/
VOID CInterfaceShakeGui::ProgressUpdateFromMouse() 
{
	// 戚究
	if( m_fShakeElapsedTime < dSHAKE_MOUSE_FIRSTTIME )
	{
		m_nTrackValue += dSHAKE_ADVANTAGE_FIRST;
	}
	else if( m_fShakeElapsedTime < dSHAKE_MOUSE_SECONDTIME )
	{
		m_nTrackValue += dSHAKE_ADVANTAGE_SECOND;
	}
	else
		m_nTrackValue += dSHAKE_ADVANTAGE_THIRD;

	// 原酔什 泌級奄 井引獣娃 段奄鉢 & 原酔什 Offset 段奄鉢
	m_fShakeElapsedTime = 0.0f;
	m_nOffsetX = 0;

	// 薦廃
	if( m_nTrackValue > m_nTrackMax )
		m_nTrackValue = m_nTrackMax;
}

/**
* \brief 徹左球稽 昔廃 ProgressBar 装亜
*/
VOID CInterfaceShakeGui::ProgressUpdateFromKeyboard() 
{
	// 戚究
	if( m_fKeyDownElapsedTime < dSHAKE_KEYBOARD_FIRSTTIME )
	{
		m_nTrackValue += dSHAKE_ADVANTAGE_FIRST;
	}
	else if ( m_fKeyDownElapsedTime < dSHAKE_KEYBOARD_SECONDTIME )
	{
		m_nTrackValue += dSHAKE_ADVANTAGE_SECOND;
	}
	else
		m_nTrackValue += dSHAKE_ADVANTAGE_THIRD;

	// KeyDown 井引獣娃 段奄鉢
	m_fKeyDownElapsedTime = 0.0f;

	// Track 葵 薦廃
	if( m_nTrackValue > m_nTrackMax )
		m_nTrackValue = m_nTrackMax;
}

/**
* \brief UI研 伸壱 琶推廃 舛左研 段奄鉢廃陥.
*/
VOID CInterfaceShakeGui::ShowInterface( VOID ) 
{
	m_nMouseX = CMouse::GetX();
	m_nOldMouseX = m_nMouseX ;
	m_nTrackValue = 0;
	m_nOffsetX = 0;
	m_bMouseToggle = FALSE;
	m_bKeyToggle = FALSE;
	m_bComplete = FALSE;
	m_fElapsedTime = 0.0f;
	m_fShakeElapsedTime = 0.0f;
	m_fKeyDownElapsedTime = 0.0f;
	m_dwLeftKey = GetInputActionMap()->FindKey( ACTION_AVATAR_LEFTTURN ) & 0xFF;
	m_dwRightKey = GetInputActionMap()->FindKey( ACTION_AVATAR_RIGHTTURN ) & 0xFF;
	
	GetNtlGuiManager()->AddUpdateFunc( this );

	m_pFlashMouseBack->RestartMovie();

	Show( TRUE );
}

/**
* \brief UI研 丸焼爽悟 背薦背操醤 馬澗 析級聖 呪楳廃陥.
*/
VOID CInterfaceShakeGui::CloseInterface( VOID ) 
{
	m_slotKeyDown = 0;
	m_slotKeyUp = 0;
	
	GetNtlGuiManager()->RemoveUpdateFunc( this );

	Show( FALSE );
}

/**
* \brief OnKeyDown
* 紬拷生稽 陥戚杖稽益 古艦煽税 SigCaptureKeyDown拭 尻衣吉 敗呪
*/
VOID CInterfaceShakeGui::OnKeyDown( gui::CComponent* pComponent, CInputDevice* pDevice, const CKey& key ) 
{
	if( key.m_dwVKey == m_dwLeftKey )
	{
		m_pBtnLeftKey->Enable( FALSE );
		if( m_bKeyToggle )
		{
			ProgressUpdateFromKeyboard();
			m_bKeyToggle = FALSE;
		}
	}
	else if( key.m_dwVKey == m_dwRightKey )
	{
		m_pBtnRightKey->Enable( FALSE );
		if( !m_bKeyToggle )
		{
			ProgressUpdateFromKeyboard();
			m_bKeyToggle = TRUE;
		}
	}
}

/**
* \brief OnKeyUp
* 紬拷生稽 陥戚杖稽益 古艦煽税 SigCaptureKeyUp拭 尻衣吉 敗呪
*/
VOID CInterfaceShakeGui::OnKeyUp( gui::CComponent* pComponent, CInputDevice* pDevice, const CKey& key ) 
{
	if( key.m_dwVKey == m_dwLeftKey )
		m_pBtnLeftKey->Enable( TRUE );
	else if( key.m_dwVKey == m_dwRightKey )
		m_pBtnRightKey->Enable( TRUE );
}

