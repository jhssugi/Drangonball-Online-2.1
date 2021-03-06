#ifndef _DBO_TSACTAVATARDEAD_H_
#define _DBO_TSACTAVATARDEAD_H_


#include "DboTSCoreDefine.h"


/**
	avatar dead action
*/


class CDboTSActAvatarDead : public CNtlTSAction
{
	NTL_TS_DECLARE_RTTI

// Member variables
protected:

	unsigned char						m_ucStart;
	NTL_TS_TA_ID						m_TaIdx;

// Constructions
public:
	CDboTSActAvatarDead( void );

// Methods
public:
	virtual unsigned int				GetEntityType( void ) const { return DBO_ACT_TYPE_ID_ACT_AVATARDEAD; }


	unsigned char						GetStart( void ) const;
	NTL_TS_TA_ID						GetTaIdx( void ) const;

// Implementations
protected:
	// 스크립트로 부터 컴포넌트 객체의 데이터를 로딩 및 입력 시키기 위한 함수
	virtual	void						ApplyScriptDataForScript( const CNtlTSScrProperty& clProperty );
	virtual	void						TakeScriptDataForScript( CNtlTSScrProperty& clProperty );
};


inline unsigned char CDboTSActAvatarDead::GetStart( void ) const
{
	return m_ucStart;
}

inline NTL_TS_TA_ID CDboTSActAvatarDead::GetTaIdx( void ) const
{
	return m_TaIdx;
}


#endif