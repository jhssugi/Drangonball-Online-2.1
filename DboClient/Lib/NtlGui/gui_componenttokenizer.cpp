#include "gui_precomp.h"
#include "gui_componenttokenizer.h"

CallComponentTokenPack g_fnCallPack = NULL;

void LinkGuiScriptLoad(CallComponentTokenPack fn)
{
	g_fnCallPack = fn;
}

void UnLinkGuiScriptLoad(void)
{
	g_fnCallPack = NULL;
}

START_GUI

// MultiByte
CComponentTokenizer::CComponentTokenizer(const std::string &filename, bool bUsePack)
{
	m_pData		 = NULL;

	m_bSuccess = Load(filename.data(), bUsePack);
	if(!m_bSuccess)
		return;

	m_strFileName = filename;
	m_nPeekPos = 0;
	m_nLastLine = 0;
	m_bInRemark = false;

	Tokenize();
}

CComponentTokenizer::CComponentTokenizer(const char *pMemory, int nSize)
{
	m_pData	= NULL;
	m_nPeekPos = 0;
	m_nLastLine = 0;
	m_bInRemark = false;
	m_bSuccess = true;

	m_nTotalSize = nSize;
	m_pData = NTL_NEW char[nSize+1];
	memcpy(m_pData, pMemory, nSize);
	m_pData[nSize] = 0;

	Tokenize();
}

CComponentTokenizer::~CComponentTokenizer()
{
	NTL_ARRAY_DELETE( m_pData );
}

bool CComponentTokenizer::Load(const char *pFileName, bool bUsePack)
{
	NTL_ARRAY_DELETE( m_pData );

	if(g_fnCallPack && bUsePack)
	{
		(*g_fnCallPack)(pFileName, (void**)&m_pData, (unsigned int*)&m_nTotalSize);
	}
	else
	{
		if(m_pData == NULL)
		{
			FILE *fp = NULL;

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
			if(fopen_s(&fp, pFileName, "rb") != 0)
				return false;
#else
			fp = fopen(pFileName, "rb");
			if(fp == NULL)
				return false;
#endif

			fseek(fp, 0, SEEK_END);
			int nSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			m_nTotalSize = nSize;
			m_pData = NTL_NEW char[nSize];

			fread(m_pData, nSize, 1, fp);
			fclose(fp);
		}
	}

	return true;
}

bool CComponentTokenizer::IsSuccess(VOID)
{
	return m_bSuccess;
}

bool CComponentTokenizer::IsSpace(char c)
{
	const char *pSpace = " \t\r\n";

	return strchr(pSpace, c) != NULL;
}


bool CComponentTokenizer::IsOperator(char c)
{
	const char *pOperators = ",=();{}<+-*/>";

	return strchr(pOperators, c) != NULL;
}


bool CComponentTokenizer::IsRemark(char c, int position)
{
	if (m_bInRemark)
	{
		if (c == '\n') 
		{	
			m_bInRemark = false;
		}
		return true;
	}
	else
	{
		if (c == '/')
		{
			if (
				position+1 < m_nTotalSize &&
				m_pData[position+1] == '/')
			{
				m_bInRemark = true;
				return true;
			}
		}
		else if (c == '#')
		{
			m_bInRemark = true;
			return true;
		}
	}

	return false;
}




VOID CComponentTokenizer::Tokenize(VOID)
{
	int cur_pos = 0;
	int cur_line = 0;
	while (cur_pos < m_nTotalSize)
	{
		while (
			cur_pos < m_nTotalSize && 
			(
				IsRemark(m_pData[cur_pos], cur_pos) ||
				IsSpace(m_pData[cur_pos])
			))
		{
			if (m_pData[cur_pos] == '\n') cur_line++;
			cur_pos++;
		}
		if (cur_pos == m_nTotalSize) break;

		if (IsOperator(m_pData[cur_pos]))
		{
			m_Tokens.push_back(CToken(std::string(&m_pData[cur_pos], 1), cur_pos, cur_line));
			cur_pos++;
		}
		else
		{
			int temp_pos = cur_pos;
			if (m_pData[temp_pos] == '"')
			{
				int num_chars = 0;
				temp_pos++;
				while (temp_pos < m_nTotalSize)
				{
					if (m_pData[temp_pos] == '"')
					{
						if (temp_pos+1>=m_nTotalSize || m_pData[temp_pos+1] != '"') break;
						else
						{
							temp_pos++;
						}
					}
					temp_pos++;
					num_chars++;
				}
				if (temp_pos == m_nTotalSize)
				{
					WriteError("Missing '""' following '""'-begin");
					break;
				}

				assert(num_chars < TOKEN_BUFF_LEN);
//				char *temp = NTL_NEW char[num_chars+1];
				m_pTemp[num_chars] = 0;
				int ofs = 1;
				for (int i=0;i<num_chars;i++)
				{
					m_pTemp[i] = m_pData[cur_pos+ofs+i];
					if (m_pData[cur_pos+ofs+i] == '"') 
					{
						ofs++;
					}
				}
				cur_pos = temp_pos+1;

				m_Tokens.push_back(CToken(std::string(m_pTemp), cur_pos, cur_line));
//				delete temp;
			}
			else
			{
				while (temp_pos < m_nTotalSize &&
					!IsSpace(m_pData[temp_pos]) &&
					!IsOperator(m_pData[temp_pos]) &&
					!IsRemark(m_pData[temp_pos], cur_pos))
				{
					temp_pos++;
				}
				int tok_size = temp_pos-cur_pos;
				m_Tokens.push_back(CToken(std::string(&m_pData[cur_pos], tok_size), cur_pos, cur_line));
				cur_pos += tok_size;
			}
		}
	}
}

std::string CComponentTokenizer::PeekNextToken(int *offset/*=NULL*/)
{
	if (m_nPeekPos >= (int) m_Tokens.size())
	{
		return "";
	}
	if (offset != NULL) *offset = m_Tokens[m_nPeekPos].offset;
	return m_Tokens[m_nPeekPos++].token;
}


std::string CComponentTokenizer::GetNextToken(int *offset/*=NULL*/)
{
	m_nPeekPos = 0;
	m_nLastLine = m_Tokens[0].line;
	std::string token = m_Tokens[0].token;
	if (offset != NULL) *offset = m_Tokens[0].offset;

	m_Tokens.pop_front();

	return token;
}

VOID CComponentTokenizer::PopToPeek(VOID)
{
	while (m_nPeekPos > 0)
	{
		m_nPeekPos--;
		m_Tokens.pop_front();
	}
}


std::string CComponentTokenizer::WriteError(std::string err_msg)
{
	std::string str = "component tokenizer Tokenize() error";
	return str;
}

//////////////////////////////////////////////////////////////////////////
// Unicode

CComponentTokenizerW::CComponentTokenizerW(const std::string &filename, bool bUsePack)
{
	m_pData		 = NULL;

	m_bSuccess = Load(filename.data(), bUsePack);
	if(!m_bSuccess)
		return;

	m_strFileName = filename;
	m_nPeekPos = 0;
	m_nLastLine = 0;
	m_bInRemark = false;

	Tokenize();
}

CComponentTokenizerW::CComponentTokenizerW(const wchar_t *pMemory, int nSize)
{
	m_pData	= NULL;
	m_nPeekPos = 0;
	m_nLastLine = 0;
	m_bInRemark = false;
	m_bSuccess = true;

	m_nTotalSize = nSize;
	m_pData = NTL_NEW wchar_t[nSize+1];
	m_pData[nSize] = '\0';

	wcscpy_s( m_pData, nSize+1 , pMemory );
	Tokenize();
}

CComponentTokenizerW::~CComponentTokenizerW()
{
	NTL_ARRAY_DELETE( m_pData );
}

bool CComponentTokenizerW::Load(const char *pFileName, bool bUsePack)
{
	NTL_ARRAY_DELETE( m_pData );

	// Kell's Comment : UTF-16/UCS2 ?? ????????. ?????? ?????? MultiByte?? ??????.
	// UTF-16?? Byte-order Mark?? 2byte ?? FF FE ?? ???????? ?? ?????? ????????
	// ?????????? Word???? ???????? ????????. (????)

	const BYTE byBomOffset = 2;
	const BYTE abyUnicodeBom[2] = { 0xFF, 0xFE };

	if( g_fnCallPack && bUsePack )
	{
		CHAR* pData = NULL;
		INT iSize = 0;

		(*g_fnCallPack)(pFileName, (void**)&pData, (unsigned int*)&iSize);
		CHAR* pBuffer = NTL_NEW CHAR[iSize+1];
		memcpy( pBuffer, pData, sizeof(char)*iSize);
		pBuffer[iSize] = '\0';
		
		// // UTF - 16?? ??????
		if( abyUnicodeBom[0] == pBuffer[0] &&
			abyUnicodeBom[1] == pBuffer[1] )
		{
			int nStrLen = (int)strlen( pBuffer+2 );
			// Wide???? ?????? 2Byte ?? ??????.
			// ???? BOM ?????? ?? ???? 2?? ?????? ???? ????????.
			m_nTotalSize = nStrLen;
			m_pData = NTL_NEW WCHAR[m_nTotalSize+1];
			memcpy( m_pData, pBuffer+2, iSize-2 );
			m_pData[m_nTotalSize] = L'\0';
		}
		// Ansi
		else
		{
			int nStrLen = (int)strlen( pData );
			m_nTotalSize = nStrLen;
			m_pData = NTL_NEW WCHAR[m_nTotalSize+1];
			::MultiByteToWideChar( GetACP(), 0, pBuffer, -1, m_pData, m_nTotalSize );
			m_pData[m_nTotalSize] = L'\0';
		}
	
		NTL_ARRAY_DELETE( pData );
		NTL_ARRAY_DELETE( pBuffer );
	}
	else
	{
		FILE *fp = NULL;

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		if(fopen_s(&fp, pFileName, "rb") != 0)
			return false;
#else
		fp = fopen(pFileName, "rb");
		if(fp == NULL)
			return false;
#endif
		// ???? 2 Byte?? ????????.
		BYTE abyHtml_Bom[2] = { 0, };
		fread( abyHtml_Bom, 2, 1, fp );

		// Unicode(LE)?? ???? ???? Assert?? ??????.
		// ?????? MultiByte?? Unicode, 2?? ?? ?????????? ????.
		// assert( abyUnicodeBom == abyHtml_Bom );

		// UTF-16(Little-Endian)
		if( abyUnicodeBom[0] == abyHtml_Bom[0] &&
			abyUnicodeBom[1] == abyHtml_Bom[1] )
		{
			fseek(fp, byBomOffset, SEEK_END);
			int nSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// Unicode?? 2byte?? 1character?? ????????.
			int nStrLen = m_nTotalSize = (nSize/2);
			m_pData = NTL_NEW wchar_t[nStrLen];
			m_pData[nStrLen-1] = L'\0';

			fread(m_pData, nSize, 1, fp);
		}
		// Ansi
		else 
		{
			fseek(fp, 0, SEEK_END);
			int nSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			m_nTotalSize = nSize;
			char* pData = NTL_NEW char[nSize+1];
			pData[nSize] = 0;

			fread(pData, nSize, 1, fp);

			m_pData = NTL_NEW wchar_t[nSize];

			// Wide Character?? ????
			::MultiByteToWideChar( GetACP(), 0,pData, -1, m_pData, nSize );

			// Data ????
			NTL_ARRAY_DELETE( pData );
		}

		fclose(fp);		
	}

	return true;
}

bool CComponentTokenizerW::IsSuccess(VOID)
{
	return m_bSuccess;
}

bool CComponentTokenizerW::IsSpace(wchar_t c)
{
	const wchar_t *pSpace = L" \t\r\n";

	return wcschr(pSpace, c) != NULL;
}


bool CComponentTokenizerW::IsOperator(wchar_t c)
{
	const wchar_t *pOperators = L",=();{}<+-*/>";

	return wcschr(pOperators, c) != NULL;
}


bool CComponentTokenizerW::IsRemark(wchar_t c, int position)
{
	if (m_bInRemark)
	{
		if (c == '\n') 
		{	
			m_bInRemark = false;
		}
		return true;
	}
	else
	{
		if (c == '/')
		{
			if (
				position+1 < m_nTotalSize &&
				m_pData[position+1] == '/')
			{
				m_bInRemark = true;
				return true;
			}
		}
		else if (c == '#')
		{
			m_bInRemark = true;
			return true;
		}
	}

	return false;
}

VOID CComponentTokenizerW::Tokenize(VOID)
{
	int cur_pos = 0;
	int cur_line = 0;
	while (cur_pos < m_nTotalSize)
	{
		while (
			cur_pos < m_nTotalSize && 
			(
			IsRemark(m_pData[cur_pos], cur_pos) ||
			IsSpace(m_pData[cur_pos])
			))
		{
			if (m_pData[cur_pos] == '\n') cur_line++;
			cur_pos++;
		}
		if (cur_pos == m_nTotalSize) break;

		if (IsOperator(m_pData[cur_pos]))
		{
			m_Tokens.push_back(CToken(std::wstring(&m_pData[cur_pos], 1), cur_pos, cur_line));
			cur_pos++;
		}
		else
		{
			int temp_pos = cur_pos;
			if (m_pData[temp_pos] == '"')
			{
				int num_chars = 0;
				temp_pos++;
				while (temp_pos < m_nTotalSize)
				{
					if (m_pData[temp_pos] == '"')
					{
						if (temp_pos+1>=m_nTotalSize || m_pData[temp_pos+1] != '"') break;
						else
						{
							temp_pos++;
						}
					}
					temp_pos++;
					num_chars++;
				}
				if (temp_pos == m_nTotalSize)
				{
					WriteError(L"Missing L following L -begin");
					break;
				}

				assert(num_chars < TOKEN_BUFF_LEN);
				//				char *temp = NTL_NEW char[num_chars+1];
				m_pTemp[num_chars] = 0;
				int ofs = 1;
				for (int i=0;i<num_chars;i++)
				{
					m_pTemp[i] = m_pData[cur_pos+ofs+i];
					if (m_pData[cur_pos+ofs+i] == '"') 
					{
						ofs++;
					}
				}
				cur_pos = temp_pos+1;

				m_Tokens.push_back(CToken(std::wstring(m_pTemp), cur_pos, cur_line));
				//				delete temp;
			}
			else
			{
				while (temp_pos < m_nTotalSize &&
					!IsSpace(m_pData[temp_pos]) &&
					!IsOperator(m_pData[temp_pos]) &&
					!IsRemark(m_pData[temp_pos], cur_pos))
				{
					temp_pos++;
				}
				int tok_size = temp_pos-cur_pos;
				m_Tokens.push_back(CToken(std::wstring(&m_pData[cur_pos], tok_size), cur_pos, cur_line));
				cur_pos += tok_size;
			}
		}
	}
}

std::wstring CComponentTokenizerW::PeekNextToken(int *offset/*=NULL*/)
{
	if (m_nPeekPos >= (int) m_Tokens.size())
	{
		return L"";
	}
	if (offset != NULL) *offset = m_Tokens[m_nPeekPos].offset;
	return m_Tokens[m_nPeekPos++].token;
}


std::wstring CComponentTokenizerW::GetNextToken(int *offset/*=NULL*/)
{
	m_nPeekPos = 0;
	m_nLastLine = m_Tokens[0].line;
	std::wstring token = m_Tokens[0].token;
	if (offset != NULL) *offset = m_Tokens[0].offset;

	m_Tokens.pop_front();

	return token;
}

VOID CComponentTokenizerW::PopToPeek(VOID)
{
	while (m_nPeekPos > 0)
	{
		m_nPeekPos--;
		m_Tokens.pop_front();
	}
}


std::wstring CComponentTokenizerW::WriteError(std::wstring err_msg)
{
	std::wstring str = L"component tokenizer Tokenize() error";
	return str;
}

END_GUI